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

        self.lines.xlabel = plt.xlabel('frequency (Hz)')
        self.lines.ylabel = plt.ylabel('amplitude (raw)')


        self.max_y = 0
        self.min_y = 200

        #Autoscale on unknown axis and known lims on the other
        self.ax.set_autoscaley_on(True)

        #Other stuff
        self.ax.grid()
        # ...

    def update(self, xdata, ydata):

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
        self.ax.set_ylim(self.min_y-5, self.max_y+5)

        #We need to draw *and* flush
        self.figure.canvas.draw()
        self.figure.canvas.flush_events()

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

#  initialize serial port


ser = serial.Serial('/dev/cu.usbmodem3056450', 500000, timeout=0.1, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.

ser.flushInput()
ser.flushOutput()


data = []

xdata = []
ydata = []
d.on_launch()

sleep(0.001)
# while True:
for i in range(0,100000):

    data = np.fromstring(ser.readline(),dtype = 'f4',sep =',')

    fft_size = data[0]
    samplerate = data[1]

    bins = np.linspace(0, samplerate / 2, fft_size / 2)

    bins = bins[1:]

    mags = data[2:-1]
    length = len(mags)

    try:
        # mags = 20*np.log10(mags)
        if len(mags) == len(bins):
               d.update(bins, mags)

    except RuntimeWarning:
        print(mags)

    sleep(0.001)


if __name__ == '__main__':
    pass