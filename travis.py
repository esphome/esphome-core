from __future__ import print_function
import glob
import sys
import codecs
import os.path

errors = []


def find_all(a_str, sub):
    for i, line in enumerate(a_str.splitlines()):
        column = 0
        while True:
            column = line.find(sub, column)
            if column == -1:
                break
            yield i, column
            column += len(sub)


files = []
for root, _, fs in os.walk('src'):
    for f in fs:
        _, ext = os.path.splitext(f)
        if ext in ('.h', '.c', '.cpp', '.tcc'):
            files.append(os.path.join(root, f))
files.sort()

for f in files:
    try:
        with codecs.open(f, 'r', encoding='utf-8') as f_handle:
            content = f_handle.read()
    except UnicodeDecodeError:
        errors.append("File {} is not readable as UTF-8. Please set your editor to UTF-8 mode."
                      "".format(f))
    for line, col in find_all(content, '\t'):
        errors.append("File {} contains tab character on line {}:{}. "
                      "Please convert tabs to spaces.".format(f, line, col))
    for line, col in find_all(content, '\r'):
        errors.append("File {} contains windows newline on line {}:{}. "
                      "Please set your editor to unix newline mode.".format(f, line, col))

for error in errors:
    print(error)

sys.exit(len(errors))
