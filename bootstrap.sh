#!/usr/bin/env bash

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
# Copyright (C) 2015-2016 Jose Ricardo Ziviani - zmvar:3,17

export GTEST_DIR=/home/ziviani/ziviani/lab/googletest/googletest
export GMOCK_DIR=/home/ziviani/ziviani/lab/googletest/googlemock

printf "\e[4;34mUpdating configure and makefile\e[0m\n"
autoreconf -vfi

printf "\e[4;34mUpdating source files\e[0m\n"
python build-aux/headers.py .

printf "\e[4;34mRun ./configure [GTEST_DIR=/path/to/gtest]\e[0m\n"
