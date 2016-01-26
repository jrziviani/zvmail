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

#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <vector>
#include <memory>

namespace zvmail
{
    std::vector<std::string> split(const std::string &text,
            char delim = '\n') noexcept;

    std::string rstrip(const std::string &text,
            const std::string &to_remove = "\n\t\r") noexcept;

    template<typename T, typename... Ts>
    std::unique_ptr<T> make_unique(Ts&&... params);
};

#endif
