#import required libraries
#import OpenCV library
import cv2
#import matplotlib library
import matplotlib.pyplot as plt
#importing time library for speed comparisons of both classifiers
import time
from picamera import PiCamera

camera = PiCamera()
camera.rotation = 270
camera.start_preview()
time.sleep(2)
camera.capture('/home/pi/Desktop/test1.jpg')
camera.stop_preview()
#load test iamge
test1 = cv2.imread('/home/pi/Desktop/test2.jpg')
#convert the test image to gray image as opencv face detector expects gray images
gray_img = cv2.cvtColor(test1, cv2.COLOR_BGR2GRAY)
#load cascade classifier training file for haarcascade
haar_face_cascade = cv2.CascadeClassifier('/home/pi/ECE-5725/haarcascades/haarcascade_frontalface_alt.xml')
#let's detect multiscale (some images may be closer to camera than others) images
faces = haar_face_cascade.detectMultiScale(gray_img, scaleFactor=1.1, minNeighbors=5);

#print the number of faces found
print('Faces found: ', len(faces))
font = cv2.FONT_HERSHEY_SIMPLEX
#go over list of faces and draw them as rectangles on original colored
for (x,y,w,h) in faces:
    cv2.putText(test1,'face',(x, y),font,1,(0,0,255),2)
    cv2.rectangle(test1, (x, y), (x+w, y+h), (0, 255, 0), 2)

# or display the gray image using OpenCV
cv2.imshow('Test Imag', test1)
cv2.imwrite('result.jpg',test1)
cv2.waitKey(5)
cv2.destroyAllWindows()
