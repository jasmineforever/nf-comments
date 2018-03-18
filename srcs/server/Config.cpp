//
// Created by xiaofeng on 1/3/18.
//

#include "Config.h"
#include "jsoncpp/json/json.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "base64.hpp"

namespace nifeng
{
    int Config::db_cache_size = 50 * 1024 * 1024;
    int          Config::db_port = 5000;
    std::string  Config::service_uri = "/comments";
    std::string  Config::db_path = "./comments.db";
    bool         Config::enable_pagination = false;
    int          Config::max_comments_per_page = 10;
    int          Config::max_comment_char_num = 255;
    std::string  Config::owner_name = "";
    std::string  Config::owner_email = "";
    std::string  Config::owner_avatar = "";
    std::string  Config::owner_show_name = "";
    std::string  Config::allow_origin = "";


    bool Config::loadFromFile(const std::string& config_file_path)
    {
        bool ret = false;
        Json::Value root;
        Json::Reader j_reader;
        char* buf = NULL;
        FILE* fp = fopen(config_file_path.c_str(), "r");

        do
        {
            if(!fp)
            {
                LOG_FATAL("can't open config file:%s", config_file_path.c_str());
                break;
            }
            fseek(fp, 0, SEEK_END);
            int file_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            if(file_size <= 0)
                break;
            buf = new char[file_size + 1];
            buf[file_size] = '\0';

            if(file_size != fread(buf, 1, file_size, fp))
            {
                LOG_FATAL("load config file failed.");
                break;
            }

            if(!j_reader.parse(buf, root))
            {
                LOG_FATAL("json parse err in load config file");
                break;
            }

            if(root.isMember("service_setting") && root["service_setting"].isObject())
            {
                Json::Value& server_setting = root["service_setting"];

                if(server_setting.isMember("db_path") && server_setting["db_path"].isString())
                {
                    Config::db_path = server_setting["db_path"].asString();
                }
                else
                {
                    LOG_WARNING("can't find db_path in configure file using default:%s", Config::db_path);
                }
                int port = -1;
                if(server_setting.isMember("db_service_port") && server_setting["db_service_port"].isInt())
                {
                    port  = atoi(server_setting["db_service_port"].asString().c_str());
                }
                if(port < 2)
                {
                    LOG_WARNING("db_service_port configure err, using default:%d", Config::db_port);
                }
                else
                {
                    Config::db_port = port;
                }
                if(server_setting.isMember("service_uri") && server_setting["service_uri"].isString())
                {
                    Config::service_uri = server_setting["service_uri"].asString();
                }
                else
                {
                    LOG_WARNING("service_uri configure err, using default:%d", Config::service_uri);
                }

                if(server_setting.isMember("db_cache_size") && server_setting["db_cache_size"].isInt())
                {
                    int cache_size = server_setting["db_cache_size"].asInt();

                    if(cache_size < 0)
                    {
                        cache_size = 0;
                    }
                    else if(cache_size > 2000)
                    {
                        cache_size = 2000 * 1024 * 1024;
                        LOG_WARNING("db_cache_size max is 2000MB");
                    }
                    else
                    {
                        cache_size = cache_size * 1024 * 1024;
                    }
                    Config::db_cache_size = cache_size;
                }
                else
                {
                    LOG_WARNING("db_cache_size configure err, using default:%d MB", Config::db_cache_size/ 1024 / 1024);
                }
                if(server_setting.isMember("allow_origin") && server_setting["allow_origin"].isString())
                {
                    Config::allow_origin = server_setting["allow_origin"].asString();
                }
            }
            if(root.isMember("comments_setting") && root["comments_setting"].isObject())
            {
                Json::Value &comments_setting = root["comments_setting"];

                if(comments_setting.isMember("enable_pagination") && comments_setting["enable_pagination"].isBool())
                {
                    Config::enable_pagination = comments_setting["enable_pagination"].asBool();
                }
                else
                {
                    LOG_WARNING("enable_pagination configure err, using default:false");
                }
                int max_comments = -1;
                if(comments_setting.isMember("max_comments_per_page") && comments_setting["max_comments_per_page"].isInt())
                {
                    max_comments = comments_setting["max_comments_per_page"].asInt();
                }
                if(max_comments <= 0)
                {
                    LOG_WARNING("max_comments_per_page configure err, using default:%d", Config::max_comments_per_page);
                }
                else
                {
                    Config::max_comments_per_page = max_comments;
                }

                int max_char_num = -1;
                if(comments_setting.isMember("max_comment_char_num") && comments_setting["max_comment_char_num"].isInt())
                {
                    max_char_num = comments_setting["max_comment_char_num"].asInt();
                }
                if(max_comments <= 0)
                {
                    LOG_WARNING("max_comment_char_num configure err, using default:%d", Config::max_comment_char_num);
                }
                else
                {
                    Config::max_comment_char_num = max_char_num;
                }
            }
            if(root.isMember("blog_setting") && root["blog_setting"].isObject())
            {
                Json::Value &blog_setting = root["blog_setting"];

                if(blog_setting.isMember("owner_name") && blog_setting["owner_name"].isString())
                {
                    Config::owner_name = blog_setting["owner_name"].asString();
                }
                if(blog_setting.isMember("owner_email") && blog_setting["owner_email"].isString())
                {
                    Config::owner_email = blog_setting["owner_email"].asString();
                }
                if(blog_setting.isMember("owner_avatar") && blog_setting["owner_avatar"].isString())
                {
                    Config::owner_avatar = blog_setting["owner_avatar"].asString();
                }
                if(blog_setting.isMember("owner_show_name") && blog_setting["owner_show_name"].isString())
                {
                    Config::owner_show_name = blog_setting["owner_show_name"].asString();
                }
            }
            ret = true;
        }
        while(false);

        if(fp)
        {
            fclose(fp);
        }
        if(buf)
        {
            delete [] buf;
        }
        return ret;
    }
}