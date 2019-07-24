import serial

def CONCHA(x):
        for i in range(0, len(x)):
                print(i, ord(x[i]))
                print(i, x[i])

port = serial.Serial("/dev/ttyACM0", baudrate=9600, timeout=0.2)

while True:
        rcv= port.readline()
        CONCHA(rcv)
        print "Tag detected: " + rcv



