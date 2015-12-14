#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#include <openssl/ssl.h>

#include <string>

class tcp_client
{
    public:
        tcp_client(std::string address, std::string port);
        ~tcp_client();

        std::string send_message(const std::string &msg) const;

        tcp_client(const tcp_client &) = delete;
        tcp_client &operator=(const tcp_client &) = delete;

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
};

#endif
