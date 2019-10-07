#!/usr/bin/python

#Copyright 2019 Adobe. All rights reserved.
#This file is licensed to you under the Apache License, Version 2.0 (the "License");
#you may not use this file except in compliance with the License. You may obtain a copy
#of the License at http://www.apache.org/licenses/LICENSE-2.0

#Unless required by applicable law or agreed to in writing, software distributed under
#the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
#OF ANY KIND, either express or implied. See the License for the specific language
#governing permissions and limitations under the License.

import sys
import os
import platform
import argparse
from subprocess import PIPE, Popen, STDOUT
import json
import fnmatch
import shutil
import ntpath
import errno
import multiprocessing
from multiprocessing import Manager, Pool, Lock
import difflib
import xml.etree.ElementTree as ET

class DirectoryIterator:
    def __init__(self, paths, recursive):
        self.files = []
        self.testexpectations = {}
        self.index = -1
        for path in paths:
            (dir, file) = os.path.split(path)
            if os.path.isfile(path):
                self.private_addFile(dir, file)
            elif os.path.isdir(path):
                self.private_walkDir(path, recursive)
            elif os.path.isdir(dir):
                self.private_walkDir(dir, False, file)
            else:
                print("  Invalid path")

    def currentPath(self):
        if self.index < 0 or self.index >= len(self.files):
            return None
        return self.files[self.index]

    def nextPath(self):
        self.index = self.index + 1
        return self.currentPath()

    def fileNum(self):
        return count(self.files)

    def fileArray(self):
        return self.files

    def testExpectations(self):
        return self.testexpectations

    def private_parseTestExpectation(self, testExpectationsPath):
        dirname = os.path.dirname(testExpectationsPath)
        with open(testExpectationsPath) as file:
            for line in file:
                chunks = line.split()
                if not chunks:
                    continue
                if chunks[0] == '#':
                    continue
                testpath = os.path.abspath(os.path.join(dirname, chunks[0]))
                for i, chunk in enumerate(chunks):
                    if chunk == '[':
                        state = chunks[i + 1]
                        self.testexpectations[testpath] = state

    def private_addFile(self, dirname, filename, pattern = None):
        if not self.private_isFileExcluded(filename) \
            and self.private_isFileAllowed(filename) \
            and (not pattern or fnmatch.fnmatch(filename, pattern)):
            self.files.append(os.path.join(dirname, filename))

    def private_walkDir(self, path, recursive, pattern = None):
        for dirname, subdirnames, filenames in os.walk(path):
            base = os.path.basename(dirname)
            if base.startswith('.'):
                continue
            for filename in filenames:
                filepath = os.path.join(dirname, filename)
                if filename == 'TestExpectations' and os.path.exists(filepath):
                    print("Found TestExpectations file at: " + filepath)
                    self.private_parseTestExpectation(filepath)
            for filename in filenames:
                filepath = os.path.abspath(os.path.join(dirname, filename))
                if filepath in self.testexpectations and (self.testexpectations[filepath] == 'Skip' or self.testexpectations[filepath] == 'Crash'):
                    continue
                self.private_addFile(dirname, filename, pattern)
            if not recursive:
                return

    def private_isFileExcluded(self, filename):
        if filename.startswith('.'):
            return True
        if filename.endswith('.txt'):
            return True
        for exc in DirectoryIterator.exceptions:
            if filename.find(exc) != -1:
                return True
        return False

    def private_isFileAllowed(self, filename):
        if filename.endswith('svg'):
            return True
        return False

    exceptions = ['-expected.', '-actual.', '-diff.']

def compare_text_files(path_1, path_2, diffpath):
    different = False
    l1 = l2 = ' '
    text1_lines = []
    text2_lines = []

    with open(path_1) as f1, open(path_2) as f2:
        while l1 != '' and l2 != '':
            l1 = f1.readline()
            text1_lines.append(l1)
            l2 = f2.readline()
            text2_lines.append(l2)
            if l1 != l2:
                different = True
    if not different:
        return True

    if diffpath != "":
        try:
            os.remove(diffpath)
        except OSError:
            pass # do nothing, just continue execution, the file probably doesn't exist

    diff = difflib.unified_diff(text1_lines, text2_lines, lineterm='\n')
    file = open(diffpath, "w")
    file.write(''.join(list(diff)))
    file.close()

    return False

def readLine(p):
    line = ""
    c = p.stdout.read(1)
    while c:
        if c == '\n' or c == '\r':
            if line:
                return line
        else:
            line += c
        if line == '% ':
            return line
        c = p.stdout.read(1)
    return line

def exportTestFilesSequential(files, args):
    hasError = 0
    while True:
        p = None
        isExpectedFileMissing = False
        inputFile = files.nextPath()
        if not inputFile or not os.path.exists(inputFile):
            return hasError
    
        (expectedDir, fname) = os.path.split(inputFile)
        (fbase, fext) = os.path.splitext(fname)
    
        expectedFile = os.path.abspath(os.path.join(expectedDir, fbase + '.txt'))
        # TODO: preserve sub-dirs
        actualFile = os.path.abspath(os.path.join(args.result_dir, fbase + '.txt'))
        diffFile = os.path.abspath(os.path.join(args.result_dir, fbase + '-diff.txt'))
        elementRef = fbase.startswith('elem-')
        if not os.path.exists(expectedFile):
            isExpectedFileMissing = True
            if not elementRef:
                p = Popen([args.program, inputFile, expectedFile])
            else:
                p = Popen([args.program, inputFile, expectedFile, 'ref'])                
            if p == None:
                print('Error opening testapp')
                hasError = -1
                continue
            print('Created missing expectation file: ' + expectedFile)
        else:
            if not elementRef:
                p = Popen([args.program, inputFile, actualFile])
            else:
                p = Popen([args.program, inputFile, actualFile, 'ref'])    
            if p == None:
                print('Error opening testapp')
                hasError = -1
                continue
            p.wait()
            if not compare_text_files(expectedFile, actualFile, diffFile):
                print('   ' + inputFile + ' FAILED.')
                hasError = -1
            else:
                print('   ' + inputFile + ' passed.')

    return hasError
    

def runTestApp(args):
    filesIterator = DirectoryIterator(args.path, args.recursive)
    files = filesIterator.fileArray()
    testexpectations = filesIterator.testExpectations()

    error = exportTestFilesSequential(filesIterator, args)
    if error == -1:
        return error

    return 0

def main(argv):
    parser = argparse.ArgumentParser(description='Arguments for RunAiTests.')

    parser.add_argument("--program", dest="program", required=False, help="path to test application")
    parser.add_argument("--debug", dest="debug", action="store_true", required=False, default=True, help="debug build")
    parser.add_argument("--output", dest="result_dir", required=False, help="path for actual results")
    parser.add_argument("--rebase", dest="rebase", action="store_true", default=False, help="overwrite all the expected files")
    parser.add_argument("--recursive", dest="recursive", action="store_true", default=False, help="walk the directories recursively")
    parser.add_argument("--port", dest="port_dir", required=False, help="expected files directory for specific port")
    parser.add_argument("--tests", dest="path", nargs="+", required=True, help="test files or folders")

    args = parser.parse_args()

    if args.program:
        args.program = os.path.realpath(args.program)
    else:
        plat = ''
        if platform.system() == 'Darwin':
            plat = 'mac'
        elif platform.system() == 'Windows':
            plat = 'win64'
        elif platform.system() == 'Linux':
            plat = 'linux'
        else:
            raise 'Unknown platform. Specify path to test application with --path'
        args.program = os.path.relpath(os.path.join(
                            os.path.dirname(os.path.abspath(__file__)),
                            '..', 'build', plat, 'example', 'testText',
                            '' if platform.system() == 'Linux' else 'Debug' if args.debug else 'Release',
                            'testSVGNative.exe' if platform.system() == 'Windows' else 'testSVGNative'))
    print(args.program)
    if not os.path.exists(args.program):
        raise 'Test application not found at: ' + args.program + '. Specify correct path with --path.'

    # Folder where we save the result to
    if not args.result_dir:
        args.result_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'viewer', 'output')
    if not os.path.isdir(args.result_dir):
        try:
            os.makedirs(args.result_dir)
        except OSError:
            raise 'Could not create test result files directory'
    args.result_dir = os.path.relpath(args.result_dir)

    ret = runTestApp(args)
    if ret != 0:
        raise '[ERROR] Unexpected error from testapp'

if __name__ == "__main__":
    main(sys.argv[1:])
