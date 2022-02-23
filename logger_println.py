#!/usr/bin/env python3
# Authors:  Areg Hovhannisyan, Areg Danagoulian
# email: areg_hovhannisyan@edu.aua.am, aregjan@mit.edu
# License: Attribution 4.0 International (CC BY 4.0)
# This is the older version of logger.py, which is using readline() to read data from aurduino that is being sent by printl()

import serial
import time
import argparse
import sys
import numpy

parser = argparse.ArgumentParser()
parser.add_argument('port', help='specify the serial port (e.g. /dev/ttyUSB0 on linux, COM14 on windows)')
parser.add_argument('-b', '--baudrate', type=int, default=115200,
					help='set the baudrate of the serial port')
parser.add_argument('-o', '--output', help='specify the output file. stdout will be used if none is given.')
parser.add_argument('-p', '--data-points',
					help='quit after the specified amount of detections. use 0 or a negative number to run indefinitely.', type=int)
parser.add_argument('-r','--rate',help='Print the real time rate. Note, this may slow down the acquisition. Input: integration counts.')
args = parser.parse_args()

outfile = None

if args.output:
	outfile = open(args.output, "w")
else:
	outfile = sys.stdout

count = 0

if args.rate:
	integration_count=int(args.rate)
	times=numpy.zeros(integration_count)

beg = time.time()
time_elapsed=0
last_time=0
rollover_correction=0
with serial.Serial(args.port, args.baudrate, timeout=10) as ser:
	while True:
#		s = ser.read(1) # just read
		s = ser.readline().decode() #get the actual time

		try:
			arduino_time=int(s.strip()) #the arduino's own time, in microseconds.  Strip the newlines
		except ValueError as e:
			print("error ",e," value of s: ",s.strip())
#			arduino_time=last_time # just use the last value.  We'll get a bunch of zeros in time_elapsed so we'll know how many of these we got.

		if (last_time-arduino_time)>2**31:   #this is risky, and works ONLY if there are at least 3-5 counts / hour
			print("Clock rollover detected!\n")
			rollover_correction+=2**32 #add 2^32 to the rollover
		last_time=arduino_time
		actual_time=(arduino_time+rollover_correction) #add the rollover, and convert to seconds
		if count==0:
			beg=actual_time
		time_elapsed=actual_time-beg
#		outfile.write(str(arduino_time)+"\t"+str(actual_time)+"\t"+str(time_elapsed))
		outfile.write(str(time_elapsed))
		outfile.flush()

		if args.rate: #print the rate information
			if count<integration_count:
				times[count]=time_elapsed
				outfile.write("\t -1 \n")
			else:
				times=numpy.roll(times,-1)
				times[integration_count-1]=time_elapsed
				outfile.write("\t" +str(int(60*integration_count/(times[-1]-times[0])))+ "\n")
		else:
			outfile.write("\t -1 \n")
		
		count += 1

		if count == args.data_points:
			print("Total counts:\t",count,"\tCount rate:\t",count/(float(time_elapsed)*1e-6/60)," cpm\n")
			quit()
