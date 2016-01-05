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

#ifndef _TEST_IMAP_H
#define _TEST_IMAP_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "src/base/tcpclient.h"
#include "src/base/imap.h"

#include <string>
#include <iostream>

using testing::_;
using testing::Return;
using testing::HasSubstr;
using testing::AnyNumber;

class mock_tcp_client : public tcp_client
{
    public:
        MOCK_CONST_METHOD1(send_message, std::string(const std::string &msg));
};

TEST (imap, login_fail)
{
    mock_tcp_client client;
    EXPECT_CALL(client, send_message(HasSubstr("login")))
            .WillOnce(Return("a1 BAD message"));
    try {
        imap myimap(client);
    } catch (const std::runtime_error &e) {
        EXPECT_STREQ(e.what(), "IMAP authentication failed");
    }
}

TEST (imap, login_pass)
{
    mock_tcp_client client;
    EXPECT_CALL(client, send_message(_))
            .Times(AnyNumber());
    EXPECT_CALL(client, send_message(HasSubstr("login")))
            .WillOnce(Return("a1 OK success msg"));

    imap myimap(client);
    /*
    EXPECT_CALL(client, send_message(HasSubstr("logout")));
    EXPECT_CALL(client, send_message(HasSubstr("close")));
    EXPECT_CALL(client, send_message(HasSubstr("list")));
    EXPECT_CALL(client, send_message(HasSubstr("select")))
            .Times(AnyNumber());*/


    //EXPECT_CALL(client, Forward(_));

    //std::cout << myimap << std::endl;
}

#endif
