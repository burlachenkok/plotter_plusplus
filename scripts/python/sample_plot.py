#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys, os, time, math

sys.dont_write_bytecode = True
sys.path.append(os.path.dirname(sys.argv[0]))
from plotter_plusplus import *

if __name__ == '__main__':
    s = DebugSocket('127.0.0.1', 4545)
#   s.cleanPlots()

    freqA = 1
    freqB = 0.5
    t = 0.0

    for i in range(0,100):
        time.sleep(0.01)            # delay
        t = t + 0.1
        print "Current model time: ", t
        s.plotGraph("cos_freq_%g" % freqA, t, math.cos(2*math.pi*freqA*t))
        s.plotGraph("cos_freq_%g" % freqB, t, math.cos(2*math.pi*freqB*t))

    s.plotCfg(name = "cos_freq_%g" % freqA, width = 3, color_rgb = (0,255,0), dotLine = 1, visible = True);

    print "SAMPLE '" + os.path.basename(sys.argv[0]) + "' COMPLETED..."

