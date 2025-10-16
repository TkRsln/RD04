//#include <RoboV2_ESP.h>

#include <ESP8266WiFi.h>
#include <RoboV2_ESP.h> # This is my custom library
#include <WiFiUdp.h>
#include <Wire.h>

const char* ssid = "<ssid>"; //Enter SSID
const char* pass = "<password>"; //Enter Password


unsigned int udpPort = 2311; // local port to listen for UDP packets


// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
MiWire wr;

char packet[20];


const byte ROBOT_STATUS_STAND = 5;
const byte ROBOT_STATUS_LAY = 6;
const byte ROBOT_STATUS_DETACH = 7;
const byte ROBOT_STATUS_GUARD = 8;

bool robot_ready = false;
byte robot_user_requested_status = ROBOT_STATUS_LAY;
byte robot_pico_status = ROBOT_STATUS_LAY;


void setup()
{
  Serial.begin(115200);
  Wire.begin(5);
  wr.begin(Wire);
  wr.setMiniListener(onMiniUpdate);

  WiFi.begin(ssid, pass);
  
  while ( WiFi.status() != WL_CONNECTED) {
    wr.sendStatus(wr.STATUS_CONNECTING);
    delay(3000);
  }
  delay(250);
  wr.changeLedColor(1000, 2,wr.LED_EFFECT_SPIRAL, 10, 180, 50, 500); //DURATION, REPEAT, EFFECT&ANIMATION, R,G,B, DELAY
  delay(500);
  wr.changeLedColor(1000, 2,wr.LED_EFFECT_SPIRAL, 10, 180, 50, 500); //DURATION, REPEAT, EFFECT&ANIMATION, R,G,B, DELAY
  
  wr.setHeadLostListener(OnHeadUnplugged);
  wr.setHeadFoundListener(OnHeadFound);
  //Serial.println("[setup] WiFi connected");
  //Serial.print("[setup] IP address: ");
  //Serial.println(WiFi.localIP());
  
  udp.begin(udpPort);
}
// ASSUME THAT WE RECEIVE 20 BYTES per loop
//int* dt = new int[4];

const byte REQUEST_UPDATE=6;
const byte REQUEST_MOVE=2;
const byte REQUEST_CHANGE_ROBOT_STATUS = 1;
const byte REQUEST_HELLO = 73; // its a broadcast msg
const byte REQUEST_HEAD=7;

void loop() {
  wr.loop_wire();

  #pragma region  UDP AREA
  
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(packet, 20);
    if (len > 0)
    {
      byte id = packet[0];
      //Serial.print("Received: ");
      //Serial.println(id);
      float m_d,m_l,d_d,d_l;
      switch (id) {
        case REQUEST_CHANGE_ROBOT_STATUS:
          //TODO
          
          statusChangeDemand(packet[1]);
          updateUser();
          break;
        case REQUEST_MOVE:
        
          byte buffer[4];
          for(int i=0;i<4;i++)
            buffer[i]=packet[1+i];
          m_d=decodeFromBytes(buffer);
      
          for(int i=0;i<4;i++)
            buffer[i]=packet[5+i];
          m_l=decodeFromBytes(buffer);
          
      
          for(int i=0;i<4;i++)
            buffer[i]=packet[13+i];
          d_l=decodeFromBytes(buffer);
          
          for(int i=0;i<4;i++)
            buffer[i]=packet[9+i];
          d_d=decodeFromBytes(buffer);
          moveFunction(m_d/PI*180,m_l,d_d/PI*180,d_l,buffer);
          

          updateUser();
          break;
        case REQUEST_UPDATE:
          updateUser();
          break;
        case REQUEST_HELLO:
          udp.beginPacket(udp.remoteIP(), udp.remotePort());
          udp.print("Im here!");
          udp.endPacket();
          break;

        case REQUEST_HEAD:
          udp.beginPacket(udp.remoteIP(), udp.remotePort());
          udp.write(wr.head_version);
          udp.endPacket();
          break;
      }
      //updateUser();
    }
  }
  #pragma endregion
  
}
void OnHeadFound(byte data){
  if(data==1){
    wr.changeLedColor(500, 2, wr.LED_EFFECT_MOVING, 0, 50, 0, 100);
  }
}
void OnHeadUnplugged(){
  wr.changeLedColor(500, 2, wr.LED_EFFECT_CONSTANT, 100, 0, 0, 500);
}

String move="";
void moveFunction(float m_d,float m_l,float d_d,float d_l, byte angle[]){
  move=(String(REQUEST_MOVE)+":"+String(m_d)+":"+String(m_l)+":"+String(d_d)+":"+String(d_l))+"#";
  Serial.print(move);
  /*if(wr.head_version==1){
    Wire.beginTransmission(10);
    Wire.write(1); //MOVE_STATUS
    Wire.write(angle[0]);
    Wire.write(angle[1]);
    Wire.write(angle[2]);
    Wire.write(angle[3]);
    Wire.endTransmission();
  }*/
}


/*
void moveFunction(char packet[]){
//  for(int i=1;i<17;i++)
//    Serial.write(packet[i]);
  /*Serial.write(packet);
    Serial.write('\n');
  move="";
  for(int i=0;i<17;i++){
    move+=String((int)packet[i])+(i!=16?":":"");
  }
  Serial.println(move);
  //Serial.write(move);
  Serial.flush();
  
}*/

long last_status_change = 0;
short status_change_delay=750;
void statusChangeDemand(byte status){
  if(millis()-last_status_change < status_change_delay)return;
  if(robot_pico_status!=status && status != robot_user_requested_status){
    Serial.print(String(REQUEST_CHANGE_ROBOT_STATUS)+":"+String(status)+"#");
    Serial.flush();
    robot_ready=false;
    robot_user_requested_status=status;
    last_status_change=millis();
  }
}


          /*
            1- Robot status (ready or not)
            2- Robot status (Detach/Lay/Stand)
            3- All datas (is there obstacle etc...)
            4- Voltage
            5- distance
                            total = 11 Byte
          */
          
void updateUser(){
          udp.beginPacket(udp.remoteIP(), udp.remotePort());
          udp.write(REQUEST_UPDATE);
          udp.write(robot_ready);
          udp.write(robot_pico_status);
          udp.write(wr.all_data);
          
          /*
          byte* arr = coder((int)(100.5f*1000));

          udp.write(arr[0]);
          udp.write(arr[1]);
          udp.write(arr[2]);
          udp.write(arr[3]);
          
          arr = coder((int)(100.7f*1000));
          udp.write(arr[0]);
          udp.write(arr[1]);
          udp.write(arr[2]);
          udp.write(arr[3]);
*/
       
          int v = (int)((wr.value_voltage-7)*100);
          v=v>255?255:(v<0?0:v);
          udp.write(v);

          v=(int)((wr.value_distance*10));
          v=v>255?255:(v<0?0:v);
          udp.write(v);
          /*
          udp.write(wr.voltage_arr[0]);
          udp.write(wr.voltage_arr[1]);
          udp.write(wr.voltage_arr[2]);
          udp.write(wr.voltage_arr[3]);
          udp.write(wr.distance_arr[0]);
          udp.write(wr.distance_arr[1]);
          udp.write(wr.distance_arr[2]);
          udp.write(wr.distance_arr[3]);
          */
          udp.endPacket();
          //Serial.println("User got update!");
}


void onMiniUpdate(){
  //TODO, check for obstacle -> later
}
float decodeFromBytes(byte data[]){
  return (decoder(data)*1.0f)/1000.0f;
}

int decoder(byte data[]){
  int value = 0;
  bool isMinus=(data[3]>>7)&1;

		for(int i=30;i>=0;i--) {
			int byte_index = i/8;
			int shift_level = i%8;
			if(((data[byte_index]>>shift_level)&1)==1)
				value+=pow(2, i);
		}
		return value*(isMinus?-1:1);
}

/*
 //TODO
          byte buffer[4];
          for(int i=0;i<4;i++)
            buffer[i]=packet[1+i];
          memcpy(&flt, buffer, sizeof(flt));
          Serial.print(flt);
          Serial.print("/");
          flt=0;
          for(int i=0;i<4;i++)
            buffer[i]=packet[5+i];
          memcpy(&flt, buffer, sizeof(flt));
          Serial.print(flt);
          Serial.print("/");
          flt=0;
          for(int i=0;i<4;i++)
            buffer[i]=packet[9+i];
          memcpy(&flt, buffer, sizeof(flt));
          Serial.print(flt);
          Serial.print("/");
          flt=0;
          for(int i=0;i<4;i++)
            buffer[i]=packet[12+i];
          memcpy(&flt, buffer, sizeof(flt));
          Serial.println(flt);
*/
