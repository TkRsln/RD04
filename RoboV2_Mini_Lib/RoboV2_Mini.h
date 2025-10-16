
#include <Adafruit_SSD1306.h>
#include <FastLED.h>

#include <Wire.h>

class Button {
	public:

		typedef void (*ButtonFunctionPointer)();
		int pin;
		int button_long_duration=750;
		
		void setButtonShortListener(ButtonFunctionPointer f);
		void setButtonLongListener(ButtonFunctionPointer f);
		Button();
		Button(int _pin);
		
		void loop_button();
		
	private:


		ButtonFunctionPointer button_short_function = nullptr;
		ButtonFunctionPointer button_long_function = nullptr;
		
		long button_down_time = 0;
		bool button_previous_status=false;
		bool button_current_status = false;
		
		
};

class SDistance{
	public:
		
		typedef void (*DistanceFunctionPointer)(int);
		
		int pin;
		float value=20;
		int distance_measure_delay=100;
		bool is_close=false;
		float close_range=10;
		
		SDistance();
		SDistance(int _pin);
		SDistance(int _pin,int delay);
		
		void setCloseListener(DistanceFunctionPointer func);
		void setFarListener(DistanceFunctionPointer func);
		void loop_distance();
	
	private:
		DistanceFunctionPointer FunctionClose = nullptr;
		DistanceFunctionPointer FunctionFar = nullptr;
		
		long distance_last_measure=0;
		
		
		float voltages_arr[8]={3.50f,3.30f,2.52f,2.19f,1.90f,1.50f,1.20f,1.06f};
		float distance_arr[8]={5,7,10,12,15,20,25,30};
		float distance_voltage=0;
		float distance_change_speed=0.5f;
		float distance=10;
		
		
};


class SVoltage{
	public:
		typedef void (*VoltageFunction)(float);
		typedef void (*ChargeFunction)();
		
		int pin;
		int pin_charge;
	
		SVoltage();
		SVoltage(int _pin);
		SVoltage(int _pin,int _delay);
		SVoltage(int _pin,int pcharge,int _delay);
		
		float value =0;
		float warn_mod_value=0.25;
		float low_voltage=7.15;
		bool is_charging=false;
		
		
		void loop_voltage();
		
		int delay=3000;
		
		void setLowListener(VoltageFunction f); 				//When the battery is critical low
		void setNormalListener(VoltageFunction f); 				//When the battery is no more low
		void setVoltageDecreasingListener(VoltageFunction f);	//When battery is decreasing 
		
		void setChargePluggedListener(ChargeFunction f);		//On charging
		void setChargeUnpluggedListener(ChargeFunction f);		//On charge unplugged
		void setChargeCompleteListener(ChargeFunction f);		//On Charge complete
		
		
	private:
	
		VoltageFunction NormalFunctionF = nullptr;
		VoltageFunction LowFunctionF = nullptr;
		VoltageFunction VoltageDecreaseFunctionF = nullptr;
		ChargeFunction ChargePluggedFunctionF = nullptr;
		ChargeFunction ChargeUnpluggedF = nullptr;
		ChargeFunction ChargeCompleteF = nullptr;
		
		
		long voltage_last_measure=0;
		bool charge_complete=false;
		bool send_warn=false;
		bool send_low_warn = false;
		
		
	
		
};

class SMotion{
	public:
		typedef void (*MotionFunction)(bool);
		
		int pin;
		int sensor_pin=2;
		int delay=500;
		
		SMotion();
		SMotion(int _pin,int sensor_pin);
		
		bool is_enable=false;
		
		void enable();
		void disable();
		
		void loop_motion();
		void setMotionListener(MotionFunction f);
		
	private:
		
		MotionFunction MotionF = nullptr;
	
		bool ignore_first=true;
		bool previous_motion_status=false;
		long last_motion_time=0;
		
	
};


class Screen{
	public:
		typedef float (*RequestFunction)();
		typedef void (*OnScreenMenuSelectionFunction)(byte,byte);
		
		Adafruit_SSD1306 display;
		Screen(TwoWire &wireObject);
		Screen();
		
		
		byte CURRENT_SCREEN_MODE = 0;
		byte MODE_SLEEP = 0;
		byte MODE_SPLASH = 1;
		byte MODE_MENU = 2;
		byte MODE_BATTERY = 3;
		byte MODE_BATTERY_WARN = 4;
		byte MODE_CONNECTING = 5;

		short DURATION_BATTERY_WARN=10000;
		short DURATION_BATTERY=2500;
		short DURATION_SPLASH = 3000;
		short DURATION_CONNECTION = 5500;
		short DURATION_MENU = 15000;

		void begin();
		void loop_screen();
		void clear();
		void show_splash();
		void show_battery_warn(float _level);
		void show_battery(float _level);
		void show_connection();
		void show_main_menu();
		void onNextButton();
		void onAcceptButton();
		
		void setRequestFunction(RequestFunction _requestf);
		void setMenuSelectionListener(OnScreenMenuSelectionFunction selectionf);
		
		
	private:
			
		int duration = -1;
		int screen_mod=-1;
		long last_change=0;
		float level=8;

		RequestFunction requestf = nullptr;
		OnScreenMenuSelectionFunction selectionf=nullptr;
		
		String main_menu[2] = {"Motion","Battery"};
		String motion_menu[4] = {"Stand","Sit","Lay","Back"};
		byte menu_id=0;
		byte menu_selection=1;
	
		void show_only_battery();
		void connection_loop();
		void draw_menu();
		void calculate_time_out();
	
};
class EsWire{
	public:

		typedef void (*InformFunction)(byte[]);
		
		
		static bool b1 ;
		static bool b2 ;
		static bool b3 ;
		static bool b4 ;
		static bool b5 ;
		static float v;
		static float d;
		static byte screen_decision;
		
		static byte INFO_REFRESH_VALUE ;
		static byte INFO_ON_CONNECTION ;
		static byte INFO_ON_HEAD_FOUND ;
		static byte INFO_CHANGE_RGB;
		
	
		EsWire();
		void begin(TwoWire &wireObject);
		
		static void onRc(int hm);
		static void onRq();
		
		void setEspListener(InformFunction f);
		
		
	private:
		TwoWire &wire;
		static InformFunction EspInformFunction;
		 
		
};


class LEDControl{
	public:
		
		
		const byte EFFECT_SPIRAL = 4;
		const byte EFFECT_MOVING = 3;
		const byte EFFECT_FADE = 2;
		const byte EFFECT_CONSTANT = 1;
		const byte EFFECT_OFF = 0;
		
		byte current_status=0;
			
		float R = 0;
		float G = 0;
		float B = 0;
		
		CRGB leds[10];
		
		LEDControl();
		
		void loop_led();
		
		void setEffect(short _duration,byte repeat,byte effect,byte _R,byte _G,byte _B);
		void setEffect(short _duration,byte repeat,byte effect,byte _R,byte _G,byte _B,short delay);
		void setOff();
		
		
	private:
		long last_status_change=0;
		long last_update=0;
		bool turn_effect_off=false;
		short duration=10000;
		short current_effect=0;
		short effect_repeat=0;
		short delay = 0;
	
};



/*
class EsWire{
	public:
		//typedef bool (*ReqBoolFunction
		//typedef float (*ReqFloatFunction)();
		typedef char* (*ReqFunction)();
		ReqFunction requestDatas = nullptr;
		TwoWire &wire;
		EsWire();
		void setWire(TwoWire &_wire);
		void setRequestListener(ReqFunction f);
		void onRequest();
		void onReceive(int hm);
		
		
};*/

























