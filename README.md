# GeigerDAQ

__Authors__:  Areg Hovhannisyan (areg_hovhannisyan@edu.aua.am ), Areg Danagoulian (aregjan@mit.edu)

__License and Copyright__: see individual files

This is a suite of various codes necessary for reading out the TTL signal from a typical Geiger-Muller detector by an Arduino microprocessor.  
The suite consists of two types of codes:


 * Arduino codes for detecting the TTL pulse, and transfering the timestamp of the pulse to the computer over the serial protocol
 * Python codes for reading in the serial data from the Arduino
 * readout from the microphone signal
 * code for ROOT, for reading in data and analyzing its time distribution

 More specific information:

 * logger.py -- reads data from the serial bus, performs analysis, writes to a file
 * logger_barebone.py -- does the bare minimum of the above
 * logger_println.py -- the older (less efficient) version of logger.py
 * read_mike_trigger.py -- a completely different approach, which instead of arduino instead has the TTL sent to the audio-jack of the laptop, with pyaudio reading the voltage on the microphone connector

 * GeigerCounter/GeigerCounter.ino -- the code for Arduino
 