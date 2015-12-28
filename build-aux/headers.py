#!/usr/bin/env python3

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# Copyright (C) 2015 Jose Ricardo Ziviani - zmvar:3,17

import sys
import os
import shutil
import re

def apply_c_cpp_header(path):
    apply_header(path, '//', 0)

def apply_bash_header(path):
    apply_header(path, '#', 2)

def apply_python_header(path):
    apply_header(path, '#', 2)

def apply_header(path, comment_symbol, line_to_start):
    def _add_comment(line):
        if line == '\n':
            return comment_symbol + '\n'
        return comment_symbol + ' ' + line

    if check_header(path, comment_symbol) is not None:
        return

    print 'Applying header to %s' % path

    template = []
    with open('build-aux/template.txt', 'r') as template:
        template = template.readlines()

    template = map(_add_comment, template)

    with open(path + '.tmp', 'w') as dest:
        with open(path, 'r') as source:
            for number, line in enumerate(source):
                if number == line_to_start:
                    template[-1] = '%s - zmvar:%s,%s\n\n' % \
                        (template[-1].strip('\n'),
                        str(line_to_start + 1),
                        str(len(template) + line_to_start))

                    dest.writelines(template)

                dest.write(line)

    shutil.move(path + '.tmp', path)

def check_header(path, comment_symbol):
    with open(path, 'r') as source:
        for line in source:
            if not line.startswith(comment_symbol):
                continue
            if 'zmvar:' not in line:
                continue

            return re.search('zmvar:(?P<sl>[0-9]+),(?P<el>[0-9]+)', line)

    return None

def main(argv):
    '''
    '''
    if len(argv) != 2:
        print 'usage: headers.py <directory>'
        sys.exit(1)

    initial_dir = argv[1]

    for root, dirs, files in os.walk(initial_dir):
        for source in files:
            if source.endswith('.h') \
               or source.endswith('.c') \
               or source.endswith('.cpp'):
                apply_c_cpp_header(os.path.join(root, source))

            elif source.endswith('.sh'):
                apply_bash_header(os.path.join(root, source))

            elif source.endswith('.py'):
                apply_python_header(os.path.join(root, source))
# main()

if __name__ == '__main__':
    main(sys.argv)
