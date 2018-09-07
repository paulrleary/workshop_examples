#
#Created by paulleary on 9/6/18 
#

import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt


import numpy as np
import serial as serial
from time import time, sleep

plt.ion()

class DynamicUpdate():

    def on_launch(self):
        #Set up plot
        self.figure, self.ax = plt.subplots()
        self.lines, = self.ax.plot([],[], '-')

        self.lines.xlabel = plt.xlabel('Samples')
        self.lines.ylabel = plt.ylabel('Correlation (normalized)')


        self.max_y = 1
        self.min_y = -1

        #Autoscale on unknown axis and known lims on the other
        self.ax.set_autoscaley_on(True)

        #Other stuff
        self.ax.grid()
        # ...

        # Annotation
        self.arrow = matplotlib.text.Annotation('{}'.format(0),xy=(0,0),
                                                xytext = (-10, -10),textcoords='offset points',
                                                arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))
        self.ax.add_artist(self.arrow)


    def updateLines(self, xdata, ydata):
        #Update data (with the new _and_ the old points)
        self.lines.set_xdata(xdata)
        self.lines.set_ydata(ydata)
        self.min_x = min(xdata)
        self.max_x = max(xdata)
        self.ax.set_xlim(self.min_x, self.max_x)

        if max(ydata) >self.max_y :
            self.max_y = max(ydata)
        if min(ydata)<self.min_y:
            self.min_y = min(ydata)
        self.ax.set_ylim(self.min_y, self.max_y)

        # # Update the annotation in the current axis..
        # indx = np.argmax(ydata)
        # self.arrow.xy = (xdata[ indx ], ydata[ indx ]) # points to
        # self.arrow._text = '{:.5}'.format(xdata[indx]) # text field

        #We need to draw *and* flush
        self.figure.canvas.draw()
        self.figure.canvas.flush_events()
        sleep(0.0001)

    def updatePeak(self, xdata, ydata):
        # Update the annotation in the current axis..
        # indx = np.argmax(ydata)
        self.arrow.xy = (xdata, ydata)  # points to
        self.arrow._text = '{:.5}'.format(xdata)  # text field

    #Example
    def __call__(self):
        self.on_launch()
        xdata = []
        ydata = []
        for x in np.arange(0,10,0.5):
            xdata.append(x)
            ydata.append(np.exp(-x**2)+10*np.exp(-(x-7)**2))
            self.on_running(xdata, ydata)
            time.sleep(1)
        return xdata, ydata


d = DynamicUpdate()

#  initialize serial port with and without the last 3 parameters, and also at 1Mbps, same happens.
ser = serial.Serial('/dev/cu.usbmodem3056451', 500000, timeout=0.1, xonxoff=False, rtscts=False, dsrdtr=False)
ser.flushInput()
ser.flushOutput()

zero_centered = 1

data = []

xdata = []
ydata = []
d.on_launch()

sleep(0.001)
# while True:
for i in range(0,100000):

    data = np.fromstring(ser.readline(),dtype = 'f4',sep =',')
    try:
        if data[0]>= 2:
            buffer_size = data[0]
            samplerate = data[1]
            peakindex = data[2]
            peakvalue = data[3]

            print(peakindex-255)

            if zero_centered:
                bins = np.linspace(-(buffer_size / 2 - 1), buffer_size / 2 - 1, buffer_size - 1)
            else:
                bins = np.linspace(0, buffer_size, buffer_size-1)

            mags = data[4:]
            len_mags = len(mags)
            len_bins = len(bins)

            try:
                if len_mags == len_bins:
                    d.updateLines(bins, mags)
                    # d.updatePeak(peakindex, peakvalue)
                    if zero_centered:
                        d.updatePeak(peakindex-255, peakvalue)
                    else:
                        d.updatePeak(peakindex, peakvalue)
            except RuntimeWarning:
                print(mags)
        else:
            print(i, data[0])

    except IndexError:
        print(i)

    # sleep(0.000001)

if __name__ == '__main__':
    pass


if __name__ == '__main__':
    pass