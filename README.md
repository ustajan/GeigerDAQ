# GeigerDAQ


![Image of a typical setup](pictures/image.jpg)

__Authors__:  Areg Hovhannisyan (areg_hovhannisyan@edu.aua.am ), Areg Danagoulian (aregjan@mit.edu)

__License and Copyright__: see individual files

This is a suite of various codes necessary for reading out the TTL signal from a typical Geiger-Muller detector by an Arduino microprocessor.  
The suite consists of two types of codes:


 * Arduino codes for detecting the TTL pulse, and transfering the timestamp of the pulse to the computer over the serial protocol
 * Python codes for reading in the serial data from the Arduino
 * readout from the microphone signal
 * code for ROOT, for reading in data and analyzing its time distribution

 More specific information:

 * requirements.txt -- Requirements for running the Python scripts
 * requirements\_rpi.txt -- Requirements for running the logger_GPIO.py file (on a Raspberry Pi)
 * logger.py -- reads data from the serial bus, performs analysis, writes to a file
 * logger\_barebone.py -- does the bare minimum of the above
 * logger\_println.py -- the older (less efficient but simpler) version of logger.py. Will be retired in the near future.
 * plotter.py -- simply python script for plotting the results (requires only one column)
 * waiting_time.C -- the C++ code for ROOT, that does an analysis similar to plotter.py
 * logger_GPIO.py -- the python code for reading in the signal using the GPIO of a Raspberry Pi
 * read\_mike\_trigger.py -- a completely different approach, which instead of arduino instead has the TTL sent to the audio-jack of the laptop, with pyaudio reading the voltage on the microphone connector

**Arduino codes**

There are two sets of software for running the Arduino microcontroller.

 * GeigerCounter/GeigerCounter.ino -- this is the older code that uses a 16x2 (large, clunky) LCD, in a configuration that is similar to the photo above.  Deprecated, *will be removed in future versions*.
 * GeigerCounter\_oled/GeigerCounter\_oled.ino -- this is the newer code that operates a 128x64 OLED screen.  A lot of buggs have been fixed and the code has been simplified.  It displays the battery voltage, the HV voltage (in real time -- allowing to tune it via the 50 Ohm pot), as well as the CPM and the dose rate. We *strongly* recommend this version over the older GeigerCounter.ino.

# Testing:

 The code here has been tested with two platforms:
  * Arduino Pro Micro
  * Arduino Nano Every
  

Documentation:

The usage of GeigerCounter.ino and logger\_barebone.py can be found in the Documentation directory.  To run the latex (in Mac OS X) do
> pdflatex -shell-escape Documentation.tex 
