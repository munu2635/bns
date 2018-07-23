# Import SDK packages
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient

# For certificate based connection
myMQTTClient = AWSIoTMQTTClient("/home/laply/bns/AWS/testFiles/bns.public.key")
# For Websocket connection
# myMQTTClient = AWSIoTMQTTClient("myClientID", useWebsocket=True)
# Configurations
# For TLS mutual authentication
myMQTTClient.configureEndpoint("a2ydgvmemsrzk9.iot.ap-northeast-1.amazonaws.com", 8883)
# For Websocket
# myMQTTClient.configureEndpoint("YOUR.ENDPOINT", 443)
# For TLS mutual authentication with TLS ALPN extension
# myMQTTClient.configureEndpoint("YOUR.ENDPOINT", 443)
myMQTTClient.configureCredentials("/home/laply/bns/AWS/testFiles/root-CA.crt", "/home/laply/bns/AWS/testFiles/bns.private.key", "/home/laply/bns/AWS/testFiles/bns.cert.pem")
# For Websocket, we only need to configure the root CA
# myMQTTClient.configureCredentials("/home/laply/d0061e5064-certificate.pem.crt")
myMQTTClient.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
myMQTTClient.configureDrainingFrequency(2)  # Draining: 2 Hz
myMQTTClient.configureConnectDisconnectTimeout(10)  # 10 sec
myMQTTClient.configureMQTTOperationTimeout(5)  # 5 sec

myMQTTClient.connect()

try:
	for i in range(5):
		print(i)
		myMQTTClient.publish("sdk/test/cpp", "haha: "+ str(i), 0)

except KeyboardInterrupt:
	print("Finished!")
	myMQTTClient.disconnect()
