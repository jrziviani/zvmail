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

#include "messages.h"
#include <iostream>
#include <stack>
#include <sstream>

using std::string;
using std::weak_ptr;
using std::unique_ptr;
using std::vector;
using std::wstringstream;

string contact_t::jsonfy() const
{
    std::stringstream json;
    json << "{\"_name\"  : \"" << _name  << "\",";
    json << " \"_email\" : \"" << _email << "\"}";

    return json.str();
}

string message_t::jsonfy()
{
    std::stringstream json;
    json << "{";
    json << "\"message\"   : {";
    json << "\"_id\"       : \"" << _id        << "\",";
    json << "\"_subject\"  : \"" << _subject   << "\",";
    json << "\"_body\"     : \"" << _body      << "\",";
    json << "\"_header\"   : \"" << _header    << "\",";
    json << "\"_timestamp\": \"" << _timestamp << "\",";
    json << "\"_is_read\"  : \"" << _is_read   << "\",";
    json << "\"_is_recent\": \"" << _is_recent << "\",";
    json << "\"_from\"     : [";
    for (contacts_t::const_iterator it = _from.begin();
                                    it != _from.end();
                                    it++) {
        json << (it)->jsonfy();
        json << ((it + 1 == _from.end()) ? "" : ",");
    }
    json << "],";
    json << "\"_to\"     : [";
    for (contacts_t::const_iterator it = _to.begin();
                                    it != _to.end();
                                    it++) {
        json << (it)->jsonfy();
        json << ((it + 1 == _to.end()) ? "" : ",");
    }
    json << "],";
    json << "\"_cc\"     : [";
    for (contacts_t::const_iterator it = _cc.begin();
                                    it != _cc.end();
                                    it++) {
        json << (it)->jsonfy();
        json << ((it + 1 == _cc.end()) ? "" : ",");
    }
    json << "],";
    json << "\"_bcc\"     : [";
    for (contacts_t::const_iterator it = _bcc.begin();
                                    it != _bcc.end();
                                    it++) {
        json << (it)->jsonfy();
        json << ((it + 1 == _bcc.end()) ? "" : ",");
    }
    json << "]";
    json << "}}";

    return json.str();
}

message_t::message_t()
{

}

message_t::~message_t()
{

}

bool message_t::is_read() const
{
    return _is_read;
}

void message_t::is_read(bool read)
{
    _is_read = read;
}

bool message_t::is_recent() const
{
    return _is_recent;
}

void message_t::is_recent(bool recent)
{
    _is_recent = recent;
}

unsigned int message_t::id() const
{
    return _id;
}

void message_t::id(unsigned int id)
{
    _id = id;
}

string message_t::subject() const
{
    return _subject;
}

void message_t::subject(const string &subject)
{
    _subject = subject;
}

string message_t::body() const
{
    return _body;
}

void message_t::body(const string &body)
{
    _body = body;
}

string message_t::header() const
{
    return _header;
}

void message_t::header(const string &header)
{
    _header = header;
}

string message_t::timestamp() const
{
    return _timestamp;
}

void message_t::timestamp(const string &timestamp)
{
    _timestamp = timestamp;
}

contacts_t message_t::from() const
{
    return _from;
}

void message_t::from(const contacts_t &from)
{
    _from = from;
}

contacts_t message_t::to() const
{
    return _to;
}

void message_t::to(const contacts_t &to)
{
    _to = to;
}

contacts_t message_t::cc() const
{
    return _cc;
}

void message_t::cc(const contacts_t &cc)
{
    _cc = cc;
}

contacts_t message_t::bcc() const
{
    return _bcc;
}

void message_t::bcc(const contacts_t &bcc)
{
    _bcc = bcc;
}

void message_t::add_weak_ptr_to_folder(weak_ptr<folder_t> folder)
{
    _folders.emplace_front(folder);
}

void message_t::foreach_folder(const callbacks_t &cb_list) const
{
    for (auto &folder : _folders) {

        auto folder_ptr = folder.lock();
        if (!folder_ptr) {
            //TODO: log it
            continue;
        }

        for (auto &callback : cb_list)
            callback(folder);
    }
}

using namespace std;

int main()
{
    shared_ptr<folder_t> root(new folder_t);

    shared_ptr<folder_t> f1(new folder_t);
    f1->_name = "Folder 1";
    f1->_parent = root;
    root->_children.emplace_front(f1);

    shared_ptr<folder_t> f2(new folder_t);
    f2->_name = "Folder 2";
    f2->_parent = root;
    root->_children.emplace_front(f2);

    shared_ptr<folder_t> f2a(new folder_t);
    f2a->_name = "Folder 2a";
    f2a->_parent = f2;
    f2->_children.emplace_front(f2a);

    shared_ptr<folder_t> f2aa(new folder_t);
    f2aa->_name = "Folder 2aa";
    f2aa->_parent = f2a;
    f2a->_children.emplace_front(f2aa);

    shared_ptr<folder_t> f2b(new folder_t);
    f2b->_name = "Folder 2b";
    f2b->_parent = f2;
    f2->_children.emplace_front(f2b);

    messages_t messages;
    messages.reserve(20);

    // loop cada pasta, mandando query pro server sobre
    // msgs disponiveis na tal pasta
    shared_ptr<message_t> msg1(new message_t);
    msg1->id(0);
    msg1->subject("Message 1");
    contacts_t from = {contact_t("Zivi", "jrziviani@gmail.com"),
                       contact_t("Zéça", "ze@yahoo.com.br")};
    msg1->from(from);
    messages.emplace_back(msg1);
    f1->_messages.push_back(messages.back());
    f2a->_messages.push_back(messages.back());
    messages.back()->add_weak_ptr_to_folder(f1);
    messages.back()->add_weak_ptr_to_folder(f2a);

    shared_ptr<message_t> msg2(new message_t);
    msg2->id(1);
    msg2->subject("Message 2");
    messages.emplace_back(msg2);
    f2a->_messages.push_back(messages.back());
    messages.back()->add_weak_ptr_to_folder(f2a);

    shared_ptr<message_t> msg3(new message_t);
    msg3->id(2);
    msg3->subject("Message 3");
    messages.emplace_back(msg3);
    f2b->_messages.push_back(messages.back());
    f2aa->_messages.push_back(messages.back());
    messages.back()->add_weak_ptr_to_folder(f2b);
    messages.back()->add_weak_ptr_to_folder(f2aa);

    shared_ptr<message_t> msg4(new message_t);
    msg4->id(3);
    msg4->subject("Message 4");
    messages.emplace_back(msg4);
    f1->_messages.push_back(messages.back());
    f2aa->_messages.push_back(messages.back());
    messages.back()->add_weak_ptr_to_folder(f1);
    messages.back()->add_weak_ptr_to_folder(f2aa);

    shared_ptr<message_t> msg5(new message_t);
    msg5->id(4);
    msg5->subject("Message 5");
    messages.emplace_back(msg5);
    f1->_messages.push_back(messages.back());
    messages.back()->add_weak_ptr_to_folder(f1);

    cout << "test --------\n";
    cout << &f2b->_messages[0] << endl;
    cout << &messages[0] << endl;
    cout << &messages[1] << endl;
    cout << &messages[2] << endl;
    cout << &messages[3] << endl;
    cout << &messages[4] << endl;
    //f1->_messages[0].get()._subject = "novo...";
    cout << "test --------\n";

    for (auto &msg : messages) {
        cout << &msg << " - " << msg->id() << endl;
        cout << msg->subject() << endl;

        auto cb = [](weak_ptr<folder_t> folder) {
            auto folder_ptr = folder.lock();
            cout << folder_ptr << ": " << folder_ptr->_name;
            cout << "\n";
        };

        callbacks_t cbs;
        cbs.emplace_back(cb);

        msg->foreach_folder(cbs);

    }
    cout << "------------\n";


    stack<shared_ptr<folder_t>> tmp;

    auto it = root->_children.cbegin();
    for ( ; it != root->_children.cend(); ++it) {
        tmp.emplace(*it);

        while (!tmp.empty()) {

            auto top = tmp.top();
            tmp.pop();

            cout << top->_name << endl;
            for (auto &msg : top->_messages) {
                cout << " -> " << msg->subject() << endl;
                cout << msg->jsonfy() << endl;
            }

            auto child_it = top->_children.cbegin();
            for ( ; child_it != top->_children.cend(); ++child_it) {
                tmp.emplace(*child_it);
            }

        }

    }
    return 0;
}
