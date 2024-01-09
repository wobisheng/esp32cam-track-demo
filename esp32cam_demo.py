from asyncio.windows_events import NULL
from ctypes.wintypes import POINT
from turtle import color
import requests
import cv2
import numpy as np
import time
while True:
    response = requests.get('http://192.168.4.1/')
    image = np.asarray(bytearray(response.content), dtype=np.uint8)
    image = cv2.imdecode(image,-1)
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    low_hsv = np.array([105,100,110])
    high_hsv = np.array([124,255,255])
    mask = cv2.inRange(hsv,lowerb=low_hsv,upperb=high_hsv)
    binary,end = cv2.threshold(mask, 100, 255, cv2.THRESH_BINARY)
    erode = cv2.erode(end,np.ones((2,3),np.uint8),iterations = 1)
    dilate = cv2.dilate(erode,np.ones((3,3),np.uint8),iterations = 6)
    img_temp = cv2.copyMakeBorder(dilate,0,0,30,30,cv2.BORDER_CONSTANT,value = 0)
    blue_out = cv2.Canny(dilate,10,255)
    blue_cnts,hierarchy = cv2.findContours(blue_out.copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)
    blue_cnts = sorted(blue_cnts,key = cv2.contourArea,reverse = True)[:1]
    if len(blue_cnts) == 0:
        cv2.imshow("img",image)
        cv2.waitKey(100)
    else:
        x,y,w,h = cv2.boundingRect(blue_cnts[0])
        output = cv2.rectangle(image,(x,y),(x+w,y+h),(0,255,0),1,8)
        x_t = int((x+x+w)/2)
        print(str(x_t))
        if x <=55:
            angle = int(0.2*(60-x))
            response = requests.get('http://192.168.4.1/posi?position=r&angle='+str(angle))
        if x >=65:
            angle = int(0.2*(x-60))
            response = requests.get('http://192.168.4.1/posi?position=l&angle='+str(angle))
        cv2.imshow("img",output)
        cv2.waitKey(100)