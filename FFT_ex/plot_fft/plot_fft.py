import matplotlib.pyplot as plt
import numpy as np
import serial as serial
from time import time, sleep

plt.ion()

class DynamicUpdate():
    #Suppose we know the x range
    # min_x = 0
    # max_x = 4500 #FIX THIS LINE
    # min_y = 50
    # max_y = 140
    def on_launch(self):
        #Set up plot
        self.figure, self.ax = plt.subplots()
        self.lines, = self.ax.plot([],[], '-')

        self.lines.xlabel = plt.xlabel('frequency (Hz)')
        self.lines.ylabel = plt.ylabel('amplitude (dB)')
        #PRL: Set up text label in plot
        # self.label = self.ax.text(0, 0,'')

        self.max_y = 0
        self.min_y = 200

        #Autoscale on unknown axis and known lims on the other
        self.ax.set_autoscaley_on(True)

        #Other stuff
        self.ax.grid()
        # ...

    def on_running(self, xdata, ydata):


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


        #PRL: Update text, label peak frequency and strength
        # idx = ydata==max(ydata)
        # f = xdata(idx)
        # p = ydata(idx)
        # str = "f = %.2f, p = %d" % (f,p)
        # self.label.set_xdata(f)
        # self.label.set_ydata(p)
        # self.label.set_textdata(str)
        # self.label(f,p,str)

        #Need both of these in order to rescale
        # self.ax.relim()
        # self.ax.autoscale_view()
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

# need to update samplerate and fft_size if changed in teensy
# samplerate = 10000
# fft_size = 256


d = DynamicUpdate()

#  initialize serial port
# '''do serial stuff here'''
ser = serial.Serial('/dev/cu.usbmodem3056451', 500000, timeout=0.1, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.
ser.flushInput()
ser.flushOutput()
# assume that bins and mags are the returned by the fft via serial

out = [];

data = []
tstart = time()

xdata = []
ydata = []
d.on_launch()
# ser.write(b'256;9000;1000;5;\r\n')
sleep(0.001)
# while True:
for i in range(0,100000):
    # print(i)
    data = np.fromstring(ser.readline(),dtype = 'f4',sep =',')
    # channel = data[0]
    fft_size = data[0]
    samplerate = data[1]
    # virtualfreq = data[3]
    # iir_n = data[4]
    # power = data[5]
    bins = np.linspace(0, samplerate / 2, fft_size / 2)

    mags = data[2:]
    length = len(mags)
    # if (length/2) % 2 == 0:

    try:
        mags = 20*np.log10(mags)
        if len(mags) == len(bins):
            # x = time() - tstart
            # mags =0.01* np.sin(x*0.1+0.35)*bins
            # assign the values of  bins and mags to the properties of self.lines
            d.on_running(bins, mags)

            # out.channel[i] = channel
            # out.fft_size[i] = fft_size
            # out.samplerate[i] = samplerate
            # out.virtualfreq[i] = virtualfreq
            # out.iir_n[i] = iir_n
            # out.power[i] = power
            # out.mags[i] = mags
            # out.bins[i] = bins

    except RuntimeWarning:
        print(mags)

            # bins = np.linspace(0,samplerate, length)
            # mags = mags[:length/2]

        # except TypeError as err:
        #     del err

    sleep(0.001)


if __name__ == '__main__':
    pass