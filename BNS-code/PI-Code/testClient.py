from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import time
result = "";
start = 0;

def button(btn):
	if btn == 1 and start == 0:
		start_button()
	elif btn == 2 and start == 1:
		end_button()
	elif btn == 3: #test make txt
		get_message("hi")
		

def start_button():
	global start
	print(1)
	myMQTTClient.publish("bns/client/startbtn", str(1), 0)
	start = 1
		
def end_button():
	global start
	print(2)
	myMQTTClient.publish("bns/client/endbtn", str(2), 0)
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

myMQTTClient = AWSIoTMQTTClient("/home/laply/bns/BNS-code/BWS-ConfigureData/bns.public.key")
myMQTTClient.configureEndpoint("a2ydgvmemsrzk9.iot.ap-northeast-1.amazonaws.com", 8883)
myMQTTClient.configureCredentials("/home/laply/bns/BNS-code/BWS-ConfigureData/root-CA.crt", "/home/laply/bns/BNS-code/BWS-ConfigureData/bns.private.key", "/home/laply/bns/BNS-code/BWS-ConfigureData/bns.cert.pem")

myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec

myMQTTClient.connect()
myMQTTClient.subscribe("bns/server/+", 1, customCallback)

try:
	while True:
		btn = input("input 1 after 2 : ")
		button(btn)

except KeyboardInterrupt:
	print(" Finished!")
	if start == 1:
		print(2)
		myMQTTClient.publish("bns/client/endbtn", str(2), 0)
	myMQTTClient.unsubscribe("bns/server/+")
	myMQTTClient.disconnect()
