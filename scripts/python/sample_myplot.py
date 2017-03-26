#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys, os, time, math

sys.dont_write_bytecode = True
sys.path.append(os.path.dirname(sys.argv[0]))
from plotter_plusplus import *

if __name__ == '__main__':
    s = DebugSocket('127.0.0.1', 4545)
    s.cleanPlots()
    s.plotGraphForFunction('my_func', 
                           lambda x: x**2,
                           -10.0, 10.0, 0.1)
    print "SAMPLE '" + os.path.basename(sys.argv[0]) + "' COMPLETED..."
