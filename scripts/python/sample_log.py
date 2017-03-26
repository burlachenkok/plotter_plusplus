#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys,os,time

sys.dont_write_bytecode = True
sys.path.append(os.path.dirname(sys.argv[0]))
from plotter_plusplus import *

if __name__ == '__main__':
    s = DebugSocket('127.0.0.1', 4545)
    s.cleanLog()
    s.logMsg("log,Hello! From 2 seconds I will give you one more message\n")
    time.sleep(2)
    s.logMsg("log,Please don't use new line in text message becaus it's a message terminator")
    time.sleep(2)
    s.logMsg("log,Buy!")
    print "SAMPLE '" + os.path.basename(sys.argv[0]) + "' COMPLETED..."
