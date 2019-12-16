"""This example is for Raspberry Pi (Linux) only!
   It will not work on microcontrollers running CircuitPython!"""
 
import os
import math
import time
 
import busio
import board
import matplotlib.pyplot as plt 
import numpy as np
import pygame
from scipy.interpolate import griddata
 
from colour import Color
import cv2
import adafruit_amg88xx
 
i2c_bus = busio.I2C(board.SCL, board.SDA)
 
#low range of the sensor (this will be blue on the screen)
MINTEMP = 20.
 
#high range of the sensor (this will be red on the screen)
MAXTEMP = 33.
 
#how many color values we can have
COLORDEPTH = 1024
 
os.putenv('SDL_FBDEV', '/dev/fb1')
pygame.init()
 
#initialize the sensor
sensor = adafruit_amg88xx.AMG88XX(i2c_bus)
 
# pylint: disable=invalid-slice-index
points = [(math.floor(ix / 8), (ix % 8)) for ix in range(0, 64)]
grid_x, grid_y = np.mgrid[0:7:32j, 0:7:32j]
# pylint: enable=invalid-slice-index
 
#sensor is an 8x8 grid so lets do a square
height = 360
width = 360
 
#the list of colors we can choose from
blue = Color("indigo")
colors = list(blue.range_to(Color("red"), COLORDEPTH))
 
#create the array of colors
colors = [(int(c.red * 255), int(c.green * 255), int(c.blue * 255)) for c in colors]
 
displayPixelWidth = width / 30
displayPixelHeight = height / 30
 
lcd = pygame.display.set_mode((width, height))
 
lcd.fill((255, 0, 0))
 
pygame.display.update()
pygame.mouse.set_visible(False)
 
lcd.fill((0, 0, 0))
pygame.display.update()
 
#some utility functions
def constrain(val, min_val, max_val):
    return min(max_val, max(min_val, val))
 
def map_value(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
 
#let the sensor initialize
time.sleep(.5)
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
    if index == 1:
        cv2.imwrite('result3.jpg',img_copy)
    #show image
    #cv2.imshow("Preview",img_copy)
    #cv2.waitKey(1)
    return img_copy
    
def detect_body(f_cascade, colored_img, scaleFactor = 1.1):
    global index2
    #just making a copy of image passed, so that passed image is not changed
    img_copy = colored_img.copy()
    #convert the test image to gray image as opencv face detector expects gray images
    gray = cv2.cvtColor(img_copy, cv2.COLOR_BGR2GRAY)       
    #let's detect multiscale (some images may be closer to camera than others) images
    body = f_cascade.detectMultiScale(gray, scaleFactor=scaleFactor, minNeighbors=2)  
    print('Body found: ', len(body)) 
    index2 = len(body)
    if index2 == 1:
        cv2.imwrite('result.jpg',img_copy)
    #go over list of faces and draw them as rectangles on original colored img
    for (x, y, w, h) in body:
        cv2.putText(img_copy,'body',(x, y),font,1,(0,0,255),2)
        cv2.rectangle(img_copy, (x, y), (x+w, y+h), (0, 255, 0), 2)
    if index2 == 1:
        cv2.imwrite('result2.jpg',img_copy)
    return img_copy
    
q = 0
number = 0
body = 0

total=0.0
average=0.0
buff = []
count = 0.0
x1 = []
diff = 0
s = []
try:
    
    while True:
        ret,img=cap.read()
        img = cv2.rotate(img,cv2.ROTATE_90_CLOCKWISE)
        #img = cv2.rotate(img,cv2.ROTATE_90_COUNTERCLOCKWISE)
        faces_detected_img = detect_faces(haar_face_cascade, img)
        if index > 0 and q == 0:
            body = 1
            q = 1
            print("Play music!!")
            
        
        if body == 1:
            body_detected_img = detect_body(haar_body_cascade, img)
            if index2 == 0:
                number +=1
                if number == 20:
                    print("Stop music, Nobody here")
                    body = 0
                    q = 0
            if index2 >0 or index >0:
                number = 0
            #show temp
            for row in sensor.pixels:
                # Pad to 1 decimal place
                #print(['{0:.1f}'.format(temp) for temp in row])
                a = ['{0:.1f}'.format(temp) for temp in row]
                total = float(a[0]) + float(a[1]) + float(a[2]) + float(a[3]) + float(a[4]) + float(a[5]) + float(a[6]) + float(a[7])
                average = average + total
                #print("")
      
            average = average / 64
            average = round(average,3)
            print ('Average: ' + str(average))
            if count >=1:
                buff.append(average)
                x1.append(count)
                #print (buff)
            print("\n")    
            print(count)
            if count == 15: # determine the current atmosphere
                l1=plt.plot(x1,buff,'r--')
                plt.title('The temperature trend')
                plt.xlabel('time')
                plt.ylabel('temp')
                slope,intercept = np.polyfit(np.log(x1),np.log(buff),1)
                #print (slope)
                buff = []
                x1 = []
                count = 0 
                s.append(slope)
                #plt.show()
            if len(s) == 2:
                diff = s[1]-s[0]
                #print(s)
                if diff > 0:
                    print("change to romantic music")
                if diff <= 0:
                    print("We need pop music!!")
                s[0] = s[1]
                del s[1]

            average = 0.0
            #print (buff)

            #read the pixels
            pixels = []
            for row in sensor.pixels:
                pixels = pixels + row
            pixels = [map_value(p, MINTEMP, MAXTEMP, 0, COLORDEPTH - 1) for p in pixels]
     
            #perform interpolation
            bicubic = griddata(points, pixels, (grid_x, grid_y), method='cubic')
     
            #draw everything
            for ix, row in enumerate(bicubic):
                for jx, pixel in enumerate(row):
                    pygame.draw.rect(lcd, colors[constrain(int(pixel), 0, COLORDEPTH- 1)],
                                    (displayPixelHeight * ix, displayPixelWidth * jx,
                                    displayPixelHeight, displayPixelWidth))
        
            count += 1
            cv2.imshow("Preview2",body_detected_img)
            pygame.display.flip()
            
        cv2.imshow("Preview1",faces_detected_img)
        cv2.waitKey(1)
except KeyboardInterrupt:
    print("Ctl C pressed - ending program")
    cv2.destroyAllWindows()
