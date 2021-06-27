# ESP-01S Board for monitoring PMbus data

* Base on ESP-01S Board (Low Cost Board!!) <br/>

* Install the [Andunio IDE](https://www.arduino.cc/en/software "https://www.arduino.cc/en/software") <br/>

* Add the [ESP8266 Arduino core & Libraries](https://github.com/esp8266/Arduino "https://github.com/esp8266/Arduino") <br/>

* Install two apps on the phone:
> 1. [Serial USB Terminal APP](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en "https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en") (is a line-oriented terminal / console app for microcontrollers, arduinos and other devices with a serial / UART interface connected with a USB to serial converter to your android device).
> 2. [MQTT Dash IoT APP](https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en "https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en") (Use the ESP8266 with the MQTT protocol to send PMbus data to the cloud). <br/>

* The Pmbus.ino / Smbus.ino / Twii2c.ino inspired by [Linduino libraries](https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html "https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html") <br/>

* Modify "Wire.h", Wire.cpp", added the [Wire.requestFromS](https://github.com/Dafeng1980/PSU_PMbusMonitor/tree/master/doc/Wire "https://github.com/Dafeng1980/PSU_PMbusMonitor/tree/master/doc/Wire") to Support SMbus Wire.requestFrom. 
 >   \*In folder (%USERPROFILE%\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.1\libraries\Wire). <br/>

### ESP-01S:  <br/>
 
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/esp-01s.jpg)   <br/> <br/>

### Monitor PMbus data with Serial USB Terminal APP [https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en)   <br/>

![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/connected.jpg) 

![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/pmbus_Serial.png)   <br/> <br/>

### Monitor PMbus data with MQTT Dash APP: [https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en](https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en) <br/>

![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/Mqtt_Phone.jpg)  

![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/pmbus_Mqtt.png)  <br/> <br/>





