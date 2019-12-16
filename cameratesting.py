#import required libraries
#import OpenCV library
import cv2
#import matplotlib library
import matplotlib.pyplot as plt
#importing time library for speed comparisons of both classifiers
import time
from picamera import PiCamera


cap=cv2.VideoCapture(0)

try:
	while True:
		ret,img=cap.read()
		im = cv2.resize(img,(960,540))
		cv2.imshow("Preview",im)
		print("1")
		time.sleep(2)
		
		
		
		
		

except KeyboardInterrupt:
	  print("Ctl C pressed - ending program")
	  cap.release()
	  cv2.destroyAllWindows()
