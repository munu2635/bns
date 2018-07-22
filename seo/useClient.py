#클라이언트에서 계속 실행 버튼입력받으면 mqtt로 메세지 전송 및 측정값 mqtt로 받음 -> txt 파일로 저장 
import RPi.GPIO as gpio
import paho.mqtt.client as mqtt

button1_pin = 11
button2_pin = 13
result = "";

gpio.setmode(gpio.BCM)
gpio.setup(button1_pin, gpio.IN)
gpio.setup(button2_pin, gpio.IN)

def use_button():
	if gpio.input(button1_pin) == True:
	        mqttc.publish("bns/client/startbtn", 1)
	elif gpio.input(button2_pin) == True:
def get_message():
	if result is not None:
		f = open("result.txt", 'w')
		f.write(result)
		f.close()
		result = "";

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

def on_publish(client, userdata, mid):
    print("message published ")

def on_message(client, userdata, msg):
	global result 
	if msg.topic == "bns/server/result":
		result = msg.payload

mqttc = mqtt.Client()
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_message = on_message
mqttc.connect("127.0.0.1")
mqttc.loop_start()

try:
	while True:
		use_button()
		get_message()
		
except KeyboardInterrupt:
	print("Finished!")
	mqttc.loop_stop()
	mqttc.disconnect()
