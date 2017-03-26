#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys, os, time, math

sys.dont_write_bytecode = True
sys.path.append(os.path.dirname(sys.argv[0]))
from plotter_plusplus import *

if __name__ == '__main__':
    s = DebugSocket('127.0.0.1', 4545)

    if True:
        sendImage = [255,0,0,  0,255,0,  0,0,255,  # red,   green, blue
                     0,255,0,  0,0,255,  255,0,0]  # green, blue,  red
        bpp = 3
        s.plotImg(sendImage, 3, 2, bpp)

    if True:
        sendImage = [255,255,255,  0,255,0,       # white,   green
                     0,255,0,      0,0,255, ]     # green, blue
        bpp = 3
        s.plotImg(sendImage, 2, 2, bpp)

    print "SAMPLE '" + os.path.basename(sys.argv[0]) + "' COMPLETED..."
