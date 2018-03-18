//
// Created by xiaofeng on 1/3/18.
//

#include <ctime>
#include <sstream>
#include "CommentElement.h"
namespace nifeng
{
    BaseElement::BaseElement(const std::string& _name,
                const std::string& _website,
                const std::string&  _content):
    name(_name),
    website(_website),
    content(_content)
    {

    }

    BaseElement::~BaseElement()
    {

    }

    ShowElement::ShowElement(int _commit_id,
                             const std::string& _name,
                            const std::string& _website,
                            const std::string& _email,
                            const std::string&  _content,
                            const std::string&  _reply_to,
                             int _time):
            commit_id(_commit_id),
            BaseElement(_name, _website, _content),
            email(_email),
            reply_to(_reply_to),
            time(_time)
    {
    }
    int ShowElement::mem_size()
    {
        return sizeof(ShowElement) + name.size() + website.size() + content.size() + reply_to.size();
    }
    ShowElement::~ShowElement()
    {

    }
    InputElement::InputElement(const std::string& _name,
                 const std::string& _website,
                 const std::string&  _content,
                 const std::string&  _email,
                 int _reply_to_id,
                 int _parent_id):
            BaseElement(_name, _website, _content),
            email(_email),
            reply_to_id(_reply_to_id),
            parent_id(_parent_id)
    {

    }
    InputElement::~InputElement()
    {

    }
    StorageElement::StorageElement(const std::string& _name,
                   const std::string& _website,
                   const std::string&  _content,
                   const std::string&  _email,
                   int         _group_id,
                   int         _reply_to_id,
                   int         _parent_id,
                   int         _time,
                   int         _comment_id):
            BaseElement(_name, _website, _content),
            email(_email),
            group_id(_group_id),
            reply_to_id(_reply_to_id),
            parent_id(_parent_id),
            time(_time),
            comment_id(_comment_id)
    {

    }
    StorageElement::~StorageElement()
    {

    }
}