#include<RoboV2_ESP.h>
#include<Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

MiWire::MiWire(){}

void MiWire::begin(TwoWire &wireObj){
	wire=wireObj;
}
/*
	HEAD_ADRESS = 10
*/
void MiWire::loop_wire(){
	if(millis()-last_demand>=delay){
		Wire.requestFrom(8, 10);
		
	  while (Wire.available()) { // slave may send less than requested
		//buffer[count]=Wire.read(); // receive a byte as character
		//count++;
		all_data = Wire.read();
		screen_data = Wire.read();
		voltage_arr[0] = Wire.read();
		voltage_arr[1] = Wire.read();
		voltage_arr[2] = Wire.read();
		voltage_arr[3] = Wire.read();
		distance_arr[0] = Wire.read();
		distance_arr[1] = Wire.read();
		distance_arr[2] = Wire.read();
		distance_arr[3] = Wire.read();
	  }
		
	  memcpy(&value_voltage, voltage_arr, sizeof(float));
	  memcpy(&value_distance, distance_arr, sizeof(float));
	  // SEPARATING DATA FROM BYTES
	  is_obstacle_close = (all_data >> 0) & 1; // 1. bit
	  is_voltage_low = (all_data >> 1) & 1; // 2. bit
	  is_charging = (all_data >> 2) & 1; // 3. bit
	  is_motion_active = (all_data >> 3) & 1; // 4. bit
	  is_there_motion = (all_data >> 4) & 1; // 5. bit

	  //Serial.println(String(value_voltage)+"-"+String(value_distance)
	  //                  +"/"+String(b1)+"-"+String(b2)+"-"+String(b3)+"-"+String(b4)+"-"+String(b5));
		
		if(OnUpdateByMiniFunction!=nullptr)
			OnUpdateByMiniFunction();
		last_demand=millis();
	}
	else if(false &&(millis()-last_demand_cam >= delay_head)){
		Wire.requestFrom(10, 1);
		short found = -1;
		while (Wire.available()) {
			head_version=Wire.read();
			found = head_version;
		}
		
		if(!head_active && found!=-1){
			head_active=true;
			if(HeadFoundFunction!=nullptr){
				HeadFoundFunction((byte)head_version);
			}
		}else if(head_active && found==-1){
			head_active=false;
			if(HeadLostFunction!=nullptr){
				HeadLostFunction();
			}
		}
	}
	
}
void MiWire::setMiniListener(InformFunction f){
	OnUpdateByMiniFunction=f;
}

void MiWire::sendStatus(byte status){
  Wire.beginTransmission(8);
  Wire.write(status);
  Wire.endTransmission();
}
void MiWire::setHeadFoundListener(InformByteFunction f){
	HeadFoundFunction=f;
}
void MiWire::setHeadLostListener(InformFunction f){
	HeadLostFunction=f;
}
void MiWire::changeLedColor(short duration, byte repeat, byte  effect, byte R, byte G, byte B, short delay){
  Wire.beginTransmission(8);
  Wire.write(CHANGE_LED);
  Wire.write((byte)(duration/100));
  Wire.write(repeat);
  Wire.write(effect);
  Wire.write(R);
  Wire.write(G);
  Wire.write(B);
  Wire.write((byte)(delay/100));
  Wire.endTransmission();
}


//Networking::Networking(){}
/*
void Networking::begin(char* ssid,char* pass, MiWire &wire){
  wr=wire;
  WiFi.begin(ssid, pass);
  while ( WiFi.status() != WL_CONNECTED) {
	  if(OnWifiStatusChange!=nullptr)
		OnWifiStatusChange(WIFI_STATUS_CONNECTING)
    delay(1000);
  }
  
	  if(OnWifiStatusChange!=nullptr)
		OnWifiStatusChange(WIFI_STATUS_CONNECTED)
  
  udp.begin(udpPort);
}

void Networking::loop_network(){
	
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
          //robot_ready=false;
          //robot_user_requested_status=packet[1];
		  if(OnRobotStatusChangeDemandF!=nullptr)
			  OnRobotStatusChangeDemandF(packet[1]);
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
            buffer[i]=packet[9+i];
          d_d=decodeFromBytes(buffer);
      
          for(int i=0;i<4;i++)
            buffer[i]=packet[13+i];
          d_l=decodeFromBytes(buffer);
		  
		  if(OnMoveF !=nullptr)
			OnMoveF();
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
      }
    }
  }
}

void Networking::setOnWifiListener(InformFunction f){
	OnWifiStatusChange=f;
}

void Networking::setOnMoveListener(OnMoveFunction f){
	OnMoveF=f;
}

void Networking::updateUser(){
	
          udp.beginPacket(udp.remoteIP(), udp.remotePort());
          udp.write(REQUEST_UPDATE);
          udp.write(robot_ready);
          udp.write(robot_pico_status);
          udp.write(wr.all_data);
          
          int v = (int)((wr.value_voltage-7)*100);
          v=v>255?255:(v<0?0:v);
          udp.write(v);

          v=(int)((wr.value_distance*10));
          v=v>255?255:(v<0?0:v);
          udp.write(v);
          udp.endPacket();
}


float Networking::decodeFromBytes(byte data[]){
  return (decoder(data)*1.0f)/1000.0f;
}

int Networking::decoder(byte data[]){
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
void Networking::setRobotStatusChangeListener(InformFunction f){
	OnRobotStatusChangeDemandF=f;
}*/








