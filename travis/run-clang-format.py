#!/usr/bin/env python

from __future__ import print_function

import argparse
import click
import glob
import json
import multiprocessing
import os
import fnmatch
import re
import shutil
import subprocess
import sys
import tempfile
import threading
import traceback


is_py2 = sys.version[0] == '2'

if is_py2:
    import Queue as queue
else:
    import queue as queue


HEADER_FILTER = r'^.*/src/esphome/.*'


def make_absolute(f, directory):
    if os.path.isabs(f):
        return f
    return os.path.normpath(os.path.join(directory, f))


def get_tidy_invocation(f, inplace):
    """Gets a command line for clang-tidy."""
    start = ['clang-format-7']
    if inplace:
        start.append('-i')
    start.append(f)
    return start


def run_tidy(args, queue, lock):
    """Takes filenames out of queue and runs clang-tidy on them."""
    while True:
        name = queue.get()
        invocation = get_tidy_invocation(name, args.inplace)

        proc = subprocess.Popen(invocation, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE)
        output, err = proc.communicate()
        with lock:
            if proc.returncode != 0:
                print(' '.join(invocation))
                print(output.decode('utf-8'))
                print(err.decode('utf-8'))
        queue.task_done()


def progress_bar_show(value):
    if value is None:
        return ''
    return os.path.relpath(value, os.path.join(os.getcwd(), 'src', 'esphome'))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-j', '--jobs', type=int,
                        default=multiprocessing.cpu_count(),
                        help='number of tidy instances to be run in parallel.')
    parser.add_argument('files', nargs='*', default=['src/esphome'],
                        help='files to be processed (regex on path)')
    parser.add_argument('-i', '--inplace', action='store_true',
                        help='apply fix-its')
    parser.add_argument('-q', '--quiet', action='store_false',
                        help='Run clang-tidy in quiet mode')
    args = parser.parse_args()

    file_name_re = re.compile('|'.join(args.files))
    files = []
    for root, dirnames, filenames in os.walk(os.path.join('src', 'esphome')):
        for filename in fnmatch.filter(filenames, '*.cpp'):
            files.append(os.path.normpath(os.path.join(os.getcwd(), root, filename)))
        for filename in fnmatch.filter(filenames, '*.h'):
            files.append(os.path.normpath(os.path.join(os.getcwd(), root, filename)))
        for filename in fnmatch.filter(filenames, '*.tcc'):
            files.append(os.path.normpath(os.path.join(os.getcwd(), root, filename)))
    files = sorted([f for f in files if file_name_re.search(f)])
    max_task = args.jobs

    return_code = 0
    try:
        # Spin up a bunch of tidy-launching threads.
        task_queue = queue.Queue(max_task)
        # List of files with a non-zero return code.
        lock = threading.Lock()
        for _ in range(max_task):
            t = threading.Thread(target=run_tidy,
                                 args=(args, task_queue, lock))
            t.daemon = True
            t.start()

        # Fill the queue with files.
        with click.progressbar(files, width=30, file=sys.stderr,
                               item_show_func=progress_bar_show) as bar:
            for name in bar:
                task_queue.put(name)

        # Wait for all threads to be done.
        task_queue.join()

    except KeyboardInterrupt:
        print()
        print('Ctrl-C detected, goodbye.')
        if tmpdir:
            shutil.rmtree(tmpdir)
        os.kill(0, 9)

    sys.exit(return_code)


if __name__ == '__main__':
    main()
