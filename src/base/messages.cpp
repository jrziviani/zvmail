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
#include <iterator>

using std::string;
using std::weak_ptr;
using std::unique_ptr;
using std::vector;
using std::stringstream;
using std::stack;
using std::next;


// contact implementation -----------------------------------------------------
contact_t::contact_t(string name, string email) :
    _name(name), _email(email)
{

}

string contact_t::jsonfy() const
{
    stringstream json;
    json << "{\"_name\"  : \"" << _name  << "\",";
    json << " \"_email\" : \"" << _email << "\"}";

    return json.str();
}
// ----------------------------------------------------------------------------


// message implementation -----------------------------------------------------
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

unsigned long int message_t::id() const
{
    return _id;
}

void message_t::id(unsigned long int id)
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

void message_t::foreach_folder(const folder_callbacks_t &cb_list) const
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

string message_t::jsonfy()
{
    stringstream json;
    json << _id <<        ": {";
    json << "\"_id\"       : \"" << _id        << "\",";
    json << "\"_subject\"  : \"" << _subject   << "\",";
    json << "\"_body\"     : \"" << _body      << "\",";
    json << "\"_header\"   : \"" << _header    << "\",";
    json << "\"_timestamp\": \"" << _timestamp << "\",";
    json << "\"_is_read\"  : \"" << _is_read   << "\",";
    json << "\"_is_recent\": \"" << _is_recent << "\",";
    json << "\"_from\"     : [";
    for (contacts_t::const_iterator it = _from.cbegin();
                                    it != _from.cend();
                                    it++) {
        json << it->jsonfy();
        json << ((it + 1 == _from.cend()) ? "" : ",");
    }
    json << "],";
    json << "\"_to\"     : [";
    for (contacts_t::const_iterator it = _to.cbegin();
                                    it != _to.cend();
                                    it++) {
        json << it->jsonfy();
        json << ((it + 1 == _to.end()) ? "" : ",");
    }
    json << "],";
    json << "\"_cc\"     : [";
    for (contacts_t::const_iterator it = _cc.cbegin();
                                    it != _cc.cend();
                                    it++) {
        json << it->jsonfy();
        json << ((it + 1 == _cc.cend()) ? "" : ",");
    }
    json << "],";
    json << "\"_bcc\"     : [";
    for (contacts_t::const_iterator it = _bcc.cbegin();
                                    it != _bcc.cend();
                                    it++) {
        json << it->jsonfy();
        json << ((it + 1 == _bcc.cend()) ? "" : ",");
    }
    json << "],";
    json << "\"_folders\": [";
    for (folders_t::const_iterator it = _folders.cbegin();
                                   it != _folders.cend();
                                   it++) {
        const auto folder_ptr = it->lock();
        json << folder_ptr->id();
        json << ((next(it) == _folders.cend()) ? "" : ",");
    }
    json << "]}";

    return json.str();
}
// ----------------------------------------------------------------------------


// folder implementation ------------------------------------------------------
unsigned long int folder_t::id() const
{
    return _id;
}

void folder_t::id(unsigned long int id)
{
    _id = id;
}

string folder_t::name() const
{
    return _name;
}

void folder_t::name(const string &name)
{
    _name = name;
}

std::shared_ptr<folder_t> folder_t::parent() const
{
    return _parent.lock();
}

void folder_t::parent(std::weak_ptr<folder_t> folder)
{
    _parent = folder;
}

void folder_t::add_children(std::weak_ptr<folder_t> folder)
{
    _children.emplace_front(folder);
}

void folder_t::foreach_children(const folder_callbacks_t &cb_list) const
{
    for (auto &folder : _children) {

        auto folder_ptr = folder.lock();
        if (!folder_ptr) {
            //TODO: log it
            continue;
        }

        for (auto &callback : cb_list)
            callback(folder);
    }
}

void folder_t::foreach_children_rec(const folder_callbacks_t &cb_list) const
{
    stack<weak_ptr<folder_t>> tmp;

    for (auto &folder : _children)
        tmp.emplace(folder);

    while (!tmp.empty()) {
        auto top = tmp.top();
        tmp.pop();

        for (auto &callback : cb_list)
            callback(top);

        auto folder = top.lock();
        if (folder == nullptr)
            continue;

        for (auto &child : folder->_children)
            tmp.emplace(child);
    }
}

void folder_t::add_message(std::weak_ptr<message_t> message)
{
    _messages.emplace_back(message);
}

messages_weak_t &folder_t::messages()
{
    return _messages;
}

void folder_t::foreach_message(const message_callbacks_t &cb_list) const
{
    for (auto &message : _messages) {

        auto message_ptr = message.lock();
        if (!message_ptr) {
            //TODO: log it
            continue;
        }

        for (auto &callback : cb_list)
            callback(message);
    }
}

string folder_t::jsonfy()
{
    stringstream json;

    json << _id << ": {";
    json << "\"_name\"       : \"" << _name        << "\",";
    json << "\"_id\"         :   " << _id          << "  ,";
    if (auto parent_ptr = _parent.lock())
        json << "\"_parent\" : " << parent_ptr->_id << " ,";
    else
        json << "\"_parent\" : \"null\""            << " ,";
    json << "\"_children\"   : [";
    for (folders_t::const_iterator it = _children.cbegin();
                                   it != _children.cend();
                                   it++) {
        const auto folder_ptr = it->lock();
        json << folder_ptr->_id;
        json << ((next(it) == _children.cend()) ? "" : ",");
    }
    json << "],";
    json << "\"_messages\"   : [";
    for (messages_weak_t::const_iterator it = _messages.cbegin();
                                         it != _messages.cend();
                                         it++) {
        const auto message_ptr = it->lock();
        json << message_ptr->id();
        json << ((it + 1 == _messages.cend()) ? "" : ",");
    }
    json << "]}";

    return json.str();
}
// ----------------------------------------------------------------------------

using namespace std;

int main()
{
    shared_ptr<folder_t> root(new folder_t);

    shared_ptr<folder_t> f1(new folder_t);
    f1->name("Folder 1");
    f1->id(1);
    f1->parent(root);
    root->add_children(f1);

    shared_ptr<folder_t> f2(new folder_t);
    f2->name("Folder 2");
    f2->id(2);
    f2->parent(root);
    root->add_children(f2);

    shared_ptr<folder_t> f2a(new folder_t);
    f2a->name("Folder 2a");
    f2a->id(3);
    f2a->parent(f2);
    f2->add_children(f2a);

    shared_ptr<folder_t> f2aa(new folder_t);
    f2aa->name("Folder 2aa");
    f2aa->id(4);
    f2aa->parent(f2a);
    f2a->add_children(f2aa);

    shared_ptr<folder_t> f2b(new folder_t);
    f2b->name("Folder 2b");
    f2b->id(5);
    f2b->parent(f2);
    f2->add_children(f2b);

    root->foreach_children({
            [](weak_ptr<folder_t> folder) {
                cout << folder.lock()->name() << endl;
            }
    });
    cout << "xxxxxxxxxxxxxxxxxxxxxx" << endl;
    cout << f1->name() << endl;
    f1->foreach_children({
            [](weak_ptr<folder_t> folder) {
                cout << " -> " << folder.lock()->name() << endl;
            }
    });
    cout << "xxxxxxxxxxxxxxxxxxxxxx" << endl;
    cout << f2->name() << endl;
    f2->foreach_children({
            [](weak_ptr<folder_t> folder) {
                cout << " -> " << folder.lock()->name() << endl;
            }
    });
    cout << "xxxxxxxxxxxxxxxxxxxxxx" << endl;
    cout << f2a->name() << endl;
    f2a->foreach_children({
            [](weak_ptr<folder_t> folder) {
                cout << " -> " << folder.lock()->name() << endl;
            }
    });
    cout << "xxxxxxxxxxxxxxxxxxxxxx" << endl;
    cout << f2aa->name() << endl;
    f2aa->foreach_children({
            [](weak_ptr<folder_t> folder) {
                cout << " -> " << folder.lock()->name() << endl;
            }
    });
    cout << "xxxxxxxxxxxxxxxxxxxxxx" << endl;
    cout << f2b->name() << endl;
    f2b->foreach_children({
            [](weak_ptr<folder_t> folder) {
                cout << " -> " << folder.lock()->name() << endl;
            }
    });
    cout << "xxxxxxxxxxxxxxxxxxxxxx" << endl;

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
    f1->add_message(messages.back());
    f2a->add_message(messages.back());
    messages.back()->add_weak_ptr_to_folder(f1);
    messages.back()->add_weak_ptr_to_folder(f2a);

    shared_ptr<message_t> msg2(new message_t);
    msg2->id(1);
    msg2->subject("Message 2");
    messages.emplace_back(msg2);
    f2a->add_message(messages.back());
    messages.back()->add_weak_ptr_to_folder(f2a);

    shared_ptr<message_t> msg3(new message_t);
    msg3->id(2);
    msg3->subject("Message 3");
    messages.emplace_back(msg3);
    f2b->add_message(messages.back());
    f2aa->add_message(messages.back());
    messages.back()->add_weak_ptr_to_folder(f2b);
    messages.back()->add_weak_ptr_to_folder(f2aa);

    shared_ptr<message_t> msg4(new message_t);
    msg4->id(3);
    msg4->subject("Message 4");
    messages.emplace_back(msg4);
    f1->add_message(messages.back());
    f2aa->add_message(messages.back());
    messages.back()->add_weak_ptr_to_folder(f1);
    messages.back()->add_weak_ptr_to_folder(f2aa);

    shared_ptr<message_t> msg5(new message_t);
    msg5->id(4);
    msg5->subject("Message 5");
    messages.emplace_back(msg5);
    f1->add_message(messages.back());
    messages.back()->add_weak_ptr_to_folder(f1);

    cout << "test --------\n";
    //cout << &f2b->_messages[0] << endl;
    cout << &messages[0] << endl;
    cout << &messages[1] << endl;
    cout << &messages[2] << endl;
    cout << &messages[3] << endl;
    cout << &messages[4] << endl;
    //f1->_messages[0].get()._subject = "novo...";
    cout << "test --------\n";

    for (auto &msg : messages) {
        //cout << &msg << " - " << msg->id() << endl;
        cout << msg->subject() << endl;

        auto cb = [](weak_ptr<folder_t> folder) {
            auto folder_ptr = folder.lock();
            cout << " -> " << folder_ptr << ": " << folder_ptr->name();
            cout << "\n";
        };

        folder_callbacks_t cbs;
        cbs.emplace_back(cb);

        msg->foreach_folder(cbs);

    }
    cout << "------------\n";


    auto print_msg = [](weak_ptr<folder_t> folder)
    {
        auto folder_ptr = folder.lock();
        cout << "[" << folder_ptr->name() << "]\n";
        for (const auto &msg : folder_ptr->messages()) {
            auto msg_ptr = msg.lock();
            cout << " -> " << msg_ptr->subject() << endl;
        }
    };

    root->foreach_children_rec({print_msg});

/*
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
*/
    return 0;
}
