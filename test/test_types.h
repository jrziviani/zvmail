#ifndef _TEST_TYPES_H
#define _TEST_TYPES_H

#include <gtest/gtest.h>

#include "src/base/types.h"

#include <string>
#include <iostream>

using testing::_;
using testing::Return;
using testing::AnyNumber;

TEST (types, create_contact)
{
    contact_t contact("test name", "email@test.com");
    EXPECT_EQ(contact.jsonfy(),
            "{\"_name\"  : \"test name\", \"_email\" : \"email@test.com\"}");
}


#endif
