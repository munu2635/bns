import RPi.GPIO as gpio
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient

button1_pin = 11
button2_pin = 13
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
	myMQTTClient.publish("bns/client/startbtn", str(1), 0)
	start = 1

def end_button():
	global start
	myMQTTClient.publish("bns/client/endbtn", str(2), 0)
	time.sleep(1)
	print("off streamming")
	start = 0

	start = 0

def get_message():
	f = open("result.txt", 'w')
	f.write(result)
	f.close()
	result = "";

def customCallback(client, userdata, message):
	global start
	if start == 1:
		get_message()
		print("Received a new message: " + message.payload)

myMQTTClient = AWSIoTMQTTClient("/home/laply/bns/AWS/testFiles/bns.public.key")
myMQTTClient.configureEndpoint("a2ydgvmemsrzk9.iot.ap-northeast-1.amazonaws.com", 8883)
myMQTTClient.configureCredentials("/home/laply/bns/AWS/testFiles/root-CA.crt", "/home/laply/bns/AWS/testFiles/bns.private.key", "/home/laply/bns/AWS/testFiles/bns.cert.pem")

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
	myMQTTClient.publish("bns/client/endbtn",  str(2), 0)\	myMQTTClient.unsubscribe("bns/server/+")
	myMQTTClient.disconnect()
	myMQTTClient.unsubscribe("bns/server/+")
	myMQTTClient.disconnect()
