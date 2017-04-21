#!/usr/bin/env python
# Build distribution

import subprocess, os, sys, time, shutil

def safeCall(*cmdline):
    print "RUN: ", cmdline
    ret = subprocess.call(cmdline, shell=True)
    print "RETURN CODE: ", ret
    if ret !=0: sys.exit(ret)

def copyFile(src_path, dst_path):
    print "COPY ", src_path, "=>", dst_path
    shutil.copy(src_path, dst_path)

def copyDir(src_path, dst_path):
    print "COPY DIR WITH PRELIMINARY CLEAN", src_path, "=>", dst_path
    if os.path.isdir(dst_path): shutil.rmtree(dst_path)
    shutil.copytree(src_path, dst_path)

#-----------------------------------------------------------------------
if __name__ == "__main__":
    t0 = time.time()
    print "Python version: " +  sys.version
    if os.name == 'nt' or sys.platform == 'win32':
        PATH_TO_QT = os.path.abspath('''C:/Qt/5.7/msvc2013_64/bin''')
        os.environ["PATH"] += os.pathsep + PATH_TO_QT + os.pathsep
#        PATH_TO_QT = os.path.abspath('''C:/Programming/Qt_5_8/5.8/msvc2013_64/bin''')
#        os.environ["PATH"] += os.pathsep + PATH_TO_QT + os.pathsep

        print "Generate project files for windows"
        safeCall(os.path.join(PATH_TO_QT, 'qmake'), '-t', 'vcapp', '-r', './../plotter_plusplus.pro')
        print "Build project with Visual Studio 2013"
        safeCall('vsbuilder_release.bat', 'vc12', 'plotter_plusplus.vcxproj')

        print "Copy extra files to release folder"
        copyFile(os.path.join(PATH_TO_QT, 'Qt5Charts.dll' ), os.path.join('./release'))
        copyFile(os.path.join(PATH_TO_QT, 'Qt5Core.dll' ), os.path.join('./release'))
        copyFile(os.path.join(PATH_TO_QT, 'Qt5DataVisualization.dll' ), os.path.join('./release'))
        copyFile(os.path.join(PATH_TO_QT, 'Qt5Gui.dll' ), os.path.join('./release'))
        copyFile(os.path.join(PATH_TO_QT, 'Qt5Network.dll' ), os.path.join('./release'))
        copyFile(os.path.join(PATH_TO_QT, 'Qt5Widgets.dll' ), os.path.join('./release'))
        copyFile(os.path.join(PATH_TO_QT, 'Qt5DataVisualization.dll' ), os.path.join('./release'))
        copyDir(os.path.join(PATH_TO_QT, '..', 'plugins/imageformats' ), os.path.join('./release/imageformats'))
        copyDir(os.path.join(PATH_TO_QT, '..', 'plugins/platforms' ), os.path.join('./release/platforms'))

    elif os.name == 'posix' or sys.platform.startswith('linux'):
        PATH_TO_QT = os.path.abspath('''/home/bruzzo/Qt_5_8/5.8/gcc_64/bin''')
        os.environ["PATH"] += os.pathsep + PATH_TO_QT + os.pathsep
        print "Generate project files for gnu make"
        safeCall("mkdir -p release")
        safeCall(os.path.join(PATH_TO_QT, 'qmake') + ''' -t app -r -o ./release/Makefile "CONFIG+=release" ./../plotter_plusplus.pro''')
        print "Build project with GNU make"
        os.chdir("./release")
        safeCall('make -f Makefile -j4')
        os.chdir("./../")

    print "\'%s\' COMPLETED IN: %s SECONDS [OK]" %( sys.argv[0], str(time.time() - t0))
