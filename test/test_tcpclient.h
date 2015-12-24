#include <gtest/gtest.h>
#include <string>
#include <exception>

#include "src/base/tcpclient.h"

TEST (tcp_client, send_scheme)
{
    EXPECT_ANY_THROW(tcp_client tcp("http://ziviani.net", "80"));
}

TEST(tcp_client, http_response)
{
    tcp_client tcp("ziviani.net", "80");
    std::string response = tcp.send_message("GET / HTTP/1.1\r\n"
            "Host: ziviani.net\r\n"
            "Scheme: http\r\n"
            "Connection: close\r\n\r\n");

    EXPECT_LT(0, response.size());
    EXPECT_EQ(response.substr(0, 15), std::string("HTTP/1.1 200 OK"));
}

TEST(tcp_client, timeout)
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
TEST(tcp_client, https_response)
{
    tcp_client tcp("twitter.com", "443");
    std::string response = tcp.send_message("GET / HTTP/1.1\r\n"
            "Host: twitter.com\r\n"
            "Scheme: https\r\n"
            "Connection: close\r\n\r\n");

    EXPECT_LT(0, response.size());
    EXPECT_EQ(response.substr(0, 15), std::string("HTTP/1.1 200 OK"));
}
