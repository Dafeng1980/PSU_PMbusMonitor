# ESP8266 Boards for monitoring PMbus data

* Base on ESP-01S & HEKR 1.1 Boards (Low Cost!!) <br/>

* Install the [Andunio IDE](https://www.arduino.cc/en/software "https://www.arduino.cc/en/software") <br/>

* Install the [ESP8266 Arduino core & Libraries](https://github.com/esp8266/Arduino "https://github.com/esp8266/Arduino") <br/>

* Add the Pmbus.ino / Smbus.ino / Twii2c.ino inspired by [Linduino libraries](https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html "https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-development-platforms/linduino.html") <br/>

* Modify the "Wire.h", Wire.cpp", add the [Wire.requestFromS](https://github.com/Dafeng1980/PSU_PMbusMonitor/tree/master/doc/Wire "https://github.com/Dafeng1980/PSU_PMbusMonitor/tree/master/doc/Wire") to Support SMbus Wire.requestFrom. 
 > \>  In folder (%USERPROFILE%\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.1\libraries\Wire). <br/>

* Install below the apps on the phone:
> 1. [Serial USB Terminal APP](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en "https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en") (is a line-oriented terminal / console app for microcontrollers, arduinos and other devices with a serial / UART interface connected with a USB to serial converter to your android device).
> 2. [MQTT Dash IoT APP](https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en "https://play.google.com/store/apps/details?id=net.routix.mqttdash&hl=en") (Use the ESP8266 with the MQTT protocol to send PMbus data to the cloud). 
> 3. [MQTT Dashboard APP](https://play.google.com/store/apps/details?id=com.app.vetru.mqttdashboard "https://play.google.com/store/apps/details?id=com.app.vetru.mqttdashboard") Iot <br/>

### ESP-01S Board & HEKR 1.1 Board:  <br/> 
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/esp-01s.jpg)
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/ESP8266hekr.png)
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/LM1117_3V3.jpg)
<br/>

### Use USB to TTL device and TYPE-C to USB-A OTG cable for communication to the cell phone.
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/USB2tll.jpg)
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/TYPE-C2USBotg.png)

### Monitor PMbus data  with Serial USB Terminal APP <br/>
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/connected.jpg) 
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/pmbus_Serial.png)   <br/> 

### Monitor PMbus data with MQTT Dashboard APP: <br/>
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/Mqtt_Dashboard.jpg) <br/>

### Use Raspberry-Pi to build a MQTT Broker Server & Wi-Fi Hotspot :  <br/>
![image](https://github.com/Dafeng1980/PowerPMbusTools/raw/master/doc/Mqtt_server.jpg)  <br/> 

