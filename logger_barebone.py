#!/usr/bin/env python3
# Authors:  Areg Hovhannisyan
# email: areg_hovhannisyan@edu.aua.am
# License: Attribution 4.0 International (CC BY 4.0)
import serial
import time
import argparse
import sys

parser = argparse.ArgumentParser()
parser.add_argument('port', help='specify the serial port (e.g. /dev/ttyUSB0 on linux, COM14 on windows)')
parser.add_argument('-b', '--baudrate', type=int, default=115200,
					help='set the baudrate of the serial port')
parser.add_argument('-o', '--output', help='specify the output file. stdout will be used if none is given.')
parser.add_argument('-p', '--data-points',
					help='quit after the specified amount of detections. use 0 or a negative number to run indefinitely.', type=int)
args = parser.parse_args()

outfile = None

if args.output:
	outfile = open(args.output, "w")
else:
	outfile = sys.stdout

count = 0

beg = time.time()
last=0
with serial.Serial(args.port, args.baudrate) as ser:
	while True:
		s = ser.read(4)
		micros = int.from_bytes(s, "little")
		if count==0:
			beg=micros

#		if (micros-last)<1000:
		outfile.write(str(micros) + "\n")
		outfile.flush()
		last=micros
		count += 1
		if count == args.data_points:
			print("Total counts:\t",count,"\t Total time:\t",micros-beg," us\t")
			quit()
