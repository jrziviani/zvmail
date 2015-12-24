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
#include <vector>


class imap
{
    using folder_list = std::vector<std::string>;

    public:
        imap(const tcp_client &tcp);
        ~imap();

        folder_list get_folders() const;
        std::string get_header_by_folder(std::string &folder) const;

    private:
        void init_imap();
        void finish_imap();

    private:
        const tcp_client &_tcpclient;
        folder_list _folders;
};


#endif
