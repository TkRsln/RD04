

//////////////////////////////[Voltage Sensor]
/*
	[/!\] This code might read wrong! need to recalculate... 
*/
#define voltage_sensor_pin A0
#define voltage_const 30000
#define voltage_correction_factor 7500

float voltage_value=0;

float getVoltage(){
  voltage_value = analogRead(voltage_sensor_pin);    // read the current sensor value (0 - 1023) 
  voltage_value = (voltage_value / 1024.0f) * 5.0f;             // convert the value to the real voltage on the analog pin
  voltage_value =  voltage_value * 5.128;  
  return voltage_value;  
}
void voltageSetup(){
  pinMode(voltage_sensor_pin, INPUT);
}
//////////////////////////////
//////////////////////////////[Motion Sensor]
/*
Simply activates RCWL sensor via pin 2. and read its data with A1

	[/!\] RCWL's VCC pin directly connected to arduino's pin 2. Arduino can supply enogh power but this is absolutely not recomended! (try to use RCWL with simple Transistor)
	
	[/!\] RCWL-0516 uses less than 3 mA in order to https://lastminuteengineers.com/rcwl0516-microwave-radar-motion-sensor-arduino-tutorial/#:~:text=self-contained%20unit.-,Power,current%20to%20power%20external%20circuits
	[/!\] For Arduino, les then 20 mA is safe for arduino, https://forum.arduino.cc/t/maximum-pin-current/1145859

*/
#define motion_sensor_activate_pin 2
#define motion_sensor_pin A1

void motionSetup(){
  pinMode(motion_sensor_activate_pin,OUTPUT);
  pinMode(motion_sensor_pin, INPUT);
  digitalWrite(motion_sensor_activate_pin,LOW);
}
void motionEnable(){
  digitalWrite(motion_sensor_activate_pin,HIGH);
}
void motionDisable(){
  digitalWrite(motion_sensor_activate_pin,LOW);
}
bool getMotion(){
  return digitalRead(motion_sensor_pin);
}
//////////////////////////////
//////////////////////////////[WS2812B]
/*

Robots animative LEDs codes

*/

#include <FastLED.h>
#define led_pin 11
#define led_numbers 8
CRGB leds[led_numbers];

void ledSetup(){
  FastLED.addLeds<WS2812, led_pin, GRB>(leds, led_numbers);
}
/*
  leds[0] = CRGB(255, 0, 0);
  FastLED.show();
  delay(500);  
  leds[1] = CRGB(0, 255, 0);
  FastLED.show();
  delay(500);
  leds[2] = CRGB(0, 0, 255);
  FastLED.show();
  delay(500);
 */
//////////////////////////////
//////////////////////////////[OLED Display]
/*
const unsigned char BatteryV1 [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x06, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};
*/

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool i2c_esp=false;
bool i2c_pico=false;
bool ready=false;

void oledSetup(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}
void oledClear(){
  display.clearDisplay();
  display.display();
}
// range is between 0 and 10
void oledShowBattery(int level){
  display.clearDisplay();
  display.setTextSize(2); 
  //display.drawBitmap(0, 1, BatteryV1, 128, 32, WHITE);
  //display.setCursor(1,2);
  display.setTextColor(WHITE);
  String tmp ="";
  for(int y=1;y<5;y++){
  display.setCursor(1,3+y*2);
    for(int x=0;x<level;x++)
      tmp+=((char)220);
    display.println(tmp);
    tmp="";
  }
  display.println(""); 

  display.display();//Finally display the created image
}
void oledShowMsg(String msg){
  display.clearDisplay();
  display.setTextSize(2);   
  display.setCursor(0,1);
  display.setTextColor(WHITE);
  display.println(msg); 
  display.display();//Finally display the created image
}
void oledShowSplash(){
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);    
  display.setCursor(20,5);
  display.println("RD-04"); 
  display.setTextSize(1);    
  display.setCursor(5,5);
  display.println("TK"); 
  display.display();

}
void oledShowI2C(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);      
  display.setCursor(0,1);  
  display.println("I2C Status:");    
  display.setCursor(7,11);  
  display.println(String("Esp  ")+String(i2c_esp?"+":"."));   
  display.setCursor(7,22);  
  display.println(String("Pico ")+String(i2c_pico?"+":".")); 
  display.setCursor(60,17);  
  display.println(String((i2c_esp&&i2c_pico)?"Granted":"Searching..")); 
  display.display();

}

//////////////////////////////




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  //ledSetup();
  Wire.begin(4);
  pinMode(13,OUTPUT);
  oledSetup();
  voltageSetup();
  //oledTest();
  Wire.onReceive(receiveEvent); // register event
  oledShowSplash();

  digitalWrite(13,HIGH);
  delay(750);
  digitalWrite(13,LOW);
}

long i2c_received_time=3000;
byte i2c_code = 1;
String i2c_msg = ""; 

void loop() {
  
  if(i2c_code==1){
    if(i2c_received_time==0){
      oledShowSplash();
      i2c_received_time=millis();
    }
  }
  else if(i2c_code==2){
    if(i2c_received_time==0){
      float volt = getVoltage();
      volt= volt-5.75f; //min=5.75 max=7.5
      volt = volt<0?0:(volt>1.75f?1.75f:volt);
      volt = volt*10/1.75f;
      oledShowBattery((int)volt);
      i2c_received_time=millis();
    }
  }else if(i2c_code==3){
    if(i2c_received_time==0){
      oledShowMsg(i2c_msg);
      i2c_msg="";
      i2c_received_time=millis();
    }
  }else if(i2c_code==4){
    if(i2c_received_time==0){
      i2c_esp=true;
      oledShowI2C();
      i2c_received_time=millis();
    }
  }else if(i2c_code==5){
    if(i2c_received_time==0){
      i2c_pico=true;
      oledShowI2C();
      i2c_received_time=millis();
    }
  }
  if(millis()-i2c_received_time>5000){
    if(!(i2c_esp&&i2c_pico)){
      oledShowI2C();
      i2c_received_time=millis();
      
      Wire.beginTransmission(5);
      Wire.write((byte)1); //PINGING
      Wire.endTransmission();
      delay(10);
      Wire.beginTransmission(6);
      Wire.write((byte)1); //PINGING
      Wire.endTransmission();
    }else if(!ready){
      ready=true;
      Wire.beginTransmission(5);
      Wire.write((byte)2); //READY
      Wire.endTransmission();
      delay(10);
      Wire.beginTransmission(6);
      Wire.write((byte)2); //READY
      Wire.endTransmission();
    }
    else if(i2c_code!=0 ){
      oledClear();
    }
  }
}


//////////////////////////////i2c 


/*
SEND CODES
1:WAITING
2:READY

RECEIVED CODES
0:NONE
1:SPLASH
2:BATTERY
3:MSG
4:ESP_GRANTED
5:PICO_GRANTED
*/
void receiveEvent(int howMany)
{
  i2c_code = Wire.read();
  if(i2c_code==3){
    i2c_msg="";
    while(0 < Wire.available()) // loop through all but the last
      i2c_msg+= (char)Wire.read(); // receive byte as a character
  }else{
    while(0 < Wire.available()) // loop through all but the last
      Serial.print(" "); // receive byte as a character
      Serial.print((int)Wire.read());
  }
  i2c_received_time = 0;
  Serial.print("W:");
  Serial.println((int)i2c_code);
  //digitalWrite(13,HIGH);
  //delay(750);
  //digitalWrite(13,LOW);

  /*
  while(0 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }*/
} 

