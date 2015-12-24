#include "tcpclient.h"

#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdexcept>
#include <cstring>

#include <iostream>

using namespace std;

const ssize_t BUFFER_SIZE = 1024;
const long TIMEOUT = 3;

tcp_client::tcp_client(string address, string port)
    : _ssl(NULL), _socket(0), _address(address), _port(port), _is_secure(false)
{
    init_socket();
    if (port == "443" || port == "993") {
        _is_secure = true;
        init_ssl_socket();
    }
}

tcp_client::~tcp_client()
{
    if (_is_secure) {
        if (SSL_shutdown(_ssl) == 0)
            SSL_shutdown(_ssl);
        SSL_free(_ssl);
    }
    close(_socket);
}

void tcp_client::init_socket()
{
    struct addrinfo info;
    struct addrinfo *result;
    memset(&info, 0, sizeof(struct addrinfo));
    info.ai_family      = AF_UNSPEC;
    info.ai_socktype   = SOCK_STREAM;
    info.ai_flags      = 0;
    info.ai_protocol   = 0;
    int info_ret = getaddrinfo(_address.c_str(), _port.c_str(), &info, &result);
    if (info_ret != 0) {
        throw runtime_error(gai_strerror(info_ret));
    }

    struct addrinfo *result_it = result;
    for (; result_it != NULL; result_it = result->ai_next) {
        _socket = socket(result_it->ai_family,
                result_it->ai_socktype,
                result_it->ai_protocol);
        if (_socket == -1)
            continue;

        struct timeval timeout = { .tv_sec = TIMEOUT, .tv_usec = 0 };
        if (setsockopt(_socket,
                    SOL_SOCKET,
                    SO_RCVTIMEO,
                    (struct timeval*)&timeout,
                    (socklen_t)sizeof(struct timeval)) == -1) {
            throw runtime_error(strerror(errno));
        }

        if (connect(_socket, result_it->ai_addr, result_it->ai_addrlen) != -1)
            break;

        close(_socket);
        throw runtime_error(strerror(errno));
    }
}

void tcp_client::init_ssl_socket()
{
    SSL_load_error_strings();
    SSL_library_init();

    SSL_CTX *context = SSL_CTX_new(SSLv23_client_method());
    if (context == NULL)
        throw runtime_error(strerror(errno));

    _ssl = SSL_new(context);
    if (_ssl == NULL)
        throw runtime_error(strerror(errno));

    int ret_code;
    ret_code = SSL_set_fd(_ssl, _socket);
    if (ret_code != 1)
        throw runtime_error(strerror(SSL_get_error(_ssl, ret_code)));

    ret_code = SSL_connect(_ssl);
    if (ret_code != 1)
        throw runtime_error(strerror(SSL_get_error(_ssl, ret_code)));

    SSL_CTX_free(context);
}

string tcp_client::send_message(const string &msg) const
{
    return (_is_secure) ?
        send_ssl_message(msg) :
        send_nosecure_message(msg);
}

string tcp_client::send_nosecure_message(const string &msg) const
{
    if (send(_socket, msg.c_str(), msg.size(), 0) <= 0)
        throw runtime_error(strerror(errno));

    char buffer[BUFFER_SIZE] = {};
    string ret;
    ssize_t check_recv;
    while (true) {
        check_recv = recv(_socket, buffer, BUFFER_SIZE, 0);
        if (check_recv == 0)
            break;

        if (check_recv < 0)
            throw runtime_error(strerror(errno));

        if (check_recv <= BUFFER_SIZE)
            ret += buffer;
    }
    return ret;
}

string tcp_client::send_ssl_message(const string &msg) const
{
    int ret_code = SSL_write(_ssl, msg.c_str(), msg.size());
    if (ret_code <= 0)
        throw runtime_error(strerror(SSL_get_error(_ssl, ret_code)));

    char buffer[BUFFER_SIZE];
    string ret;
    int check_read;
    while (true) {
        check_read = SSL_read(_ssl, buffer, BUFFER_SIZE);
        if (check_read == 0)
            break;

        if (check_read < 0)
            throw runtime_error(strerror(SSL_get_error(_ssl, check_read)));

        if (check_read <= BUFFER_SIZE)
            ret += buffer;

        break;
    }

    return ret;
}
/*
int main()
{
    try {
        tcp_client tcp("stackoverflow.com", "443");
        cout << tcp.send_ssl_message("GET / HTTP/1.1\r\nHost: stackoverflow.com\r\nScheme: https\r\nConnection: close\r\n\r\n") << endl;
    }
    catch(runtime_error &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
*/
/*
test case 1:
        tcp_client tcp("stackoverflow.com", "443");
        cout << tcp.send_ssl_message("GET / HTTP/1.1\r\nHost: stackoverflow.com\r\nScheme: https\r\nConnection: close\r\n\r\n") << endl;

-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
-----------------------------------------------------------
test case 2:
*/