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

#include "imap.h"

#include <iostream>
#include <stdexcept>

using namespace std;

imap::imap(const tcp_client &tcp) :
    _tcpclient(tcp)
{
    init_imap();
    get_folders();
}

imap::~imap()
{
}

imap::folder_list imap::get_folders() const
{
    imap::folder_list ret;

    cout << _tcpclient.send_message("a2 SELECT INBOX\r\n") << endl;

    return ret;
}

string imap::get_header_by_folder(std::string &folder) const
{
    string ret;

    return ret;
}

void imap::init_imap()
{
    string result = _tcpclient.send_message("a1 login jrziviani@gmail.com PASS\r\n");
    cout << result << endl;

}

void imap::finish_imap()
{

}

int main()
{
    try {
        tcp_client tcp("imap.gmail.com", "993");
        imap myimap(tcp);
    }
    catch(runtime_error &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
