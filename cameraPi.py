from picamera import PiCamera
from time import sleep

camera = PiCamera()
camera.rotation = 270
camera.start_preview()
sleep(5)
camera.capture('/home/pi/Desktop/test1.jpg')
camera.stop_preview()
