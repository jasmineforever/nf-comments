//
// Created by xiaofeng on 1/3/18.
//

#ifndef SQLITE3TEST_COMMENTELEMENT_H
#define SQLITE3TEST_COMMENTELEMENT_H

#include <string>

namespace nifeng
{
    class BaseElement
    {
    public:
        BaseElement(const std::string& _name = "",
                    const std::string& _website = "",
                    const std::string&  _content = "");
        ~BaseElement();
    public:
        std::string name;
        std::string website;
        std::string content;
    };
    class ShowElement : public BaseElement
    {
    public:
        ShowElement(int _commit_id,
                    const std::string& _name = "",
                    const std::string& _website = "",
                    const std::string& _email = "",
                    const std::string&  _content = "",
                    const std::string&  _reply_to = "",
                    int _time = 0);
        ~ShowElement();
        int mem_size();
    public:
        std::string email;
        std::string reply_to;
        int time;
        int commit_id;
    };
    class InputElement : public BaseElement
    {
    public:
        InputElement(const std::string& _name = "",
                     const std::string& _website = "",
                     const std::string&  _content = "",
                     const std::string&  _email = "",
                     int _reply_to_id = 0,
                     int _parent_id = 0
        );
        ~InputElement();
    public:
        std::string email;
        int         reply_to_id;
        int         parent_id;
    };
    class StorageElement : public BaseElement
    {
    public:
        StorageElement(const std::string& _name,
                       const std::string& _website,
                       const std::string&  _content,
                       const std::string&  _email,
                        int         _group_id,
                        int         _reply_to_id,
                        int         _parent_id,
                        int         _time,
                        int         _comment_id = 0);
        ~StorageElement();
    public:
        std::string email;
        int         group_id;
        int         reply_to_id;
        int         time;
        int         comment_id;
        int         parent_id;
    };
}



#endif //SQLITE3TEST_COMMENTELEMENT_H
