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

#ifndef _LOGGER_H
#define _LOGGER_H

#include <ostream>
#include <fstream>
#include <string>
#include <mutex>

class logger
{
    public:

        logger(const std::string &filepath);
        logger();
        ~logger();

        template<typename... Ts>
        void debug(const Ts&... msgs);

        template<typename... Ts>
        void info(const Ts&... msgs);

        template<typename... Ts>
        void warn(const Ts&... msgs);

        template<typename... Ts>
        void error(const Ts&... msgs);

        template<typename... Ts>
        void critical(const Ts&... msgs);

    private:
        template<typename T>
        void log(const T &msg);

        template<typename T, typename... Ts>
        void log(const T &msg, const Ts&... msgs);

    private:
        std::ofstream _outfile;
        std::ostream _output;
        std::mutex _outmutex;
};

#endif
