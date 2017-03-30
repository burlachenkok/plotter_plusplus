# Plotter++

This is an advanced plotter tool which receives commands over the network TCP connection. 
Goal of this program is to assist debugging and development process.
It have been written in C++ and it use Qt Framework 5.7.* as only one external library.

It allow to do the following things:

[1] Plot several curves consist of points (x,y) in one plot

[2] Display received bitmap of 2d images and store history of the images

[3] Take function ranges and function domains from [2] and view it as 3d point in 3d plot

[4] Text console

# Features

1. It support text-based ASCII protocol and support the following commands

    1.1 Accept new points for 2d curves via accepting commands "plot,graphname,x,y\n" over TCP

    1.2 Accept log commands "log,text message...\n" over TCP

    1.3 Commands to display images "image,width,heigt,size\n" and then "size bytes" bytes over TCP

    1.4 Clean commands: "clean,plots\n"  "clean,log\n" over TCP

    1.5 Change graph view "plotCfg,<NAME>, width|dotLine|visible|color_rgb|, <VALUES>\n" over TCP

2. Save, Load all this info for 1-3 into text file with JSON format
3. Append/change/delete manualy in GUI point by point for 2d curves (x(t), y(t)) as t increases.
4. Create a pointcloud view from available plots

# How to build and use
1. You should have QT 5.7.* or higher on you development machine with some C++11 toolchain
2. Open "plotter_plusplus/plotter_plusplus.pro" in QtCreator and build it
3. Lauch tool and start **listen** port in configure panel
4. In "plotter_plusplus/scripts/python" there are some test scripts which connects to the tool over TCP
5. If you have python project use the following python module "plotter_plusplus/scripts/python/plotter_plusplus.py" to have ability to transfer commands to the tool

# Hints
1. Focus in plot in 'plot tab' and use A,W,S,D,+,- and wheel to manipulate a plot.  
2. Use mouse to draw rectangle to zoom and hover over the spline to receive values.

# Author
Konstantin Burlachenko (burlachenkok@gmail.com)

# Various links
1. YouTube demo: https://youtu.be/xD080qQVaD8
2. Link to this document: https://github.com/burlachenkok/plotter_plusplus/blob/master/README.md
3. Download link for Python 2.7: https://www.python.org/download/releases/2.7/
4. Download link for Qt Creator: https://www.qt.io/ru/download/
