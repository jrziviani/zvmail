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
#include <stack>
#include <functional>

#include <pcrecpp.h>

using namespace std;

// constants ------------------------------------------------------------------
//const char FOLDER_REGEX[] = R"(\* LIST \(((\\\w+\s?)+)\) \"(.)\" (.+))";
const char FOLDER_REGEX[] = R"(\s?\* LIST \(((\\|\$?[a-zA-Z0-9]+\s?)+)\) \"(.)\" (.+))";

//const char FOLDER_HEADER_REGEX[] = R"(.*\* FLAGS\s?\(((\\|\$?[a-zA-Z0-9]+\s?)+).*\*\s?([0-9]+)\s?EXISTS.*\*\s?([0-9]+)\s?RECENT.*a[0-9]+\sOK.*)";
const char FOLDER_HEADER_REGEX[] = R"(.*\* FLAGS\s?\(((\\|\$?[a-zA-Z0-9]+\s?)+).*\*\s?([0-9]+)\s?EXISTS.*\*\s?([0-9]+)\s?RECENT.*a[0-9]+\sOK.*)";


// implementation -------------------------------------------------------------
imap::imap(const tcp_client &tcp) :
    _tcpclient(tcp), _counter(0)
{
    authenticate();
    retrieve_folders();
    retrieve_messages();
}

imap::~imap()
{
    cout << _tcpclient.send_message("a1 close\r\n") << endl;
    cout << _tcpclient.send_message("a1 logout\r\n") << endl;
}

void imap::authenticate()
{
    //TODO: use user/pass provided from stdin
    string id = msg_id();
    string result = _tcpclient.send_message(id + " login test test\r\n");
    if (result.find(id + " OK") == string::npos)
        throw runtime_error("IMAP authentication failed");
}

void imap::retrieve_folders()
{
    // request the list of folders for the current account to the imap
    // server and split the results in a vector
    string msg_folders = _tcpclient.send_message(msg_id() +
                                                 " list \"\" *\r\n");
    strings_t folder_list = zvmail::split(msg_folders);

    // extract the required data from the imap response and starting
    // filling the folder data structure with folders name
    pcrecpp::RE regex(FOLDER_REGEX,
            pcrecpp::RE_Options()
            .set_utf8(true));

    string flags, folder_name, separator;
    for (const auto &folder : folder_list) {
        if (!regex.FullMatch(folder,
                    &flags,
                    static_cast<void*>(NULL),
                    &separator,
                    &folder_name))
            continue;

        _separator = separator[0];
        vector<string> folders = zvmail::split(zvmail::rstrip(folder_name),
                separator[0]);
        fill_folder_tree(_folders, folders);
    }
}

void imap::fill_folder_tree(boxes_t &folder,
        strings_t folder_list,
        string parent)
{
    if (folder_list.empty())
        return;

    if (folder.find(folder_list[0]) == folder.end()) {
        folder.emplace(make_pair(folder_list[0],
                    unique_ptr<folder_t>(new folder_t)));
        folder[folder_list[0]]->_name = folder_list[0];
        folder[folder_list[0]]->_parents = parent;
        folder[folder_list[0]]->_n_messages = 0;
        folder[folder_list[0]]->_n_recents = 0;
        folder[folder_list[0]]->_n_unreads = 0;
        folder[folder_list[0]]->_n_subfolders = 0;
    }
    auto &temp = folder[folder_list[0]]->_children;
    fill_folder_tree(temp,
            vector<string>(folder_list.begin()+1, folder_list.end()),
            parent += folder_list[0] + _separator);
}

void imap::retrieve_messages()
{
    callbacks_t cbs {
        bind(&imap::parse_folder_details, this, placeholders::_1),
        bind(&imap::parse_message_subjects, this, placeholders::_1)
    };
    for_each_folder(cbs);
}

void imap::parse_folder_details(boxes_cit box)
{
    string msg = msg_id() + " select " +
                 box->second->_parents +
                 box->second->_name +
                 "\r\n";

    string headers = _tcpclient.send_message(msg);

    pcrecpp::RE regex(FOLDER_HEADER_REGEX,
            pcrecpp::RE_Options()
            .set_multiline(true)
            .set_dotall(true)
            .set_utf8(true));

    string flags;
    long long exists, recents;
    if (!regex.FullMatch(headers, &flags, static_cast<void*>(NULL), &exists, &recents))
        return;

    box->second->_n_messages = exists;
    box->second->_n_recents = recents;
    box->second->_n_subfolders = box->second->_children.size();
}

void imap::parse_message_subjects(boxes_cit box)
{
    /*
    cout << "parse_message_subjects\n";
    string msg_messages = _tcpclient.send_message("a1 fetch 1:* BODY.PEEK[HEADER.FIELDS(SUBJECT)]\r\n");
    */
}

void imap::for_each_folder(const callback_t &callback) const
{
    for_each_folder(callbacks_t{callback});
}

void imap::for_each_folder(const callbacks_t &callback_list) const
{
    using b_it = boxes_t::const_iterator;
    stack<b_it> tmp;
    b_it it = _folders.cbegin(), child_it;
    for ( ; it != _folders.cend(); ++it) {
        tmp.emplace(it);

        while (!tmp.empty()) {
            b_it top = tmp.top();
            tmp.pop();

            for (const auto &cb : callback_list)
                cb(top);

            child_it = top->second->_children.cbegin();
            for ( ; child_it != top->second->_children.cend(); ++child_it) {
                tmp.emplace(child_it);
            }
        }
    }
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

string imap::get_header_by_folder(string &folder) const
{
    string ret;

    return ret;
}


void imap::finish_imap()
{

}

void parse(string hdr)
{
    pcrecpp::RE regex(FOLDER_HEADER_REGEX,
            pcrecpp::RE_Options()
            .set_multiline(true)
            .set_dotall(true)
            .set_utf8(true));

    string flags;
    long long exists, recent;
    if (!regex.FullMatch(hdr, &flags, static_cast<void*>(NULL), &exists, &recent))
        return;


    cout << "Flags:" << flags << endl;
    cout << "Exists: " << exists << endl;
    cout << "Recent: " << recent << endl;
}

/*
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

    string hdr = "* OK [CLOSED] Previous mailbox closed.\n\
* FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft)\n\
* OK [PERMANENTFLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft \\*)] Flags permitted.\n\
* 0 EXISTS\n\
* 0 RECENT\n\
* OK [UIDVALIDITY 1380434913] UIDs valid\n\
* OK [UIDNEXT 46] Predicted next UID\n\
* OK [HIGHESTMODSEQ 92] Highest\n\
a10 OK [READ-WRITE] Select completed (0.000 secs).\n";

    hdr = "* FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft $Forwarded NonJunk $label3)\r\n* OK [PERMANENTFLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft $Forwarded NonJunk $label3 \\*)] Flags permitted.\r\n* 352 EXISTS\r\n* 0 RECENT\r\n* OK [UIDVALIDITY 1380434911] UIDs valid\r\n* OK [UIDNEXT 3712] Predicted next UID\r\n* OK [HIGHESTMODSEQ 6846] Highest\r\na2 OK [READ-WRITE] Select completed (0.000 secs).\r\n";
    //parse(hdr);


    //return 0;
    try {
        tcp_client tcp("192.168.122.55", "143");
        imap myimap(tcp);
        cout << "folders: " << myimap << endl;
    }
    catch(const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}*/
