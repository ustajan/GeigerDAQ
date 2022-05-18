#!/usr/bin/env python3
# Author:  Areg Danagoulian
# email: regjan@mit.edu
# License: Attribution 4.0 International (CC BY 4.0)
# Description:  simple code for reading the TTL trigger from the Geiger Counter (or anything, really) using the GPIO of a RasPi

import signal
import sys
import argparse
import time
import RPi.GPIO as GPIO

parser = argparse.ArgumentParser()
parser.add_argument('-o', '--output', help='specify the output file. stdout will be used if none is given.')
parser.add_argument('-p', '--data-points',
					help='quit after the specified amount of detections. use 0 or a negative number to run indefinitely.', type=int)
args = parser.parse_args()

outfile = None

if args.output:
	outfile = open(args.output, "w")
else:
	outfile = sys.stdout


BUTTON_GPIO = 16
time_start=time.time()


def signal_handler(sig, frame):
    GPIO.cleanup()
    outfile.close()
    sys.exit(0)
def trigger_callback(channel):
    current_time=(time.time()-time_start)*1e+6 #time since run start in us
    outfile.write(str(current_time)+"\t-1\n")
    outfile.flush()

if __name__ == '__main__':
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(BUTTON_GPIO, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    GPIO.add_event_detect(BUTTON_GPIO, GPIO.RISING, 
            callback=trigger_callback, bouncetime=1)
    
    signal.signal(signal.SIGINT, signal_handler)
    signal.pause()
