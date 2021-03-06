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

#include "imap.h"
#include "src/utils/utils.h"

#include <iostream>
#include <stdexcept>
#include <stack>
#include <functional>

#include <pcrecpp.h>


// namespace members ----------------------------------------------------------
using std::endl;
using std::cout;
using std::stack;
using std::vector;
using std::unique_ptr;
using std::runtime_error;
using std::placeholders::_1;
using std::to_string;


// constants ------------------------------------------------------------------
const char FOLDER_REGEX[] = R"(\s?\* LIST \(((\\|\$?[a-zA-Z0-9]+\s?)+)\) \"(.)\" (.+))";
const char FOLDER_HEADER_REGEX[] = R"(.*\* FLAGS\s?\(((\\|\$?[a-zA-Z0-9]+\s?)+).*\*\s?([0-9]+)\s?EXISTS.*\*\s?([0-9]+)\s?RECENT.*a[0-9]+\sOK.*)";

// implementation -------------------------------------------------------------
imap::imap(const tcp_client &tcp, logger &log) :
    _tcpclient(tcp), _counter(0), _logger(log)
{
    authenticate();
    retrieve_folders();
    retrieve_messages();
}

imap::~imap()
{
    _tcpclient.send_message(msg_id() + " close\r\n");
    _tcpclient.send_message(msg_id() + " logout\r\n");
}

void imap::authenticate()
{
    _logger.info("Authenticating ,", "userid", " in ", "tcp server");
    //TODO: use user/pass provided from stdin
    string id = msg_id();
    string result = _tcpclient.send_message(id + " login test test\r\n");
    if (result.find(id + " OK") == string::npos) {
        _logger.critical("User has not permission");
        throw runtime_error("IMAP authentication failed");
    }
}

void imap::retrieve_folders()
{
    // request the list of folders for the current account to the imap
    // server and split the results in a vector
    string msg_folders = _tcpclient.send_message(msg_id() +
                                                 " list \"\" *\r\n");

    _logger.debug("folders received:", msg_folders);
    strings_t folder_list = zvmail::split(msg_folders);

    // create the regex object to match every folder listed in the imap
    // response
    pcrecpp::RE regex(FOLDER_REGEX,
            pcrecpp::RE_Options()
            .set_utf8(true));

    // for each folder receveied, extract all details and fill up the
    // data structure with all names found
    // TODO: it is not currently using the flags extracted from the imap
    // response
    string flags, folder_name, separator;
    for (const auto &folder : folder_list) {
        if (!regex.FullMatch(folder,
                    &flags,
                    static_cast<void*>(NULL),
                    &separator,
                    &folder_name))
            continue;

        _logger.debug("folders considered:", folder);
        _separator = separator[0];

        // split the folder path in a vector and create each as nodes
        // in _folder structure:
        // inbox/received/test -> folders[0]: inbox
        //                        folders[1]: received
        //                        folders[2]: test
        vector<string> folders = zvmail::split(
                zvmail::rstrip(folder_name),
                separator[0]);
        fill_folder_tree(_folders, folders);
    }
}

void imap::fill_folder_tree(boxes_t &folder,
        strings_t folder_list,
        string parent)
{
    // TODO: create a more intelligent approach to skip existing
    // folders instead of opening it recursively or even make it
    // iterative like for_each_folder()
    if (folder_list.empty())
        return;

    // create the folder in the struct if it does not exist yet
    //   '_parents' is a string with the formated path until this
    //              current folder, for example:
    //              for folder 'inbox', _parents = ""
    //                      'received', _parents = "inbox"
    //                          'test', _parents = "inbox/received"
    //              this is to make it easier when requesting data
    //              for a particular folder
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

    // call this method recursively to add the remaining folders in
    // the _folders struct
    auto &temp = folder[folder_list[0]]->_children;
    fill_folder_tree(temp,
            vector<string>(folder_list.begin() + 1, folder_list.end()),
            parent += folder_list[0] + _separator);
}

void imap::retrieve_messages()
{
    callbacks_t cbs {
        bind(&imap::parse_folder_details, this, _1),
        bind(&imap::parse_message_subjects, this, _1)
    };
    for_each_folder(cbs);
}

void imap::parse_folder_details(boxes_cit box)
{
    string msg = msg_id() + " select " +
                 box->second->_parents +
                 box->second->_name +
                 "\r\n";
    _logger.debug("Folder details:", msg);

    string headers = _tcpclient.send_message(msg);
    _logger.debug("Folder response:", headers);

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
    cout << box->second->_name << ": " << box->second->_n_messages << "\n";
}

void imap::parse_message_subjects(boxes_cit box)
{
    string msg_message = _tcpclient.send_message(msg_id() +
        " fetch 1:* (FLAGS BODY.PEEK[HEADER.FIELDS (DATE FROM SUBJECT)])\r\n");

    cout << msg_message << "\n";
}

void imap::for_each_folder(const callback_t &callback) const
{
    // handy method for callers with only one callback
    for_each_folder(callbacks_t{callback});
}

void imap::for_each_folder(const callbacks_t &callback_list) const
{
    using b_it = boxes_t::const_iterator;
    stack<b_it> tmp;

    // basically a DFS through the folders, each folder calls all
    // the callbacks interested to receive a const iterator of the
    // current folder, note that boxes_t is an unordered map (hash table)
    // of <string, unique_ptr<message_t>>, it means that such pair is
    // imutable but the content of the value (message_t) is perfectly
    // mutable
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

void imap::finish_imap()
{

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
        //tcp_client tcp("192.168.122.55", "143");
        tcp_client tcp("imap.ziviani.net", "143");
        imap myimap(tcp);
        cout << "folders: " << myimap << endl;
    }
    catch(const std::exception &e) {
        std::cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
*/
