import cv2
import sys
import logging as log
import datetime as dt
from time import sleep

#telegram
import requests
import telegram

bot = telegram.Bot(token='863169587:AAGqpDZjM4XhdI862MMi94PbbftqBMjGpiQ')
chat_id = '77798588'
def telegram_bot_sendtext(bot_message):
    
    bot_token = '863169587:AAGqpDZjM4XhdI862MMi94PbbftqBMjGpiQ'
    bot_chatID = '77798588'
    send_text = 'https://api.telegram.org/bot' + bot_token + '/sendMessage?chat_id=' + bot_chatID + '&parse_mode=Markdown&text=' + bot_message

    response = requests.get(send_text)

    return response.json()


# Creating face cascade. (Check image example)
cascPath = "haarcascade_frontalface_default.xml"
faceCascade = cv2.CascadeClassifier(cascPath)
log.basicConfig(filename='webcam.log',level=log.INFO)

# Set video source to default webcam
video_capture = cv2.VideoCapture(0)
anterior = 0
x=0
while True:
    if not video_capture.isOpened():
        print('Unable to load camera.')
        sleep(5)
        pass

    # Capture video frame-by-frame
    ret, frame = video_capture.read()

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    faces = faceCascade.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30)
    )

    # Draw a rectangle around the faces
    for (x, y, w, h) in faces:

        # Rectangle
        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

        # Send message to telgram user
        test = telegram_bot_sendtext(str(len(faces))+" FACE(S) DETECTED! at "+str(dt.datetime.now()))

        # Create image
        
        cv2.imwrite("face.jpg", frame)
        

        bot.send_photo(chat_id=chat_id, photo=open('face.jpg', 'rb'))
        x=1
    if anterior != len(faces):
        anterior = len(faces)
        log.info("faces: "+str(len(faces))+" at "+str(dt.datetime.now()))


    # Display the resulting frame
    cv2.imshow('Deteccion de Caras', frame)


    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    # Display the resulting frame
    cv2.imshow('Deteccion de Caras', frame)

# When everything is done, release the capture
video_capture.release()
cv2.destroyAllWindows()
