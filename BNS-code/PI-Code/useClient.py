import RPi.GPIO as gpio
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time

button1_pin = 14
button2_pin = 15
start = 0;

gpio.setmode(gpio.BCM)
gpio.setup(button1_pin, gpio.IN)
gpio.setup(button2_pin, gpio.IN)


def button():
	if gpio.input(button1_pin) == True and start == 0:
		start_button()
	elif gpio.input(button2_pin) == True and start == 1:
		end_button()

def start_button():
	global start
	print("start streamming")
	myMQTTClient.publish("bns/client/startBtn", str(1), 0)
	start = 1

def end_button():
	global start
	myMQTTClient.publish("bns/client/endBtn", str(2), 0)
	time.sleep(1)
	print("off streamming")
	start = 0

def get_message(result):
	f = open("result.txt", 'w')
	f.write(result)
	f.close()

def customCallback(client, userdata, message):
	global start
	if start == 1:
		get_message(message.payload)
		print("Received a new message: " + message.payload)

myMQTTClient = AWSIoTMQTTClient("/home/pi/bns/BNS-code/BWS-ConfigureData/bns.public.key")
myMQTTClient.configureEndpoint("a2ydgvmemsrzk9.iot.ap-northeast-1.amazonaws.com", 8883)
myMQTTClient.configureCredentials("/home/pi/bns/BNS-code/BWS-ConfigureData/root-CA.crt", "/home/pi/bns/BNS-code/BWS-ConfigureData/bns.private.key", "/home/pi/bns/BNS-code/BWS-ConfigureData/bns.cert.pem")

myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec

myMQTTClient.connect()
myMQTTClient.subscribe("bns/server/+", 1, customCallback)

try:
	while True:
		button()
		
except KeyboardInterrupt:
	print("Finished!")
	if start == 1:
		print(2)
		myMQTTClient.publish("bns/client/endbtn", str(2), 0)

	myMQTTClient.unsubscribe("bns/server/+")
	myMQTTClient.disconnect()

