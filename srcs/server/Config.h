//
// Created by xiaofeng on 1/3/18.
//

#ifndef SQLITE3TEST_CONFIG_H
#define SQLITE3TEST_CONFIG_H
#include <string>
#include "Logger.h"

namespace nifeng
{
    class Config
    {
    public:
        static bool loadFromFile(const std::string& config_file_path);
    public:
        static int          db_cache_size; // in Byte
        static int          db_port;
        static bool         enable_pagination;
        static int          max_comments_per_page;
        static int          max_comment_char_num;
        static std::string  service_uri;
        static std::string  db_path;
        static std::string  owner_name;
        static std::string  owner_email;
        static std::string  owner_avatar;
        static std::string  owner_show_name;
        static std::string  allow_origin;
    };
}


#endif //SQLITE3TEST_CONFIG_H
