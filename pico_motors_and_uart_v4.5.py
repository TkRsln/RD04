from machine import Pin, PWM
import time

class Servo:
    __servo_pwm_freq = 50
    __min_u16_duty = 1640 - 2 # offset for correction
    __max_u16_duty = 7864 - 0  # offset for correction
    min_angle = 0
    max_angle = 180
    current_angle = 0.001

    def __init__(self, pin):
        self.__initialise(pin)


    def update_settings(self, servo_pwm_freq, min_u16_duty, max_u16_duty, min_angle, max_angle, pin):
        self.__servo_pwm_freq = servo_pwm_freq
        self.__min_u16_duty = min_u16_duty
        self.__max_u16_duty = max_u16_duty
        self.min_angle = min_angle
        self.max_angle = max_angle
        self.__initialise(pin)


    def move(self, angle):
        # round to 2 decimal places, so we have a chance of reducing unwanted servo adjustments
        angle = round(angle, 2)
        # do we need to move?
        if angle == self.current_angle:
            return
        self.current_angle = angle
        # calculate the new duty cycle and move the motor
        duty_u16 = self.__angle_to_u16_duty(angle)
        self.__motor.duty_u16(duty_u16)

    def __angle_to_u16_duty(self, angle):
        return int((angle - self.min_angle) * self.__angle_conversion_factor) + self.__min_u16_duty


    def __initialise(self, pin):
        self.current_angle = -0.001
        self.__angle_conversion_factor = (self.__max_u16_duty - self.__min_u16_duty) / (self.max_angle - self.min_angle)
        self.__motor = PWM(Pin(pin))
        self.__motor.freq(self.__servo_pwm_freq)


"""
LEG - FRONT LEFT                         Angle Info
> SHOULDER: PIN=19, MID=95              -> Higer:Close / Lower:Open
> UPPER LEG: PIN=20, MID=97             -> Higer:Up / Lower:Down
> LOWER LEG: PIN=18, MIN=142, MAX= 48|20    -> 142:Close / 20:Open

LEG - FRONT RIGHT                        Angle Info
> SHOULDER: PIN=12, MID=100             -> Higer:Close / Lower:Open
> UPPER LEG: PIN=11, MID=94             -> Higer:Down / Lower:UP
> LOWER LEG: PIN=10, MIN=141, MAX=70|47 -> 141:Close / 47:Open

LEG - BACK LEFT                          Angle Info
> SHOULDER: PIN=16, MID=100             -> Higer:Open / Lower: Close
> UPPER LEG: PIN=21, MID=92             -> Higer:Down / Lower:Up
> LOWER LEG: PIN=17, MIN=137, MAX=0     -> 141:Close / 47:Open

LEG - BACK RIGHT
> SHOULDER: PIN=15   MID=84                    -> Higer:Open
> UPPER LEG: PIN=14, MID=104                   -> Higer:Open
> LOWER LEG: PIN=13, MIN=141, MAX=45           -> 141:Close / 45:Open
"""
class Leg:
    
    def __init__(self,is_back,is_left):
        self.shoulder_ready=False 
        self.upper_ready=False
        self.lower_ready=False
        self.servos_armed=False
        self.shoulder_previous_angle=0
        self.upper_previous_angle=0
        self.lower_previous_angle=0
        self.shoulder_target_angle=0
        self.upper_target_angle=0
        self.lower_target_angle=0
        self.shoulder = None
        self.lower = None
        self.upper = None
        self.lower_duration=0
        self.lower_start_time=0
        self.upper_duration=0
        self.upper_start_time=0
        self.shoulder_duration=0
        self.shoulder_start_time=0
        self.shoulder_end_anim=True
        self.upper_end_anim=True
        self.lower_end_anim=True
        self.is_back=is_back
        self.is_left=is_left
    
    def setShoulderInfo(self,pin:int,mid_angle,lay_angle):
        self.shoulder_pin=pin
        self.shoulder_mid_angle=mid_angle
        self.shoulder_ready=True
        self.shoulder_end_anim=False 
        self.shoulder_lay_angle=lay_angle
        if self.is_all_setted():
            self.armServos()
        
    def setUpperLegInfo(self,pin:int, mid_angle,lay_angle):
        self.upper_pin=pin
        self.upper_mid_angle=mid_angle
        self.upper_ready=True
        self.upper_end_anim=False 
        self.upper_lay_angle=lay_angle
        if self.is_all_setted():
            self.armServos()
        
    def setLowerLegInfo(self,pin:int,min_angle,max_angle,lay_angle):
        self.lower_pin=pin
        self.lower_max_angle=max_angle
        self.lower_min_angle=min_angle
        self.lower_ready=True
        self.lower_end_anim=False 
        self.lower_lay_angle=lay_angle
        if self.is_all_setted():
            self.armServos()
            
    def calculateLower(self,angle): #angle must be between 0 and 90
        
        #print(angle," ",self.lower_min_angle," ",(self.lower_max_angle-self.lower_min_angle)," ",angle/90," ",(self.lower_min_angle+(self.lower_max_angle-self.lower_min_angle)*angle/90))
        angle=abs(90-angle)
        return self.lower_min_angle+(self.lower_max_angle-self.lower_min_angle)*angle/90
            
    def calculateReverseLower(self,angle): #angle must be between 0 and 90
        angle= 90-(angle-self.lower_min_angle)/(self.lower_max_angle-self.lower_min_angle)*90
        return angle
        #self.lower_min_angle+(self.lower_max_angle-self.lower_min_angle)*angle/90
    
    def calculateReverseUpper(self,angle):
        return (angle-self.upper_mid_angle)*(-1 if self.is_left else 1)
    
    def calculateReverseShoulder(self,angle):
        return (angle-self.shoulder_mid_angle)*(1 if self.is_back else -1)
        
    def armServos(self):
        self.shoulder = Servo(pin=self.shoulder_pin)
        self.upper = Servo(pin=self.upper_pin)
        self.lower = Servo(pin=self.lower_pin)
        
        
        self.lower_previous_angle=self.lower_lay_angle
        self.lower_target_angle=self.lower_lay_angle
        
        self.upper_previous_angle=self.upper_lay_angle
        self.upper_target_angle=self.upper_lay_angle
        
        self.shoulder_previous_angle=self.shoulder_lay_angle
        self.shoulder_target_angle=self.shoulder_lay_angle
        
        self.shoulder.move(self.shoulder_mid_angle+self.shoulder_lay_angle)
        self.upper.move(self.upper_mid_angle+self.upper_lay_angle)
        self.lower.move(self.calculateLower(self.lower_lay_angle))
        self.setShoulderAngle(self.shoulder_lay_angle,0)
        self.setUpperAngle(self.upper_lay_angle,0)
        self.setLowerAngle(self.lower_lay_angle,0)
        self.servos_armed=True
        
    def is_all_setted(self):
        if self.shoulder_ready and self.upper_ready and self.lower_ready:
            return True
        else:
            return False
        
    def setLowerAngle(self,angle,duration):
        self.lower_target_angle=angle
        self.lower_duration=duration
        self.lower_previous_angle=self.calculateReverseLower(self.lower.current_angle)
        self.lower_end_anim=False
        self.lower_start_time=time.ticks_ms()
        
    def setUpperAngle(self,angle,duration):
        self.upper_target_angle=angle
        self.upper_duration=duration
        self.upper_previous_angle=self.calculateReverseUpper(self.upper.current_angle)
        self.upper_end_anim=False
        self.upper_start_time=time.ticks_ms()
        
    def setShoulderAngle(self,angle,duration):
        self.shoulder_target_angle=angle
        self.shoulder_duration=duration
        self.shoulder_previous_angle=self.calculateReverseShoulder(self.shoulder.current_angle)
        self.shoulder_end_anim=False
        self.shoulder_start_time=time.ticks_ms()
        
    def setAngle(self,upper,lower,duration):
        self.setUpperAngle(upper)
        self.setLowerAngle(lower)
        
    def setAngle(self,shoulder,upper,lower,duration):
        self.setShoulderAngle(shoulder,duration)
        self.setUpperAngle(upper,duration)
        self.setLowerAngle(lower,duration)
        
    def is_leg_ready(self):
        if self.shoulder_end_anim and self.upper_end_anim and self.lower_end_anim:
            return True
        else:
            return False
        
    def loop(self):
        if not self.servos_armed:
            return
        
        if not self.shoulder_end_anim:
            dt=time.ticks_ms()-self.shoulder_start_time
            if dt<self.shoulder_duration:
                calculation = (self.shoulder_previous_angle+(self.shoulder_target_angle-self.shoulder_previous_angle)*(dt/self.shoulder_duration))*(1 if self.is_back else -1)
                self.shoulder.move(self.shoulder_mid_angle+calculation)
                #print("Shoulder Angle:",self.shoulder.current_angle)
            else:
                self.shoulder_previous_angle=self.shoulder_target_angle
                self.shoulder.move(self.shoulder_mid_angle + self.shoulder_previous_angle*(1 if self.is_back else -1))
                self.shoulder_end_anim=True
                #print("Shoulder end.")
                
        if not self.upper_end_anim:
            dt=time.ticks_ms()-self.upper_start_time
            if dt<self.upper_duration:
                calculation = (self.upper_previous_angle + (self.upper_target_angle-self.upper_previous_angle)*(dt/self.upper_duration))*(-1 if self.is_left else 1)
                self.upper.move(self.upper_mid_angle+calculation)
                #print("Upper Angle:",self.upper.current_angle)

            else:
                self.upper_previous_angle=self.upper_target_angle
                self.upper.move(self.upper_mid_angle + self.upper_previous_angle*(-1 if self.is_left else 1))
                self.upper_end_anim=True
                #print("Upper end.")
                
        if not self.lower_end_anim:
            dt=time.ticks_ms()-self.lower_start_time
            if dt<self.lower_duration:
                self.lower.move(self.calculateLower(self.lower_previous_angle+(self.lower_target_angle-self.lower_previous_angle)*(dt/self.lower_duration)))
                #print("Lower Angle:",self.lower.current_angle)

            else:
                self.lower_previous_angle=self.lower_target_angle
                self.lower.move(self.calculateLower(self.lower_previous_angle))
                self.lower_end_anim=True
                #print("Lower end. ",self.lower.current_angle)

class BodyTest:
    
    LAY=0
    STAND_STAGE_1 = 1
    STAND=2
    
    def __init__(self):
        self.fl = Leg(is_back=False,is_left=True)
        self.fl.setShoulderInfo(19,92,0)
        self.fl.setUpperLegInfo(20,97,5)
        self.fl.setLowerLegInfo(18,30,142,0)
        
        self.fr = Leg(is_back=False,is_left=False)
        self.fr.setShoulderInfo(12,100,0)
        self.fr.setUpperLegInfo(11,94,5)
        self.fr.setLowerLegInfo(10,48,141,0)
        
        self.bl = Leg(is_back=True,is_left=True)
        self.bl.setShoulderInfo(16,100,0)
        self.bl.setUpperLegInfo(21,92,5)
        self.bl.setLowerLegInfo(17,0,137,0)
        
        self.br = Leg(is_back=False,is_left=False)
        self.br.setShoulderInfo(15,84,0)
        self.br.setUpperLegInfo(14,104,5)
        self.br.setLowerLegInfo(13,45,141,0)
        
        self.status=BodyTest.LAY
        self.status_target=BodyTest.LAY
        
        self.shoulder=[0,0,0]
        self.upper=[5,0,24]
        self.lower=[0,0,55]
        
    def is_legs_ready(self):
        if self.fr.is_leg_ready() and self.fl.is_leg_ready() and self.br.is_leg_ready() and self.bl.is_leg_ready():
            return True
        else:
            return False
        
    def setLegs(self,shoulder,upper,lower,duration):
        self.fl.setAngle(shoulder,upper,lower,duration)
        self.fr.setAngle(shoulder,upper,lower,duration)
        self.bl.setAngle(shoulder,upper,lower,duration)
        self.br.setAngle(shoulder,upper,lower,duration)
        
    def stand(self):
        self.status_target=BodyTest.STAND
        
    def lay(self):
        self.status_target=BodyTest.LAY
        
    def loop(self):
        
        if self.status != self.status_target:
            if self.status_target == BodyTest.STAND:
                if self.status == BodyTest.LAY:
                    if self.is_legs_ready():
                        self.status=BodyTest.STAND_STAGE_1
                        self.setLegs(self.shoulder[self.status],self.upper[self.status],self.lower[self.status],2500)
                elif self.status == BodyTest.STAND_STAGE_1:
                    if self.is_legs_ready():
                        self.status=BodyTest.STAND
                        self.setLegs(self.shoulder[self.status],self.upper[self.status],self.lower[self.status],1000)
                        
            elif self.status_target == BodyTest.LAY:
                if self.status == BodyTest.STAND:
                    if self.is_legs_ready():
                        self.status=BodyTest.LAY
                        self.setLegs(self.shoulder[self.status],self.upper[self.status],self.lower[self.status],1500)
                        
        
        self.fl.loop()
        self.fr.loop()
        self.bl.loop()
        self.br.loop()
        
        
            
        
                
   
   
"""
LEG - FRONT LEFT                         Angle Info
> SHOULDER: PIN=19, MID=95              -> Higer:Close / Lower:Open
> UPPER LEG: PIN=20, MID=97             -> Higer:Up / Lower:Down
> LOWER LEG: PIN=18, MIN=142, MAX= 48|20    -> 142:Close / 20:Open

LEG - FRONT RIGHT                        Angle Info
> SHOULDER: PIN=12, MID=100             -> Higer:Close / Lower:Open
> UPPER LEG: PIN=11, MID=94             -> Higer:Down / Lower:UP
> LOWER LEG: PIN=10, MIN=141, MAX=70|47 -> 141:Close / 47:Open

LEG - BACK LEFT                          Angle Info
> SHOULDER: PIN=16, MID=100             -> Higer:Open / Lower: Close
> UPPER LEG: PIN=21, MID=92             -> Higer:UP / Lower:Up
> LOWER LEG: PIN=17, MIN=137, MAX=0     -> 141:Close / 47:Open

LEG - BACK RIGHT
> SHOULDER: PIN=15   MID=84                    -> Higer:Open
> UPPER LEG: PIN=14, MID=104                   -> Higer:DOWN
> LOWER LEG: PIN=13, MIN=141, MAX=45           -> 141:Close / 45:Open
"""
b = BodyTest()

#b.loop()
#time.sleep(2.5)

#b.stand()


# Define UART pins on Raspberry Pi Pico
uart = machine.UART(0, baudrate=115200, tx=0, rx=1)

movement_demand=[0,0,0,0]
movement_robot=[0,0,0,0]

REQUEST_MOVE='2';
REQUEST_CHANGE_ROBOT_STATUS = '1';


ROBOT_STATUS_STAND = 5;
ROBOT_STATUS_LAY = 6;
ROBOT_STATUS_DETACH = 7;
ROBOT_STATUS_GUARD = 8;

robot_demand_status = ROBOT_STATUS_LAY
robot_current_status = ROBOT_STATUS_LAY



while True:
    # Send data to ESP8266
    
    # Receive data from ESP8266
    if uart.any():
        c=uart.read().decode("utf-8")
        if c.startswith(REQUEST_MOVE):
            movement = [float(element) for element in c[2:].split('#')[0].split(':')]
            print(movement)
            
            
        elif c.startswith(REQUEST_CHANGE_ROBOT_STATUS):
            robot_status = int(c[2:].split('#')[0])
            print(robot_status)
            if robot_status == ROBOT_STATUS_STAND:
                b.stand()
                print("STAND")
            elif  robot_status == ROBOT_STATUS_DETACH:
                b.lay()
                print("LAY")
                
    b.loop()
    #print("L")
    time.sleep(0.05)


"""


leg_left_front = Leg(is_back=False,is_left=True)
leg_left_front.setShoulderInfo(19,97,0)
leg_left_front.setUpperLegInfo(20,97,5)
leg_left_front.setLowerLegInfo(18,48,142,0)



leg_left_front.loop()
time.sleep(2.5)
#leg_left_front.setAngle(5,0,45,2500)
#leg_left_front.setShoulderAngle(5,2500)

while True:
    leg_left_front.loop()


#motor=Servo(pin=14) # A changer selon la broche utilisée
#motor.move(120) # tourne le servo à 0°
leg_left_front = Leg(is_back=False,is_left=True)
leg_left_front.setShoulderInfo(19,97,0)
leg_left_front.setUpperLegInfo(20,97,5)
leg_left_front.setLowerLegInfo(18,48,142,0)

leg_left_front.loop()
time.sleep(2.5)
#leg_left_front.setLowerAngle(0,0)

while True:
    leg_left_front.loop()
"""
#time.ticks_ms() - start