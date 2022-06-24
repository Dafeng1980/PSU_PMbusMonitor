# test_connect.py 
import paho.mqtt.client as mqtt 
import time
# The callback function. It will be triggered when trying to connect to the MQTT broker
# client is the client instance connected this time
# userdata is users' information, usually empty. If it is needed, you can set it through user_data_set function.
# flags save the dictionary of broker response flag.
# rc is the response code.
# Generally, we only need to pay attention to whether the response code is 0.

# broker = 'broker.emqx.io'
broker = '192.168.1.36'
port = 1883
# topic = "/python/mqtt"
# client_id = f'python-mqtt-{random.randint(0, 1000)}'
username = 'dfiot'
password = '123abc'



def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected success")
        client.subscribe("rrh/pmbus/status")	
    else:
        print(f"Connected fail with code {rc}")
def on_message(client, userdata, msg):
    print(f"{msg.topic}        {msg.payload}")
    time.sleep(0.1)
    print(msg.topic+" "+str(msg.payload))

client = mqtt.Client()
 
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(username, password)
# client.connect("192.168.1.36", 1883, 60)
client.connect(broker, port)


client.publish('rrh/pmbus/input/volt', '234.1', qos=0, retain=False)
print("input volt sent")
time.sleep(1) 
client.publish('rrh/pmbus/input/volt', '230.3', qos=0, retain=False)
time.sleep(1)
client.publish('rrh/pmbus/input/volt', '204.8', qos=0, retain=False)
time.sleep(1)
client.publish('rrh/pmbus/input/volt', '236.5', qos=0, retain=False)
time.sleep(1)
client.publish('rrh/pmbus/input/volt', '254.9', qos=0, retain=False)
client.loop_forever()
time.sleep(3)
print('disconnect')
client.disconnect()

