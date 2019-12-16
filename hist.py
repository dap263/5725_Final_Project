#!/usr/bin/env python

import numpy as np
import os
import matplotlib.pyplot as plt
import math

path = '/home/pi/ECE-5725/Final-Project/adcBuf'

fifo = open(path, "r")
size = 128
a = [0 for i in range(0,size)]
b = [i for i in range(0,size)]
for line in fifo:
	a[int(line) >> 3] += 1
	#print len(line)
	#print int(line)
	#print ord(line)

fifo.close()
print (a)
#plt.hist(a, bins='auto', range=(0,1024))
plt.bar(b,a, align='center',alpha=.5)
plt.title("Samples vs. Amplitude")
plt.xlabel("Amplitude")
plt.ylabel("Number of Samples")
plt.show()
