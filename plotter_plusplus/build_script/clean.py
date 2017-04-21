#!/usr/bin/env python
# Total clean folder before prepare final distribution

import os, shutil, sys, re

def wildcardToRegexp(wildcardRule):
    wildcardRule = re.sub(r'\.', r'(\.)', wildcardRule)
    wildcardRule = re.sub(r'\*', r'(.)*', wildcardRule)
    wildcardRule = re.sub(r'\?', r'(.)', wildcardRule)
    return wildcardRule + "$"

def remove(path):
    if os.path.isdir(path):
        shutil.rmtree(path)
        print "Folder '", path, "' cleaning completed..."
    elif os.path.isfile(path):
        os.remove(path)
        print "File '", path, "' cleaning completed..."
    else:
        print "Path '", path, "' does not exist..."

def removeFilesByWildcard(path, wildcardRule):
    for root, dirs, files in os.walk(path):
        for fname in files:
            full_fname = os.path.join(root, fname)
            if (re.match(wildcardToRegexp(wildcardRule), full_fname) != None):
                remove(full_fname)

if __name__ == "__main__":
    remove("./debug")
    remove("./release")
    remove("./plotter_plusplus.vcxproj")
    remove("./plotter_plusplus.vcxproj.filters")
    remove(".qmake.stash")
    removeFilesByWildcard("./", "*.h")
    removeFilesByWildcard("./", "*.o")
    removeFilesByWildcard("./", "*.cpp")

