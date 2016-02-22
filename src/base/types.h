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

#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <string>
#include <vector>
#include <list>
#include <forward_list>
#include <memory>
#include <functional>


// serialable definition ------------------------------------------------------
class serialazable
{
    public:
        virtual ~serialazable();

        virtual std::wstring jsonfy() const = 0;
        void save();
        void load();
        void flush();
};
// ----------------------------------------------------------------------------


// contact definition ---------------------------------------------------------
struct contact_t
{
    std::wstring _name;
    std::wstring _email;

    contact_t();
    contact_t(std::wstring name, std::wstring email);

    std::wstring jsonfy() const;
};
using contacts_t = std::vector<contact_t>;
// ----------------------------------------------------------------------------


// folder definition ----------------------------------------------------------
class folder_t;
using folders_t = std::forward_list<std::weak_ptr<folder_t>>;
using folder_callback_t  = std::function<void(std::weak_ptr<folder_t>)>;
using folder_callbacks_t = std::vector<folder_callback_t>;
// ----------------------------------------------------------------------------


// message definition ---------------------------------------------------------
class message_t
{
    public:
        message_t();
        ~message_t();

        bool is_read() const;
        void is_read(bool read);

        bool is_recent() const;
        void is_recent(bool recent);

        unsigned long int id() const;
        void id(unsigned long int id);

        std::wstring subject() const;
        void subject(const std::wstring &subject);

        std::wstring body() const;
        void body(const std::wstring &body);

        std::wstring header() const;
        void header(const std::wstring &header);

        std::wstring timestamp() const;
        void timestamp(const std::wstring &timestamp);

        contact_t from() const;
        void from(const contact_t &from);

        contacts_t to() const;
        void to(const contacts_t &to);

        contacts_t cc() const;
        void cc(const contacts_t &cc);

        contacts_t bcc() const;
        void bcc(const contacts_t &bcc);

        void add_weak_ptr_to_folder(std::weak_ptr<folder_t> folder);
        void foreach_folder(const folder_callbacks_t &cb_list) const;

        std::wstring jsonfy();

    private:
        bool _is_read;
        bool _is_recent;
        unsigned long int _id;
        std::wstring _subject;
        std::wstring _body;
        std::wstring _header;
        std::wstring _timestamp;
        contact_t _from;
        contacts_t _to;
        contacts_t _cc;
        contacts_t _bcc;
        folders_t _folders;
};
using messages_t = std::vector<std::shared_ptr<message_t>>;
using messages_weak_t = std::vector<std::weak_ptr<message_t>>;
using message_callback_t  = std::function<void(std::weak_ptr<message_t>)>;
using message_callbacks_t = std::vector<message_callback_t>;
// ----------------------------------------------------------------------------


// folder definition ----------------------------------------------------------
class folder_t
{
    public:

    public:
        unsigned long int id() const;
        void id(unsigned long int id);

        std::wstring name() const;
        void name(const std::wstring &name);

        std::shared_ptr<folder_t> parent() const;
        void parent(std::weak_ptr<folder_t> folder);

        void add_children(std::weak_ptr<folder_t> folder);
        void foreach_children(const folder_callbacks_t &cb_list) const;
        void foreach_children_rec(const folder_callbacks_t &cb_list) const;

        void add_message(std::weak_ptr<message_t> message);
        messages_weak_t &messages();
        void foreach_message(const message_callbacks_t &cb_list) const;

        std::wstring jsonfy();

    private:
        unsigned long int _id;
        std::wstring _name;
        std::weak_ptr<folder_t> _parent;
        folders_t _children;
        messages_weak_t _messages;
};
// ----------------------------------------------------------------------------


#endif
