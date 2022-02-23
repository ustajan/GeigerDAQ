#!/usr/bin/env python
# coding: utf-8

# In[1]:


"""
Code for streaming data from a microphone in realtime.
Microphone voltage is captured using pyaudio and digitized to 16 bits.
This is then used to detect TTL pulses from a Geiger-Muller detector


Dependency

>>> pip install pyaudio

Author:  Areg Danagoulian (based on code acquired online)
License: Attribution 4.0 International (CC BY 4.0)
Copyright: by authors
"""

import pyaudio
import os
import struct
import numpy as np
import matplotlib.pyplot as plt
import time
from tkinter import TclError

# use this backend to display in separate Tk window
#get_ipython().run_line_magic('matplotlib', 'tk')

# constants
CHUNK = 1024 * 1             # samples per frame
CHUNK = 128                  # used to work with 512.  Shortened the PS to 200 us, will reduce this further 
FORMAT = pyaudio.paInt16     # audio format (bytes per sample?)
CHANNELS = 1                 # single channel for microphone
RATE = 44100                 # samples per second


# In[14]:


# create matplotlib figure and axes
fig, ax = plt.subplots(1, figsize=(15, 7))

# pyaudio class instance
p = pyaudio.PyAudio()

# stream object to get data from microphone
stream = p.open(
    format=FORMAT,
    channels=CHANNELS,
    rate=RATE,
    input=True,
    output=True,
    frames_per_buffer=CHUNK
)

# variable for plotting
x = np.arange(0, 2 * CHUNK, 2)

# create a line object with random data
line, = ax.plot(x, np.random.rand(CHUNK), '-', lw=2)

# basic formatting for the axes
ax.set_title('AUDIO WAVEFORM')
ax.set_xlabel('samples')
ax.set_ylabel('volume')
#ax.set_ylim(0, 255)
ax.set_ylim(-2**15, 2**15)
ax.set_xlim(0, 2 * CHUNK)
#plt.setp(ax, xticks=[0, CHUNK, 2 * CHUNK], yticks=[0, 128, 255])

# show the plot
plt.show(block=False)

print('Started readout')

# for measuring frame rate
frame_count = 0
trigger_count=0
trigger_threshold=25000 #the threshold, in the 16 bit signed scale, for detecting the pulses from the GM tube.
# important note:   for the MightyOhm model use 5000
#                   for the MIT model use 25000.  Set the voltage divider for the input to 0.16/20.
start_time = time.time()
file = open('output.txt','a+')
verbose=True

measurement_time=600
fig.canvas.draw()

while ((time.time()-start_time)<measurement_time):
    
    # binary data
    current_time=time.time()
    data = stream.read(CHUNK,exception_on_overflow = False)
    data_np = np.frombuffer(data, dtype = np.int16) 
    max_time=current_time+data_np.argmax()/RATE #time of the peak

    line.set_ydata(data_np)
    if(data_np.max()>trigger_threshold):
        print(max_time-start_time,'\t',data_np.max(),file=file)
        #fig.canvas.draw()
        #fig.canvas.flush_events()
        #plt.show(block=True)

        if(verbose):
            print(max_time-start_time,'\t',data_np.max())
     #   fig.canvas.draw()
     #   fig.canvas.flush_events()
        trigger_count +=1
    #    break
    elif(data_np.max()>5000):
        print('Low signal (ignoring):\t',time.time()-start_time,'\t',data_np.max())
    # update figure canvas
    #for now let's disable the plotting
        
#    fig.canvas.draw()
#    fig.canvas.flush_events()
    frame_count += 1

#        break
file.close()
print("Total counts:\t",trigger_count,"\tCount rate:\t",trigger_count/(measurement_time/60)," cpm")

# In[ ]:




