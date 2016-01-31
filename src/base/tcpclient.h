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

#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#include <openssl/ssl.h>

#include "src/utils/logger.h"

#include <string>

class tcp_client
{
    public:
        tcp_client(std::string address, std::string port, logger &log);
        virtual ~tcp_client();

        virtual std::string send_message(const std::string &msg) const;

        tcp_client(const tcp_client &) = delete;
        tcp_client &operator=(const tcp_client &) = delete;

    protected:
        tcp_client(logger &log);

    private:
        void init_socket();
        void init_ssl_socket();
        std::string send_nosecure_message(const std::string &msg) const;
        std::string send_ssl_message(const std::string &msg) const;

    private:
        SSL *_ssl;
        int _socket;
        std::string _address;
        std::string _port;
        bool _is_secure;
        const logger &_logger;
};

#endif
