#!/usr/bin/env python
import zipfile, os, shutil, sys, re

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

def copyFile(src_path, dst_path):
    print "COPY ", src_path, "=>", dst_path
    shutil.copy(src_path, dst_path)

def zipdir(path, ziph):
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file))


if __name__ == "__main__":
    removeFilesByWildcard("./release", "*.h")
    removeFilesByWildcard("./release", "*.hpp")
    removeFilesByWildcard("./release", "*.cpp")
    removeFilesByWildcard("./release", "*.obj")
    removeFilesByWildcard("./release", "*.o")
    removeFilesByWildcard("./release", "*.log")
    removeFilesByWildcard("./release", "*.h.cbt")
    remove("./release/plotter_plusplus.tlog")
    remove("./release/Makefile")
    copyFile(os.path.join('./../../', 'README.md' ), os.path.join('./release', 'README.md'))

    zipf = zipfile.ZipFile('release_plotter_plus_plus.zip', 'w', zipfile.ZIP_DEFLATED)
    zipdir('./release', zipf)
    zipf.close()
