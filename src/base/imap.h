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
#include <ostream>
#include <unordered_map>

class imap
{
    struct message_t // 20 bytes
    {
        int _id;
        std::string _timestamp;
        std::string _subject;
        std::string _body;
        bool _read;
        bool _recent;
    };

    struct folder_t
    {
        long long _n_messages;
        long long _n_unreads;
        long long _n_recents;
        long long _n_subfolders;
        std::string _name;
        std::string _parents;
        std::vector<message_t> _messages;
        std::unordered_map<std::string, std::unique_ptr<folder_t>> _children;
    };

    // aliases
    using boxes_t     = std::unordered_map<std::string,
          std::unique_ptr<folder_t>>;
    using boxes_cit   = boxes_t::const_iterator;
    using strings_t   = std::vector<std::string>;
    using callback_t  = std::function<void(boxes_cit box)>;
    using callbacks_t = std::vector<callback_t>;

    public:
        imap(const tcp_client &tcp);
        ~imap();

        //folder_t get_folders() const;
        std::string get_header_by_folder(std::string &folder) const;
        void print_all(const boxes_t &parent, std::string tabs = "");

    private:
        std::string msg_id(bool generate_new = true);

        void init_imap();
        void finish_imap();

        void retrieve_messages();
        void retrieve_folders();
        void fill_folder_tree(boxes_t &folder,
                          strings_t folder_list,
                          std::string parent = "");

        void for_each_folder(const callback_t &callback) const;
        void for_each_folder(const callbacks_t &callback_list) const;

        // callbacks
        void parse_folder_details(boxes_cit box);
        void parse_message_subjects(boxes_cit box);

        friend std::ostream &operator<<(std::ostream &os, const imap &imap_obj);

    private:
        const tcp_client &_tcpclient;
        boxes_t _folders;
        char _separator;
        int _counter;
};

inline std::string imap::msg_id(bool generate_new)
{
    if (generate_new)
        return "a" + std::to_string(++_counter);

    return "a" + std::to_string(_counter);
}

inline std::ostream &operator<<(std::ostream &os, const imap &imap_obj)
{
    imap_obj.for_each_folder([&os](imap::boxes_cit box) {
            os << box->first << ": " << box->second->_parents << "\n";
            os << box->second->_n_messages << "\n";
            os << "---------------------------------------\n";
    });
    return os;
}


#endif
