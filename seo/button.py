import RPi.GPIO as gpio

button1_pin = 11
button2_pin = 13

gpio.setmode(gpio.BCM)
gpio.setup(button1_pin, gpio.IN)
gpio.setup(button2_pin, gpio.IN)

def Button():
	if gpio.input(button1_pin):
	#버튼 1을 눌렀을때 실행할 code 
	else gpio.input(button2_pin):
	#버튼 2를 눌렀을때 실행할 code

try:
	Button()

except KeyboardInterrupt:
	gpio.cleanup()
