// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// Copyright (C) 2015-2016 Jose Ricardo Ziviani - zmvar:1,15

#include "logger.h"

#include <iostream>
#include <thread>
#include <functional>

using std::string;
using std::ofstream;
using std::thread;
using std::lock_guard;
using std::mutex;
using std::ref;
using std::cout;

logger::logger(const string &file) :
    _outfile(file, std::ios::out | std::ios::app),
    _output(_outfile.rdbuf())
{
}

logger::logger() :
    _output(cout.rdbuf())
{
}

logger::~logger()
{
}

void logging(logger &log, unsigned int id)
{
    log.warn("[thread ", id, "]: warn");
    log.error("[thread ", id, "]: error");
}
/*
int main()
{
    //logger log("/tmp/zvmail.log");
    logger log;
    thread _t[20];

    for (unsigned int i = 0; i < 20; ++i) {
        _t[i] = thread(logging, ref(log), i);
    }

    for (unsigned int i = 0; i < 20; ++i) {
        _t[i].join();
    }

    return 0;
}
*/
