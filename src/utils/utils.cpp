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
// Copyright (C) 2015 Jose Ricardo Ziviani - zmvar:1,15

#include "utils.h"
#include <iostream>
using namespace std;

vector<string> zvmail::split(const string &text, char delim) noexcept
{
    vector<string> strs = {};
    if (text.empty())
        return strs;

    strs.reserve(32);
    string::size_type i = 0,
        start = 0,
        offset = 0;

    for (; i < text.size(); ++i, ++offset) {
        if (text[i] != delim)
            continue;

        strs.emplace_back(text.substr(start, offset));
        start = i + 1;
        offset = string::npos;
    }

    if (i == text.size()
            && i == start + offset
            && offset > 0)
        strs.emplace_back(text.substr(start, offset));
    return strs;
}

string zvmail::rstrip(const std::string &text,
        const std::string &to_remove) noexcept
{
    if (text.empty() || to_remove.empty())
        return text;

    string::size_type items_to_remove = 0;
    for (auto text_it = text.crbegin(); text_it != text.crend(); ++text_it) {
        auto check_end = items_to_remove;
        for (auto it = to_remove.cbegin(); it != to_remove.cend(); ++it) {
            if (*text_it != *it)
                continue;

            ++items_to_remove;
        }
        if (check_end == items_to_remove)
            break;
    }

    return text.substr(0, text.size() - items_to_remove);
}

template<typename T, typename... Ts>
std::unique_ptr<T> zvmail::make_unique(Ts&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

/*
int main()
{
    using namespace zvmail;
    auto st = string("This is a test");
    cout << rstrip(st, "est") << endl;
    return 0;
}
*/
