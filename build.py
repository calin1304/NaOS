#!/usr/bin/python3

import subprocess
import os
import scripts.bincopy
import scripts.mkfsfat12

ROOT_DIR = os.getcwd()
SOURCE_DIR = ROOT_DIR + "/src"
OBJ_DIR = ROOT_DIR + "/obj"
BUILD_DIR = ROOT_DIR + "/build"
SCRIPT_DIR = ROOT_DIR + "/scripts"


def checkDirectories():
    ''' Function checks if needed directories exit. Creates any missing directories.'''
    if not os.path.exists(OBJ_DIR):
        os.mkdir(OBJ_DIR)
    if not os.path.exists(BUILD_DIR):
        os.mkdir(BUILD_DIR)

if __name__ == "__main__":
    checkDirectories()
    os.chdir(SOURCE_DIR)
    status = subprocess.call(
        ["nasm", "-f bin", "-o {0}".format(OBJ_DIR + "/stage1.bin"), "{0}".format("stage1.asm")])
    if status != 0:
        exit()
    status = subprocess.call(
        ["nasm", "-f bin", "-o {0}".format(OBJ_DIR + "/stage2.bin"), "{0}".format("stage2.asm")])
    if status != 0:
        exit()
    print("[ + ] Assembled all file\n")

    os.chdir(ROOT_DIR)
    # scripts.mkfsfat12.make_empty_img(BUILD_DIR + "/build.img")
    scripts.bincopy.write_bytes(
        OBJ_DIR + "/stage1.bin", BUILD_DIR + "/build.img")
    scripts.bincopy.write_bytes(
        OBJ_DIR + "/stage2.bin", BUILD_DIR + "/build.img", seek=1)
