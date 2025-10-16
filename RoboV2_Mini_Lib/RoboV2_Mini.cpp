
#include <RoboV2_Mini.h>
#include<Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <FastLED.h>

////////////////////////// BUTTON 
void Button::setButtonShortListener(ButtonFunctionPointer f){
  button_short_function=f;
}

void Button::setButtonLongListener(ButtonFunctionPointer f){
  button_long_function=f;
}
Button::Button(){
	pin=A6;
	pinMode(pin,INPUT);
}
Button::Button(int _pin){
	pin=_pin;
	pinMode(pin,INPUT);
}

void Button::loop_button(){
  button_current_status = analogRead(pin)>500?1:0;
  if(button_previous_status!=button_current_status){
    if(button_current_status){
      button_down_time=millis();
    }else{
      if(millis() - button_down_time<250)return;
      if(millis() - button_down_time > button_long_duration){
        if(button_long_function!=nullptr)
          button_long_function();
      }
      else
        if(button_short_function!=nullptr)
          button_short_function();
    }
    button_previous_status=button_current_status;
  }
}


////////////////////////// DISTANCE SENSOR
SDistance::SDistance(){
  pin=A2;
  pinMode(A2,INPUT);
}

SDistance::SDistance(int _pin){
  pin=_pin;
  pinMode(pin,INPUT);
}
SDistance::SDistance(int _pin,int delay){
  pin=_pin;
  pinMode(pin,INPUT);
  distance_measure_delay=delay;
}

void SDistance::loop_distance(){
  if(millis()-distance_last_measure>distance_measure_delay){
	  distance+=(analogRead(pin)-distance)*distance_change_speed;
	  distance_voltage = distance/1024*5;
	  //Serial.print("V: ");
	  //Serial.print(distance_voltage);
	  float res = 0;
	  for(int i=1;i<8;i++){
		if(voltages_arr[i-1]>=distance_voltage && voltages_arr[i]<distance_voltage ){
		  //Serial.print(" I: ");
		  //Serial.print(i);
		  value = ((distance_voltage-voltages_arr[i])/(voltages_arr[i-1]-voltages_arr[i]));
		  value = distance_arr[i]-(distance_arr[i]-distance_arr[i-1])*value;
		  //res= distance_arr[i]-(distance_arr[i]-distance_arr[i-1])*res;
		  //break;
		}
		
	  }
	  distance_last_measure=millis();
	  if(value-0.5f>close_range && is_close){
		  if(FunctionFar!=nullptr)
			  FunctionFar(value);
		  is_close=false;
	  }else if(value+0.5f<close_range && !is_close){
		  if(FunctionClose!=nullptr)
			  FunctionClose(value);
		  is_close=true;
	  }
		  
  }
}

void SDistance::setCloseListener(DistanceFunctionPointer func){
	FunctionClose = func;
}

void SDistance::setFarListener(DistanceFunctionPointer func){
	FunctionFar=func;
}



////////////////////////// VOLTAGE SENSOR
/*
[TODO] Decrease de problem var kontrol et. 
*/
SVoltage::SVoltage(){
  pin=A0;
  pin_charge = A7;
  pinMode(pin,INPUT);
}
SVoltage::SVoltage(int _pin){
  pin=_pin;
  pin_charge = A7;
  pinMode(pin,INPUT);
}
SVoltage::SVoltage(int _pin,int _delay){
  pin=_pin;
  pin_charge = A7;
  pinMode(pin,INPUT);
  delay=_delay;
}
SVoltage::SVoltage(int _pin,int cpin,int _delay){
  pin=_pin;
  pin_charge = cpin;
  pinMode(pin,INPUT);
  delay=_delay;
}
void SVoltage::loop_voltage(){
  
  if(millis()-voltage_last_measure>delay){
	  value = analogRead(pin);    // read the current sensor value (0 - 1023) 
	  value = (value / 1024.0f) * 5.0f;             // convert the value to the real voltage on the analog pin
	  value =  value * 5.128;  
	  
	  
	  if(value>5){
		  ///////VOLTAGE
		  ///Decreasing
		  //if(value % warn_mod_value == 0 ){
		  if(fmod(value,warn_mod_value)<(warn_mod_value/3.0f) ){
			  if(!send_warn){
				  if(VoltageDecreaseFunctionF!=nullptr)
					VoltageDecreaseFunctionF(value);
				send_warn=true;
			  }
		  }else
			  send_warn=false;
		  ///Voltage Limit
		  if(value<=low_voltage){ // if too low
			  if(!send_low_warn){
				if(LowFunctionF!=nullptr)
					LowFunctionF(value);
				send_low_warn=true;
			  }
		  }else{
			  if(send_low_warn){
				  if(NormalFunctionF!=nullptr){
					 NormalFunctionF(value);
				  }
			  }
			  send_low_warn=false;
		  }
	  }
	  
	  //////CHARGE
	  // STARTED TO CHARGING
	  if(analogRead(pin_charge) < 500 && !is_charging){
		if(ChargePluggedFunctionF!=nullptr)
		  ChargePluggedFunctionF();
		is_charging=true;
		charge_complete=false;
	  }else if(analogRead(pin_charge) > 500 && is_charging){ // UNPLUGED CHARGE
		  if(ChargeUnpluggedF!=nullptr)
			ChargeUnpluggedF();
		is_charging=false;
	  }
	  if(is_charging && value>8 && !charge_complete){
		  if(ChargeCompleteF!=nullptr)
			ChargeCompleteF();
		charge_complete=true;
	  }
	  voltage_last_measure=millis();
	  
  }
  
  //return voltage_value;  
}

void SVoltage::setLowListener(VoltageFunction f){
	LowFunctionF=f;
}	//When the battery is critical low
void SVoltage::setNormalListener(VoltageFunction f){
	NormalFunctionF=f;
} //When the battery is no more low
void SVoltage::setVoltageDecreasingListener(VoltageFunction f){
	VoltageDecreaseFunctionF=f;
}
		
void SVoltage::setChargePluggedListener(ChargeFunction f){
	ChargePluggedFunctionF=f;
}
void SVoltage::setChargeUnpluggedListener(ChargeFunction f){
	ChargeUnpluggedF=f;
}
void SVoltage::setChargeCompleteListener(ChargeFunction f){
	ChargeCompleteF=f;
}



////////////////////////// MOTION SENSOR (RCWL)
SMotion::SMotion(){
	pin=A1;
	sensor_pin=2;
	  pinMode(sensor_pin,OUTPUT);
	  pinMode(pin, INPUT);
	disable();
}

SMotion::SMotion(int _pin,int _spin){
	pin=_pin;
	sensor_pin=_spin;
	  pinMode(sensor_pin,OUTPUT);
	  pinMode(pin, INPUT);
	disable();
}

void SMotion::disable(){
  digitalWrite(sensor_pin,LOW);
  is_enable=false;
}

void SMotion::enable(){
  digitalWrite(sensor_pin,HIGH);
  is_enable=true;
  ignore_first=true;
}

void SMotion::loop_motion(){
	if(!is_enable)return;
	if(millis()-last_motion_time>delay){
		
		if(previous_motion_status != digitalRead(pin)){
			if(digitalRead(pin)==1){					 	// IF There is motion
				if(!ignore_first&&MotionF!=nullptr)
					MotionF(true);
				
			}else{				
				if(MotionF!=nullptr && !ignore_first)
					MotionF(false);
				ignore_first=false;
			}
			
		}
		previous_motion_status=digitalRead(pin);
		last_motion_time=millis();
	}
}

void  SMotion::setMotionListener(MotionFunction f){
	MotionF=f;
}



///////////////////////////SCREEN
/*
TODO: HEAD FOUND MSG WILL SHOW UP
*/
Screen::Screen(TwoWire &Wire){
	display = Adafruit_SSD1306(128, 32, &Wire, -1);
	//display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}
//Screen::Screen(){}

void Screen::begin(){
	//display = Adafruit_SSD1306(128, 32, &Wire, -1);
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	show_splash();
}
	
void Screen::clear(){
  display.clearDisplay();
  display.display();
}

void Screen::loop_screen(){
  calculate_time_out();
  if(CURRENT_SCREEN_MODE==MODE_BATTERY_WARN){
    if(millis()/1000 !=screen_mod){
        screen_mod = millis()/1000;
        if(screen_mod%2==0){
          show_only_battery();
        }else{
          clear();
        }
    }
  }else if(CURRENT_SCREEN_MODE == MODE_CONNECTING){
    connection_loop();
  }
}
void Screen::show_splash(){
  if(CURRENT_SCREEN_MODE==MODE_SPLASH)
    return;
  duration = DURATION_SPLASH;
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);    
  display.setCursor(20,5);
  display.println("RD-04"); 
  display.setTextSize(1);    
  display.setCursor(5,5);
  display.println("TK"); 
  display.display();
  CURRENT_SCREEN_MODE=MODE_SPLASH;
}
void Screen::show_battery_warn(float _level){
  last_change=millis();
  duration=DURATION_BATTERY_WARN;
  CURRENT_SCREEN_MODE=MODE_BATTERY_WARN;
  level=_level;
}
void Screen::show_only_battery(){
	
  display.clearDisplay();
  display.setTextSize(2); 
  //display.drawBitmap(0, 1, BatteryV1, 128, 32, WHITE);
  //display.setCursor(1,2);
  display.setTextColor(WHITE);
  String tmp ="";
  int current_level=(level-7.05f)/(8-7.05f)*10; //(RANGE IS BETWEEN 6.80~8.00 volt)
  for(int y=1;y<5;y++){
  display.setCursor(1,3+y*2);
    for(int x=0;x<current_level;x++)
      tmp+=((char)220);
    display.println(tmp);
    tmp="";
  }
  display.setCursor(50,27);
  display.setTextSize(1); 
  //display.println(String(level)); 
  display.display();//Finally display the created image
}

void Screen::show_battery(float _level){
  level=_level;
  last_change=millis();
  if(CURRENT_SCREEN_MODE==MODE_BATTERY)
    return;
  duration=DURATION_BATTERY;
  show_only_battery();
  CURRENT_SCREEN_MODE=MODE_BATTERY;
}

void Screen::show_connection(){
  last_change=millis();
  duration=DURATION_CONNECTION;
  if(CURRENT_SCREEN_MODE==MODE_CONNECTING)
    return;
  CURRENT_SCREEN_MODE=MODE_CONNECTING;
}

void Screen::connection_loop(){
  if(millis()%2500/100 != screen_mod){
    screen_mod = millis()%2500/100;
    display.clearDisplay();
    display.setTextSize(1); 
    display.setCursor(27,15);
    display.print("Connecting...");
    display.setTextSize(2); 
    display.setTextColor(WHITE);
    for(int y=1;y<5;y++){
      display.setCursor(screen_mod*5,3+y*2);
        display.println((char)220);
    }
    display.display();
  }
}

void Screen::show_main_menu(){
  last_change=millis();
  CURRENT_SCREEN_MODE=MODE_MENU;
  menu_id=0;
  menu_selection=0;
  duration = DURATION_MENU;
  draw_menu();
}

void Screen::draw_menu(){
  display.clearDisplay();
  display.setTextColor(WHITE);
    //Serial.println("__MENU");
  if(menu_id==0){
    display.setTextSize(1); 
    display.setCursor(10,15);
    display.print(main_menu[0]);
    display.setCursor(77,15);
    display.print(main_menu[1]);

    display.setCursor(menu_selection==0?2:70,15);
    display.print(">");
    display.setCursor(menu_selection==0?47:120,15);
    display.print("<");
  }else if(menu_id==1){
    display.setTextSize(1); 
    display.setCursor(10,5);
    display.print(motion_menu[0]);
    display.setCursor(77,5);
    display.print(motion_menu[1]);
    display.setCursor(10,25);
    display.print(motion_menu[2]);
    display.setCursor(77,25);
    display.print(motion_menu[3]);

    display.setCursor(menu_selection%2==0?2:70,menu_selection/2==0?5:25);
    display.print(">");
    display.setCursor(menu_selection%2==0?47:110,menu_selection/2==0?5:25);
    display.print("<");
  }
  display.display();
}
void Screen::calculate_time_out(){
  if(duration==-1)return;
  if(millis()-last_change>duration){
	
    if(!(CURRENT_SCREEN_MODE==MODE_MENU || CURRENT_SCREEN_MODE==MODE_SPLASH) ){
      show_main_menu();
    }
	else{
	  CURRENT_SCREEN_MODE=MODE_SLEEP;
      clear();
      duration=-1;
    }
  }
}

void Screen::onNextButton(){
	
  if(CURRENT_SCREEN_MODE == MODE_SLEEP){
	show_main_menu();
	return;
  }
  else if(CURRENT_SCREEN_MODE == MODE_MENU ){
	//Serial.println(menu_selection);
    last_change=millis();
    if(menu_id==0)
      menu_selection=(menu_selection+1)%2;
    else if(menu_id==1){
      menu_selection=(menu_selection+1)%4;
    }
	draw_menu();
  }
}

void Screen::onAcceptButton(){
	if(CURRENT_SCREEN_MODE == MODE_SLEEP){
		show_main_menu();
		return;
	}
	else if(CURRENT_SCREEN_MODE == MODE_MENU ){
    last_change=millis();
    if(menu_id==0){
      if(menu_selection==0){
        menu_id=1;
        draw_menu();
      }else{
		  if(requestf!=nullptr)
			show_battery(requestf());
      }
    }else if(menu_id==1){
      if(menu_selection!=3){
		  if(selectionf!=nullptr)
			selectionf(menu_id,menu_selection);
      }
      show_main_menu();
    }
  }
}

void Screen::setRequestFunction(RequestFunction _requestf){
	requestf=_requestf;
}

void Screen::setMenuSelectionListener(OnScreenMenuSelectionFunction f){
	selectionf=f;
}

///////////////////////////WIRE

typedef void (*InformFunction)(byte[]);
byte EsWire::INFO_REFRESH_VALUE = 0;
byte EsWire::INFO_ON_CONNECTION = 5;
byte EsWire::INFO_ON_HEAD_FOUND = 6;
byte EsWire::INFO_CHANGE_RGB = 7;
InformFunction EsWire::EspInformFunction =nullptr;
bool EsWire::b1=false;
bool EsWire::b2=false;
bool EsWire::b3=false;
bool EsWire::b4=false;
bool EsWire::b5=false;
float EsWire::v=0;
float EsWire::d=10;
byte EsWire::screen_decision=0;
	
EsWire::EsWire(){}

void EsWire::begin(TwoWire &wireObj){
	wire=wireObj;
	wire.onReceive(onRc);
	wire.onRequest(onRq);
	
}
static void EsWire::onRc(int hm){
  byte d[20]; 
  d[0]=Wire.read();
  int i=1;
   while (Wire.available()) {
    d[i] = Wire.read();
	i++;
  }
  if(EspInformFunction !=nullptr){
	EspInformFunction(d);
  }
  
}
static void EsWire::onRq(){
	if(EspInformFunction !=nullptr){
		byte data[1];
		data[0]=INFO_REFRESH_VALUE;
		EspInformFunction(data);
	}

    byte other_data=0;
    other_data |= (b1 << 0); // 1. bit
    other_data |= (b2 << 1); // 2. bit
    other_data |= (b3 << 2); // 3. bit
    other_data |= (b4 << 3); // 4. bit
    other_data |= (b5 << 4); // 5. bit
  //////

  Wire.write(other_data);
  Wire.write(screen_decision);

  byte byt[4];
  memcpy(byt, &v, sizeof(float));
  Wire.write(byt[0]);
  Wire.write(byt[1]);
  Wire.write(byt[2]);
  Wire.write(byt[3]);

  byt[4];
  memcpy(byt, &d, sizeof(float));
  Wire.write(byt[0]);
  Wire.write(byt[1]);
  Wire.write(byt[2]);
  Wire.write(byt[3]);
  
  screen_decision=0;
}


void EsWire::setEspListener(InformFunction f){
		EspInformFunction=f;
}



///////////////////////
LEDControl::LEDControl(){
	FastLED.addLeds<WS2812, 11, GRB>(leds, 10);
	setEffect(600,1,EFFECT_MOVING,200,200,200);
}

void LEDControl::setOff(){
	
	setEffect(0,0,EFFECT_OFF,0,0,0);
}

void LEDControl::setEffect(short _duration,byte repeat,byte effect,byte _R,byte _G,byte _B){
			R=_R;
			G=_G;
			B=_B;
			duration=_duration;
			current_effect=effect;
			effect_repeat = repeat;
			last_update=millis();
			delay=0;
			if(effect!=current_effect)
				last_status_change=millis();
			
			
			if(effect == EFFECT_CONSTANT || effect==EFFECT_OFF){
				if(effect==EFFECT_OFF){
					R=0;
					G=0;
					B=0;
				}
				leds[0] = CRGB((R), (G), (B));
				leds[3] = CRGB((R), (G), (B));
				leds[1] = CRGB((R), (G), (B));
				leds[2] = CRGB((R), (G), (B));
				FastLED.show();		
			}
			current_effect=effect;
}
void LEDControl::setEffect(short _duration,byte repeat,byte effect,byte _R,byte _G,byte _B,short _delay){
	delay = _delay;
	setEffect(_duration,repeat,effect,_R,_G,_B);
}

void LEDControl::loop_led(){
	if(current_effect!=EFFECT_OFF){
			
		short dt = (millis()-last_status_change)%(duration+delay);
		
		if(current_effect == EFFECT_FADE){
			float x = 1-dt/(duration/2.0f);
			x = x<-1?-1:(x>1?1:x);
			x = (1-abs(x));
			
			leds[0] = CRGB((R*x), (G*x), (B*x));
			leds[3] = CRGB((R*x), (G*x), (B*x));
			leds[1] = CRGB((R*x), (G*x), (B*x));
			leds[2] = CRGB((R*x), (G*x), (B*x));
			
			
		}else if(current_effect == EFFECT_MOVING){
			float x = 1-dt/(duration/3.0f);
			float first = x<-1?-1:(x>1?1:x);
			first = (1-abs(first));
			
			x+=0.75f;
			float second = x<-1?-1:(x>1?1:x);
			second= 1-abs(second);
			
			leds[0] = CRGB((R*first), (G*first), (B*first));
			leds[3] = CRGB((R*first), (G*first), (B*first));
			leds[1] = CRGB((R*second), (G*second), (B*second));
			leds[2] = CRGB((R*second), (G*second), (B*second));
		}
		else if(current_effect == EFFECT_SPIRAL){
			float x = 1-dt/(duration/6.5f);
			float first = x<-1?-1:(x>1?1:x);
			first = (1-abs(first));
			
			
			x+=0.75f;
			float second = x<-1?-1:(x>1?1:x);
			second= 1-abs(second);
			x+=0.75f;
			float third = x<-1?-1:(x>1?1:x);
			third= 1-abs(third);
			x+=0.75f;
			float forth = x<-1?-1:(x>1?1:x);
			forth= 1-abs(forth);
			

			leds[0] = CRGB((R*first), (G*first), (B*first));
			leds[3] = CRGB((R*second), (G*second), (B*second));
			leds[1] = CRGB((R*third), (G*third), (B*third));
			leds[2] = CRGB((R*forth), (G*forth), (B*forth));
			
		}
		
		
		
		
		FastLED.show();		
		if(duration!=-1&&millis()-last_update>duration){
			effect_repeat--;
			if(effect_repeat<=0){
				setOff();
			}else{
				last_update=millis();
			}
		}
		
	}
	
}


/*

if(current_status == STATUS_STARTING){
		short dt = (millis()-last_status_change)%9000;
		float x = 1-dt/350.0f;
		float first = x<-1?-1:(x>1?1:x);
		first = (1-abs(first));
		
		
		x+=0.75f;
		float second = x<-1?-1:(x>1?1:x);
		second= 1-abs(second);
		
		byte r=30;
		byte g=30;
		byte b=30;

		leds[0] = CRGB((r*first), (g*first), (b*first));
		leds[3] = CRGB((r*first), (g*first), (b*first));
		leds[1] = CRGB((r*second), (g*second), (b*second));
		leds[2] = CRGB((r*second), (g*second), (b*second));
		FastLED.show();		
		
		if(millis()-last_update>4000){
			setStatus(STATUS_OFF);
		}
		
	}else if(current_status == STATUS_CONNECTING){
		short dt = (millis()-last_status_change)%9000;
		float x = 1-dt/750.0f;
		float first = x<-1?-1:(x>1?1:x);
		first = (1-abs(first));
		
		
		x+=0.75f;
		float second = x<-1?-1:(x>1?1:x);
		second= 1-abs(second);
		x+=0.75f;
		float third = x<-1?-1:(x>1?1:x);
		third= 1-abs(third);
		x+=0.75f;
		float forth = x<-1?-1:(x>1?1:x);
		forth= 1-abs(forth);
		
		byte r=10;
		byte g=100;
		byte b=200;

		leds[0] = CRGB((r*first), (g*first), (b*first));
		leds[3] = CRGB((r*second), (g*second), (b*second));
		leds[1] = CRGB((r*third), (g*third), (b*third));
		leds[2] = CRGB((r*forth), (g*forth), (b*forth));
		FastLED.show();		
		
		if(millis()-last_update>4000){
			setStatus(STATUS_OFF);
		}
		
	}else if(current_status == STATUS_MENU_SELECTION){
		short dt = (millis()-last_status_change)%2000;
		float x = 1-dt/500.0f;
		float first = x<-1?-1:(x>1?1:x);
		first = (1-abs(first));
		
		leds[0] = CRGB((byte)(255*first), (byte)(120*first), (byte)(50*first));
		leds[3] = CRGB((byte)(255*first), (byte)(120*first), (byte)(50*first));
		leds[1] = CRGB((byte)(255*first), (byte)(120*first), (byte)(50*first));
		leds[2] = CRGB((byte)(255*first), (byte)(120*first), (byte)(50*first));
		FastLED.show();		
		
		if(millis()-last_update>2000){
			setStatus(STATUS_OFF);
		}
		
	} 

*/



/*
void EsWire::onRequest(){
	char* arr="0000000000";
	if(requestDatas !=nullptr)
		arr=requestDatas();
	wire.write(arr);
}
void EsWire::onReceive(int hm){
	
}
void EsWire::setRequestListener(ReqFunction f){
	requestDatas=f;
}
void EsWire::setWire(TwoWire &_wire){
	wire=_wire;
}
EsWire::EsWire(){}
*/
