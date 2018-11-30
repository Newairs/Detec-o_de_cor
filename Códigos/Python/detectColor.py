import cv2 as cv
import argparse
import collections
from datetime import datetime
import hashlib
import os.path
import random
import re
import sys

import numpy as np
import time

FLAGS = None

dilation_size = 10
Width = 640
Height = 480

def main():

	r = np.array([0,0,255])
	g = np.array([0,255,0])
	b = np.array([255,0,0])
	black = np.array([0,0,0])

	opening_kernel = cv.getStructuringElement(cv.MORPH_RECT,(7,7),(3,3))
	close_kernel = cv.getStructuringElement(cv.MORPH_RECT,(6,6),(2,2))

	cap = cv.VideoCapture(0)

	cap.set(cv.CAP_PROP_FRAME_WIDTH, Width)
	cap.set(cv.CAP_PROP_FRAME_HEIGHT, Height)

	for experiment in range(0,9):
	
		init = time.time()
		for frames in range(1,100):
		
			ret,frame = cap.read()

			if experiment == 1 :
				cv.imwrite("Feed.jpg",frame)

			m_hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
			mask_g = cv.inRange(m_hsv,np.array([40,40,0]),np.array([80,255,255]))

			mask_g = cv.morphologyEx(mask_g, cv.MORPH_CLOSE, close_kernel)
			mask_g = cv.morphologyEx(mask_g, cv.MORPH_OPEN, opening_kernel)

			#mask_g = cv.cvtColor(mask_g, cv.COLOR_GRAY2BGR, 3)
	
			frame = cv.bitwise_and(frame,frame,mask = mask_g)
					
			
			#for j in range(Height):
				#for k in range(Width):
					#if (mask_g[j,k] == 255).any():
					#	frame[j,k] = g
					#else:
					#	frame[j,k] = black


			frame = cv.cvtColor(frame,cv.COLOR_BGR2GRAY)
			blur = cv.GaussianBlur(frame, (7,7), 1.5, 1.5)

			if "firstFrame" not in globals():
				firstFrame = blur.copy()

			final = cv.Canny(frame,0,30,3)
			im, contours, hierarchy = cv.findContours(final,cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

			leftmost = np.array([0,0])
			rightmost = np.array([0,0])

			count = 0
			for j in contours:
				if cv.contourArea(j,True)>6:
					leftmost = leftmost + j[0]
					rightmost = rightmost + j[1]
					count = count + 1

			leftmost = np.array([0,0])
			rightmost = np.array([0,0])

		end = time.time()
		dif = end - init

		print("Experimento%s: %s" %(experiment, dif))

	cv.imwrite("First.jpg",firstFrame)
	cap.release()	

main()
