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

#ifndef _TEST_TCPCLIENT_H
#define _TEST_TCPCLIENT_H

#include <gtest/gtest.h>
#include <string>
#include <exception>

#include "src/base/tcpclient.h"
#include "src/utils/logger.h"

TEST (tcp_client, send_scheme)
{
    logger log;
    EXPECT_ANY_THROW(tcp_client tcp("http://ziviani.net", "80", log));
}

TEST(tcp_client, http_response)
{
    logger log;
    tcp_client tcp("ziviani.net", "80", log);
    std::string response = tcp.send_message("GET / HTTP/1.1\r\n"
            "Host: ziviani.net\r\n"
            "Scheme: http\r\n"
            "Connection: close\r\n\r\n");

    EXPECT_LT(0, response.size());
    EXPECT_EQ(response.substr(0, 15), std::string("HTTP/1.1 200 OK"));
}

TEST(tcp_client, connection_refused)
{
    logger log;
    EXPECT_THROW(tcp_client("ziviani.net", "8080", log),
            std::runtime_error);
    try {
        tcp_client("ziviani.net", "8080", log);
    } catch (const std::runtime_error &e) {
        EXPECT_STREQ(e.what(), "Connection refused");
    }
}
/*
TEST(tcp_client, connection_timeout)
{
    tcp_client tcp("ziviani.net", "80");
    EXPECT_THROW(tcp.send_message("GET / HTTP/1.1"),
            std::runtime_error);

    try {
        tcp.send_message("GET / HTTP/1.1");
    } catch (const std::runtime_error &e) {
        EXPECT_STREQ(e.what(), "Resource temporarily unavailable");
    }
}
*/
TEST(tcp_client, https_response)
{
    logger log;
    tcp_client tcp("twitter.com", "443", log);
    std::string response = tcp.send_message("GET / HTTP/1.1\r\n"
            "Host: twitter.com\r\n"
            "Scheme: https\r\n"
            "Connection: close\r\n\r\n");

    EXPECT_LT(0, response.size());
    EXPECT_EQ(response.substr(0, 15), std::string("HTTP/1.1 200 OK"));
}

#endif
