import paho.mqtt.client as mqtt 
import time
broker = '192.168.200.2'
port = 1883
username = 'dfiot'
password = '123abc'
Base_topic = "npi/"
addr = 0x50
command = "[08 {:02x} {:02x} {:02x} 10 00]"
#str1 = "offset0x0000:[ 04 FF 40 11 0E 0B 00 C0 46 01 55 01 8B 30 01 42]"

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected success")
    else:
        print(f"Connected fail with code {rc}")

def pub(topic, msg):
    client.publish(Base_topic + topic, msg, qos=0, retain=False)

def sub(topic):
    client.subscribe(Base_topic + topic, qos=0)

def strmat(msg):
	if(msg.find('Fail') > 0):
		return "EEPROM Read Fail"
	str1 = msg[2:15]
	first = msg.find('[')
	end = msg.find(']')
	if(msg[first+1] == ' '):
		str = msg[(first+2):end]
	else:
		str = msg[(first+1):end]
	list = str1 +" " +str
	return list

def on_message(client, userdata, msg):
	print(strmat(str(msg.payload)))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(username, password)
client.connect(broker, port, 60)
client.loop_start()
sub('pmbus/info/eread')

def sentcom(offset, addr = 0x50):
	msb = offset >> 8
	lsb = offset & 0xff
	pub('pmbus/set', command.format(addr, msb, lsb))
	#print('%s' % command.format(addr, msb, lsb))

pub('pmbus/set', '[AA 02 00]') # Disable Pmbus Polling
time.sleep(1)
for i in range(64):    #read EEPROM 16*64 = 1K, 24C08
	sentcom(i*16)
	time.sleep(0.05)
time.sleep(1)
pub('pmbus/set', '[AA 02 01]') # Enable Pmbus Polling
client.loop_stop()			    
print('disconnect')
client.disconnect()
