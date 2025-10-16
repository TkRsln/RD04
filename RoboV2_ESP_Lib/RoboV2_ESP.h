
#include<Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

class MiWire{
	public:

		typedef void (*InformFunction)();
		typedef void (*InformByteFunction)(byte);
		
		byte STATUS_CONNECTING =5;
		byte STATUS_HEAD_FOUND = 6;
		byte CHANGE_LED = 7;
		
		const byte LED_EFFECT_SPIRAL = 4;
		const byte LED_EFFECT_MOVING = 3;
		const byte LED_EFFECT_FADE = 2;
		const byte LED_EFFECT_CONSTANT = 1;
		const byte LED_EFFECT_OFF = 0;
			
		bool is_obstacle_close = false;
		bool is_voltage_low=false;
		bool is_charging=false;
		bool is_motion_active=false;
		bool is_there_motion=false;
		bool head_active=false;
		byte all_data=0;
		byte screen_data=0;
		float value_voltage=0.0f;
		float value_distance=0.0f;
		
		byte head_version=0;
	
		byte voltage_arr[4];
		byte distance_arr[4];
		
		int delay = 150;
		int delay_head = 5000;
		MiWire();
		void begin(TwoWire &wireObject);
		void loop_wire();
		
		void setMiniListener(InformFunction f);
		void setHeadFoundListener(InformByteFunction f);
		void setHeadLostListener(InformFunction f);
		void sendStatus(byte status);
				
		void changeLedColor(short duration, byte repeat, byte effect, byte R, byte G, byte B, short delay);
		
		
	private:
		InformFunction OnUpdateByMiniFunction = nullptr;
		InformByteFunction HeadFoundFunction = nullptr;
		InformFunction HeadLostFunction = nullptr;
		TwoWire &wire=Wire;
		long last_demand=0;
		long last_demand_cam=0;
		
		MiWire& operator=(const MiWire&) = delete;
};




/*
class Networking{
	public:
		typedef void (*InformFunction)(byte);
		typedef void (*OnMoveFunction)(float,float);
		MiWire &wr;
		
		byte WIFI_STATUS_CONNECTING=0;
		byte WIFI_STATUS_CONNECTED=1;
		
		const byte REQUEST_UPDATE=6;
		const byte REQUEST_MOVE=2;
		const byte REQUEST_CHANGE_ROBOT_STATUS = 1;
		const byte REQUEST_HELLO = 73; // its a broadcast msg
		
		WiFiUDP udp;
		unsigned int udpPort = 2311;
	
		//Networking();
		void begin(char* ssid, char* pass, MiWire &wire);
		void loop_network();
		
		void setOnWifiListener(InformFunction f);
		void setOnMoveListener(OnMoveFunction f);
		void setRobotStatusChangeListener(InformFunction f);
		
	private:
		InformFunction OnWifiStatusChange = nullptr;
		OnMoveFunction OnMoveF = nullptr;
		InformFunction OnRobotStatusChangeDemandF = nullptr;
		
		
		void updateUser();
		float decodeFromBytes(byte data[]);
		int decoder(byte data[]);
		
};*/