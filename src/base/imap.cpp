#include "imap.h"

#include <iostream>
#include <stdexcept>

using namespace std;

imap::imap(const tcp_client &tcp) :
    _tcpclient(tcp)
{
    init_imap();
    get_folders();
}

imap::~imap()
{
}

imap::folder_list imap::get_folders() const
{
    imap::folder_list ret;

    cout << _tcpclient.send_message("a2 SELECT INBOX\r\n") << endl;

    return ret;
}

string imap::get_header_by_folder(std::string &folder) const
{
    string ret;

    return ret;
}

void imap::init_imap()
{
    string result = _tcpclient.send_message("a1 login jrziviani@gmail.com PASS\r\n");
    cout << result << endl;

}

void imap::finish_imap()
{

}

int main()
{
    try {
        tcp_client tcp("imap.gmail.com", "993");
        imap myimap(tcp);
    }
    catch(runtime_error &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
