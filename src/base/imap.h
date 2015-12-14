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
