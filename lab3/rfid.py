import serial
import time

port = serial.Serial("/dev/ttyACM0", baudrate=9600, timeout=0.2)


time.sleep(1)
port.write('h')

while True:
	rcv= port.readline()
	print( "Tag detected: " , rcv)
	for c in rcv
		print(c)
	

