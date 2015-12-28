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
#include "src/utils/utils.h"

#include <iostream>
#include <stdexcept>

#include <pcrecpp.h>

using namespace std;

imap::imap(const tcp_client &tcp) :
    _tcpclient(tcp)
{
    init_imap();
    retrieve_folders();
    retrieve_messages();
}

imap::~imap()
{
    cout << _tcpclient.send_message("a1 close\r\n") << endl;
    cout << _tcpclient.send_message("a1 logout\r\n") << endl;
}

void imap::retrieve_messages()
{
    cout << _tcpclient.send_message("a1 select INBOX\r\n");
    string msg_messages = _tcpclient.send_message("a1 fetch 1:* BODY.PEEK[HEADER.FIELDS (SUBJECT)]\r\n");
    cout << msg_messages << endl;
}

void imap::retrieve_folders()
{
    // request the list of folders for the current account to the imap
    // server and split the results in a vector
    string msg_folders = _tcpclient.send_message("a1 list \"\" *\r\n");
    vector<string> folder_list = zvmail::split(msg_folders);

    // extract the required data from the imap response and starting
    // filling the folder data structure with folders name
    pcrecpp::RE regex(R"(\* LIST \(((\\\w+\s?)+)\) \"(.)\" (.+))");
    string flags, folder_name, separator;
    for (const auto &folder : folder_list) {
        if (!regex.FullMatch(folder,
                    &flags,
                    static_cast<void*>(NULL),
                    &separator,
                    &folder_name))
            continue;

        vector<string> folders = zvmail::split(zvmail::rstrip(folder_name),
                separator[0]);
        fill_folders(_folders, folders);
    }

    // TODO: replace by a logger...
    print_all(_folders);
}

void imap::print_all(const boxes_t &parent, string tabs)
{
    for (const auto &it : parent) {
        cout << tabs << it.first << endl;

        if (!it.second->_children.empty()) {
            tabs.push_back('\t');
            print_all(it.second->_children, tabs);
            tabs.pop_back();
        }
    }
}

void imap::fill_folders(boxes_t &parent, vector<string> folders)
{
    if (folders.empty())
        return;

    if (parent.find(folders[0]) == parent.end()) {
        parent.emplace(make_pair(folders[0],
                    unique_ptr<folder_t>(new folder_t)));
    }
    auto &temp = parent[folders[0]]->_children;
    fill_folders(temp, vector<string>(folders.begin()+1, folders.end()));
}

string imap::get_header_by_folder(string &folder) const
{
    string ret;

    return ret;
}

void imap::init_imap()
{
    string result = _tcpclient.send_message("a1 login test test\r\n");
    if (result.find("AUTHENTICATIONFAILED") != string::npos)
        throw runtime_error("IMAP Authentication failed");
}

void imap::finish_imap()
{

}


int main()
{
    string tmp = "* LIST (\\HasChildren) \".\" INBOX\n\
* LIST (\\HasNoChildren \\UnMarked) \".\" INBOX.SPAM\n\
* LIST (\\HasNoChildren \\UnMarked \\Junk) \".\" INBOX.Spam\n\
* LIST (\\HasChildren \\UnMarked) \".\" INBOX.Archives\n\
* LIST (\\HasNoChildren \\UnMarked) \".\" INBOX.Archives.2014\n\
* LIST (\\HasNoChildren \\Marked) \".\" INBOX.Trash\n\
* LIST (\\HasNoChildren \\UnMarked) \".\" INBOX.LinkedIn\n\
* LIST (\\HasNoChildren \\UnMarked) \".\" INBOX.Sent\n\
* LIST (\\HasNoChildren \\UnMarked) \".\" INBOX.Drafts\n\
a1 OK List completed.\n";

    try {
        tcp_client tcp("192.168.122.55", "143");
        imap myimap(tcp);
    }
    catch(const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
