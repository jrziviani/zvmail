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

#ifndef _TEST_TYPES_H
#define _TEST_TYPES_H

#include <gtest/gtest.h>

#include "src/base/types.h"

#include <string>
#include <iostream>
#include <locale>
#include <ctime>

using testing::_;
using testing::Return;
using testing::AnyNumber;

TEST (types, create_contact)
{
    contact_t contact(L"test name", L"email@test.com");
    EXPECT_EQ(contact.jsonfy(),
            L"{\"_name\"  : \"test name\", \"_email\" : \"email@test.com\"}");
}

TEST (types, create_contact_utf)
{
    //std::wcout.sync_with_stdio(false);
    //std::wcout.imbue(std::locale("en_US.utf8"));

    contact_t contact(L"☠ ♛ 㔗", L"email@test.com");
    EXPECT_EQ(contact.jsonfy(),
            L"{\"_name\"  : \"☠ ♛ 㔗\", \"_email\" : \"email@test.com\"}");

    //std::wcout << contact.jsonfy() << std::endl;
    //std::wcout << L"this is a test - \u32E1" << std::endl;
}

TEST (types, create_message)
{
    std::time_t result = std::time(nullptr);
    wchar_t wstr[100];
    std::wcsftime(wstr, 100, L"%c", std::localtime(&result));

    message_t message;
    message.is_read(false);
    message.is_recent(false);
    message.id(230);
    message.header(L"");
    message.subject(L"A new subject \u32E1");
    message.body(L"\u6548\u6549\n\nThis is the message body.");
    message.timestamp(wstr);
    message.from(contact_t(L"test name", L"test@test.com"));
    message.to({contact_t(L"to name", L"to@test.com"),
                contact_t(L"another name", L"another@test.com")});

    std::wcout.sync_with_stdio(false);
    std::wcout.imbue(std::locale("en_US.utf8"));
    std::wcout << message.jsonfy() << std::endl;
}

#endif
