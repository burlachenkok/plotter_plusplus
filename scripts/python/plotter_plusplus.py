#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import struct, socket, sys

class DebugSocket:
    '''Debug socket to work with plotter++'''

    def __init__(self, hostName, port):
        'The constructor'
        try:
            self.hostName = hostName
            self.port = port
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.is_connected = False
            self.s.connect((hostName, port))
            self.is_connected = True
        except (socket.error) as e:
            self.s.close()
            print >> sys.stderr, "Can not connect to '%s:%i' via tcp !!!\n" % (hostName, port)
            raise

    def __del__(self):
        'The destructor'
        self.s.close()    

    def isConnected(self):
        'Is connection established'
        return self.is_connected

    def logMsg(self, msg):
        'Use debug socket to send log message'
        if len(msg) == 0:
            return 
        elif msg[-1] == '\n':
            self.rawSend(msg)
        else:
            self.rawSend(msg + '\n')

    def plotGraph(self, plotName, x, y):
        'Use debug socket to plot graph'
        self.rawSend("plot,%s,%f,%f\n" % (plotName, x, y))

    def plotGraphForFunction(self, graphName, func, x_start, x_end, dx):
        'Use debug socket to plot graph'
        while x_start <= x_end:
            self.plotGraph(graphName, x_start, func(x_start))
            x_start = x_start + dx

    def plotCfg(self, **options):
        'Use debug socket to plot graph'

        name = options['name']
        if 'width'     in options.keys(): self.rawSend("plotCfg,%s,width,%i\n" % (name, options['width']) )
        if 'dotLine'   in options.keys(): self.rawSend( "plotCfg,%s,dotLine,%i\n" % (name, options['dotLine']) )
        if 'visible'   in options.keys(): self.rawSend( "plotCfg,%s,visible,%i\n" % (name, options['visible']) )
        if 'color_rgb' in options.keys(): self.rawSend( "plotCfg,%s,color_rgb,%i,%i,%i\n" % (name, options['color_rgb'][0], options['color_rgb'][1], options['color_rgb'][2]) )

    def plotImg(self, rawBuffer, width, height, bytesPerPixel):
        sizeInBytes = width * height * bytesPerPixel 
        '''Use debug socket to send image.
           There are various formats of pixel formats which supported by plotter++
                RGB_U8U8U8,        ([0,255], [0,255], [0,255])
                RGB_U8U8U8U8,      ([0,255], [0,255],[0,255], 255)
                RGB_F32F32F32,     ([0.0f, 1.0f], [0.0f, 1.0f], [0.0f, 1.0f])
                GRAY_U8,           ([0,255])
                GRAY_F32,          ([0.0f, 1.0f])
                GRAY_F32_UNNORM,   ([0.0f, 255.0f])
                GRAY_F64,          ([0.0, 1.0])
                GRAY_F64_UNNORM    ([0.0, 255.0])
'''
        sendMsg = "image,%i,%i,%i\n" % (width, height, sizeInBytes) #width height totalsise
        self.rawSend(sendMsg)
        for x in rawBuffer:
            self.rawSend(struct.pack('B', x))

    def cleanPlots(self):
        'Clean(remove) all graphs with all points'
        self.s.send("clean,plots\n")

    def cleanLog(self):
        'Clean(remove) all log text'
        self.s.send("clean,log\n")

    def rawSend(self, msg):
        'Mostly for internal usage. Use it as proxy message before message will be send to network'
#        Uncomment to debug
#        print ">>>", msg,
        self.s.send(msg)
    