#
#Created by paulleary on 2/15/19 
#
############### Import Libraries ###############
from matplotlib.mlab import window_hanning,specgram
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import LogNorm
import numpy as np
import serial as serial

SAMPLES_PER_FRAME = 10

def update_fig(n):
    # data = get_sample(stream,pa)
    # arr2D,freqs,bins = get_specgram(data,rate)
    im_data = im.get_array()
    if n < SAMPLES_PER_FRAME:
        im_data = np.hstack((im_data,arr2D))
        im.set_array(im_data)
    else:
        keep_block = arr2D.shape[1]*(SAMPLES_PER_FRAME - 1)
        im_data = np.delete(im_data,np.s_[:-keep_block],1)
        im_data = np.hstack((im_data,arr2D))
        im.set_array(im_data)
    return im,


ser = serial.Serial('/dev/cu.usbmodem3056451', 500000, timeout=0.1, xonxoff=False, rtscts=False, dsrdtr=False) #Tried with and without the last 3 parameters, and also at 1Mbps, same happens.

ser.flushInput()
ser.flushOutput()
ready = False
while ~ready:
    data = np.fromstring(ser.readline(), dtype='f4', sep=',')

    fft_size = data[0]
    samplerate = data[1]

    bins = np.linspace(0, samplerate / 2, fft_size / 2)
    bins = bins[1:]

    mags = data[2:-1]
    length = len(mags)

    try:
        # mags = 20*np.log10(mags)
        if len(mags) == len(bins):
            ready = True
    except RuntimeWarning:
        print(mags)

fig = plt.figure()
extent = (bins[0],bins[-1]*SAMPLES_PER_FRAME,freqs[-1],freqs[0])
im = plt.imshow(arr2D,aspect='auto',extent = extent,interpolation="none",
                cmap = 'jet',norm = LogNorm(vmin=.01,vmax=1))
data = []

xdata = []
ydata = []
# d.on_launch()

# sleep(0.001)
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


               # d.update(bins, mags)

    except RuntimeWarning:
        print(mags)

    # sleep(0.001)
if __name__ == '__main__':
    pass