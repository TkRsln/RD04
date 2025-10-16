# RD04
4th version of Quadruped Robot
[![Videoyu izleyin](https://github.com/TkRsln/RD04/blob/main/images/20231010_022251.jpg?raw=true)](https://drive.google.com/file/d/1RnupB3pll7MTt-dQoDKnbMLql6xzSinr/view?usp=sharing)


### Software Design
/!\ Currently it can only stand / sit, there is no movement control such as forward or backward yet for this version.
i used 3 different board, these are Esp8266, Raspberry PI Pico, Arduino Pro mini.

Pico:
- it is responsible from servo motor control since it has more powerfull PWM generators, therefore it is strong enogh to handle 12 servo motor easly.

ESP8266:
- since it has wifi modul, it is master board that controls others. Via UDP, you can send any command to it or receive any information throug sensors.

ProMini:
- it is responsible to read sensors or control LEDS, Oled Screen.

Note:
- For pico, micro python is used. others are c++. 
- i entegrated ESP32-Cam with my own android app to control robot with camera from phone, when i find all codes i will post also these too!
 
### Hardware Design
- 2S4P Battery
- 1x Pico
- 1x ProMini
- 1x ESP 8266
- 4x WS2812B (1 LED Chip)
- 1x OledScreen (32x128)
- 3x 2S BMS
- 1x MPU6050
- 1x RCWL0516
- 1x Voltage sensor
- 12x Servo Motor (I used 13~kg/cm, you can prefer bigger ones)
![alt text](https://github.com/TkRsln/RD04/blob/main/images/RoboV2.drawio%20(6).png?raw=true)

