#!/usr/bin/env python

import os
import matplotlib.pyplot as plt
import timeit
import numpy as np

path = '/home/pi/ECE-5725/Final-Project/adcBuf'

#fifo = open(path, "r")

q = queue.Queue(maxsize=3000)

#a = [0 for i in range(0,1024)]
#b = [i for i in range(0,1024)]
#for line in fifo:
#	a[int(line)] += 1
#	q.put(int(line))
	#print len(line)
	#print int(line)
	#print ord(line)

#code_to_test = """
#import numpy as np
dist = [0, 0, 0, 0, 0, 1, 100, 20, 10, 5, 8, 9, 12, 15, 20, 25, 30, 40, 55, 70, 65, 75, 69, 55, 40, 30, 25, 20, 15, 12, 9, 8, 5, 10, 20, 100, 1, 0, 0, 0, 0, 0]
b = [i for i in range(len(dist))]
clean = [0, 0, 0, 0, 0, 1, 5, 7, 9, 15, 18, 25, 33, 40, 50, 62, 70, 82, 90, 100, 90, 82, 70, 62, 50, 40, 33, 25, 18, 15, 9, 7, 5, 1, 0, 0, 0, 0, 0]

npa = np.asarray(clean, dtype=np.int32)

valid = np.nonzero(npa)

last = np.amax(valid)
first = np.amin(valid)
mid = ( last + first ) >> 1
width = mid - first

asort = np.argsort(npa)
if ( asort[len(asort)-1] > (mid + (width>>1)) or asort[len(asort)-1] < (mid - (width>>1)) ):
	print ("distorted!")
else:
	print ("no distortion detected")
#"""

#elapsed_time = timeit.timeit(code_to_test, number=100)/100
#print(elapsed_time)

#fifo.close()
#print (a)
#plt.hist(dist, bins='auto', range=(0,len(dist)))
plt.bar(b,dist, align='center',alpha=.5)
plt.show()
