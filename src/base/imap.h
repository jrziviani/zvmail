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

#ifndef _IMAP_H
#define _IMAP_H

#include "tcpclient.h"
#include <string>
#include <set>
#include <vector>
#include <memory>
#include <unordered_map>

class imap
{
    struct message_t
    {
        int _id;
        std::string _timestamp;
        std::string _subject;
        bool _read;
        bool _recent;
    };

    struct folder_t
    {
        long _n_messages;
        long _n_unreads;
        long _n_recents;
        long _n_subfolders;
        std::vector<message_t> _messages;
        std::unordered_map<std::string, std::unique_ptr<folder_t>> _children;
    };

    using boxes_t = std::unordered_map<std::string, std::unique_ptr<folder_t>>;

    public:
        imap(const tcp_client &tcp);
        ~imap();

        //folder_t get_folders() const;
        std::string get_header_by_folder(std::string &folder) const;
        void print_all(const boxes_t &parent, std::string tabs = "");

    private:
        void init_imap();
        void finish_imap();
        void retrieve_messages();
        void retrieve_folders();
        void fill_folders(boxes_t &parent,
                std::vector<std::string> folders);

    private:
        const tcp_client &_tcpclient;
        boxes_t _folders;
};


#endif
