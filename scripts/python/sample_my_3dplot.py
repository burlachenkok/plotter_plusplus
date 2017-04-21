#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys, os, time, math

sys.dont_write_bytecode = True
sys.path.append(os.path.dirname(sys.argv[0]))
from plotter_plusplus import *

if __name__ == '__main__':
    s = DebugSocket('127.0.0.1', 4545)
    # Clean
    s.cleanPlots()

    # Function domain
    xStart = 0.1
    xEnd = 10.0
    dx = 0.1

    yStart = 0.1
    yEnd = 10.0
    dy = 0.1

    # Test function
    parabaloid_convex = lambda x, y: x**2+y**2
    xy_non_convex = lambda x, y: x*y
    log_sum_exp_convex = lambda x, y: math.log(math.exp(x) + math.exp(y))
    max_convex = lambda x, y: max(x,y)

    i = 0
    x = xStart
    while x < xEnd:
        y = yStart 

        while y < yEnd:
            s.plotGraph('xy', x, y)
            s.plotGraph('xy_non_convex', i, xy_non_convex(x, y))
            s.plotGraph('log_sum_exp_convex', i, log_sum_exp_convex(x, y))
            s.plotGraph('max_convex', i, max_convex(x, y))

            y = y + dy
            i = i + 1

        x = x + dx

    print "SAMPLE '" + os.path.basename(sys.argv[0]) + "' COMPLETED..."
