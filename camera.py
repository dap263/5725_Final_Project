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

total=0.0
average=0.0
buff = []
count = 0.0
x1 = []
diff = 0
s = []
while True:
    #show temp
    for row in sensor.pixels:
        # Pad to 1 decimal place
        print(['{0:.1f}'.format(temp) for temp in row])
        a = ['{0:.1f}'.format(temp) for temp in row]
        total = float(a[0]) + float(a[1]) + float(a[2]) + float(a[3]) + float(a[4]) + float(a[5]) + float(a[6]) + float(a[7])
        average = average + total
        print("")
  
    average = average / 64
    average = round(average,3)
    print ('Average: ' + str(average))
    if count >=1:
        buff.append(average)
        x1.append(count)
        #print (buff)
    print("\n")    
    
    if count == 60: # determine the current atmosphere
        l1=plt.plot(x1,buff,'r--')
        plt.title('The temperature trend')
        plt.xlabel('time')
        plt.ylabel('temp')
        slope,intercept = np.polyfit(np.log(x1),np.log(buff),1)
        print (slope)
        buff = []
        x1 = []
        count = 0 
        s.append(slope)
        #plt.show()
    if len(s) == 2:
        diff = s[1]-s[0]
        print (diff)
        if diff > 0:
            print("change to romantic music")
        if diff <= 0:
            print("We need pop music!!")
        s = []

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
    
    count += 0.2
    print (count)
    time.sleep(0.1)
    pygame.display.flip()
