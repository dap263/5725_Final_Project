#import required libraries
#import OpenCV library
import cv2
#import matplotlib library
import matplotlib.pyplot as plt
#importing time library for speed comparisons of both classifiers
import time
from picamera import PiCamera

haar_face_cascade = cv2.CascadeClassifier('/home/pi/ECE-5725/haarcascades/haarcascade_frontalface_alt2.xml')
haar_body_cascade = cv2.CascadeClassifier('/home/pi/ECE-5725/haarcascades/haarcascade_upperbody.xml')
cap=cv2.VideoCapture(0)
font = cv2.FONT_HERSHEY_SIMPLEX

def detect_faces(f_cascade, colored_img, scaleFactor = 1.2):
    global index
    #just making a copy of image passed, so that passed image is not changed
    img_copy = colored_img.copy()
    #convert the test image to gray image as opencv face detector expects gray images
    gray = cv2.cvtColor(img_copy, cv2.COLOR_BGR2GRAY)       
    #let's detect multiscale (some images may be closer to camera than others) images
    faces = f_cascade.detectMultiScale(gray, scaleFactor=scaleFactor, minNeighbors=2)  
    print('Faces found: ', len(faces)) 
    index = len(faces)
    #go over list of faces and draw them as rectangles on original colored img
    for (x, y, w, h) in faces:
        cv2.putText(img_copy,'face',(x, y),font,1,(0,0,255),2)
        cv2.rectangle(img_copy, (x, y), (x+w, y+h), (0, 255, 0), 2)
    #show image
    #cv2.imshow("Preview",img_copy)
    #cv2.waitKey(1)
    return img_copy
    
def detect_body(f_cascade, colored_img, scaleFactor = 1.2):
    global index2
    #just making a copy of image passed, so that passed image is not changed
    img_copy = colored_img.copy()
    #convert the test image to gray image as opencv face detector expects gray images
    gray = cv2.cvtColor(img_copy, cv2.COLOR_BGR2GRAY)       
    #let's detect multiscale (some images may be closer to camera than others) images
    body = f_cascade.detectMultiScale(gray, scaleFactor=scaleFactor, minNeighbors=5)  
    print('Body found: ', len(body)) 
    index2 = len(body)
    #go over list of faces and draw them as rectangles on original colored img
    for (x, y, w, h) in body:
        cv2.putText(img_copy,'body',(x, y),font,1,(0,0,255),2)
        cv2.rectangle(img_copy, (x, y), (x+w, y+h), (0, 255, 0), 2)
    return img_copy
s = 0
count = 0
body = 0
try:
    while True:
        #camera.capture('/home/pi/Desktop/test1.jpg')
        #import capture image
        ret,img=cap.read()
        #load image
        #test2 = cv2.imread('/home/pi/Desktop/test2.jpg')
        #faces_detected_img = detect_faces(haar_face_cascade, test2)
        #call our function to detect faces
        faces_detected_img = detect_faces(haar_face_cascade, img)
        
        
        if index > 0 and s == 0:
            body = 1
            s = 1
            print("Play music!!")

        if body == 1:
            body_detected_img = detect_body(haar_body_cascade, img)
            if index2 == 0:
                count +=1
                if count == 100:
                    print("Stop music, Nobody here")
                    body = 0
                    s = 0
            if index2 > 0:
                count = 0
            cv2.imshow("Preview2",body_detected_img)
            
            
            
        cv2.imshow("Preview1",faces_detected_img)
        cv2.waitKey(1)
except KeyboardInterrupt:
	  print("Ctl C pressed - ending program")
	  cv2.destroyAllWindows()
