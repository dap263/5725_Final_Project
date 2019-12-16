#!/usr/bin/env python
import os
#import matplotlib.pyplot as plt
import timeit
import numpy as np
import queue
#path = '/home/pi/ECE-5725/Final-Project/adcBuf'
path = '/home/dap263/ECE-5725/Final-Project/adcBuf'
fifo = open(path, "r")
q = queue.Queue(maxsize=30000)
a = [0 for i in range(0,256)]
a = np.asarray(a, dtype=np.int32)
cnt = 0
#for line in fifo:
#	q.put(int(line))
#	a[int(line)] += 1
#	cnt += 1
#	if (cnt > 2000):
#		break	
print("first loop done")
keep_going = True
while keep_going: 
#	a = [0 for i in range(0,1024)]
	#for line in fifo:
	#	if (q.qsize() > 2000):
	for i in range (1000):
		line = fifo.read(5)
		if q.qsize() > 20000: 
			a[q.get() >> 2] -= 1
		a[int(line) >> 2] += 1
		q.put(int(line))

	print ("next")
	#valid = np.nonzero(a)

	#last = np.amax(valid)
	#first = np.amin(valid)
	#mid = ( last + first ) >> 1
	#width = mid - first

	#asort = a
	#asort = np.argsort(a)
	#if ( asort[len(asort)-1] > (mid + (width>>1)) or asort[len(asort)-1] < (mid - (width>>1)) ):
	#	print (q.qsize(), "distorted!")
	#else:
	#	print (q.qsize(), "no distortion detected")

fifo.close()
