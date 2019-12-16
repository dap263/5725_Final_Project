import time
import busio
import board
import adafruit_amg88xx
import matplotlib.pyplot as plt 
import numpy as np 
i2c = busio.I2C(board.SCL, board.SDA)
sensor = adafruit_amg88xx.AMG88XX(i2c)
total=0.0
average=0.0
buff = []
count = 0
diff = 0
x1 = []
s = []
while True:
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
    
    if count == 5: # determine the current atmosphere
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
        plt.show()
    if len(s) == 2:
        diff = s[1]-s[0]
        print (diff)
        if diff > 0:
            print("change to romantic music")
        if diff <= 0:
            print("We need pop music!!")
        s = []
        
    time.sleep(1)
    count +=1
    average = 0.0
    print (buff)
    print (count)
