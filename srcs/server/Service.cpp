//
// Created by xiaofeng on 1/10/18.
//

#include "Service.h"
#include <jsoncpp/json/json.h>
#include <sstream>
#include "TestCaseLoader.h"
namespace nifeng
{
    Service::Service():
            _http_server(NULL),
            _dbm(NULL)
    {

    }
    Service::~Service()
    {
        if(_http_server)
            delete _http_server;
        if(_dbm)
            delete _dbm;
    }
    bool Service::run()
    {
        HttpServer * p_http_server;
        CommentsDataManager* p_dbm;
        if(!_http_server)
        {
            p_http_server = new HttpServer(Config::db_port);
            p_http_server->setDelegate(this);

            if(!p_http_server->start())
            {
                LOG_FATAL("start http server failed");
                delete p_http_server;
                return false;
            }
            LOG_TRACE("Http service url :http://localhost:%d%s", Config::db_port, Config::service_uri.c_str());
            _http_server = p_http_server;
        }

        if(!_dbm)
        {
            p_dbm = new CommentsDataManager;
//            //for test
//            p_dbm->dropAllData(Config::db_path);
//            //end test
            if(!p_dbm->init(Config::db_path, Config::db_cache_size))
            {
                LOG_FATAL("start db manager failed");
                delete p_dbm;
                return false;
            }
            _dbm = p_dbm;
        }
//        //for test
//        TestCaseLoader test_loader;
//        std::vector<InputElement> comments;
//        std::string test_uri = "/hello_world";
//
//
//        if (test_loader.loadInputElements("../test_data.txt", comments))
//        {
//            for(int i = 0 ; i < comments.size(); ++i)
//            {
//                if (SUCCEED != _dbm->insertComment(test_uri, comments[i]))
//                {
//                    LOG_TRACE("insert db failed");
//                    exit(1);
//                }
//            }
//        }
//        //end test
        return true;
    }

    bool Service::_getInput(const std::string& in, InputElement& out_e , std::string& out_key)
    {
        bool ret = false;
        Json::Reader reader;
        Json::Value root;
        ret = reader.parse(in, root);

        if(ret)
        {
            if (root.isMember("key") && root["key"].isString())
            {
                out_key = root["key"].asString();
                if(out_key.size() <= 0)
                {
                    return ret;
                }
            }
            if (root.isMember("name") && root["name"].isString())
            {
                out_e.name = root["name"].asString();
            }

            if (root.isMember("email") && root["email"].isString())
            {
                out_e.email = root["email"].asString();
            }

            if (root.isMember("website") && root["website"].isString())
            {
                out_e.website = root["website"].asString();
            }

            if (root.isMember("content") && root["content"].isString())
            {
                out_e.content = root["content"].asString();
                if(out_e.content.size() < 1)
                {
                    return ret;
                }
            }

            if (root.isMember("reply_to_id") && root["reply_to_id"].isInt())
            {
                out_e.reply_to_id = root["reply_to_id"].asInt();
                if (out_e.reply_to_id <= 0)
                {
                    out_e.reply_to_id = 0;
                }
            }

            if (root.isMember("floor_id") && root["floor_id"].isInt())
            {
                out_e.parent_id = root["floor_id"].asInt();
                if (out_e.parent_id <= 0)
                {
                    out_e.parent_id = 0;
                }
            }
            ret = true;
        }
        return ret;
    }
    int Service::join()
    {
        if(_http_server)
        {
            return _http_server->join();
        }
        return -1;
    }

    std::string* Service::onReceivePostData(const std::string& uri, std::map<std::string, std::string>& header, void* pBody, unsigned int bodySize)
    {
        Json::FastWriter writer;
        Json::Value out_root;
        _json_string.clear();
        std::string key;
        InputElement input_e;
        if(uri != Config::service_uri || pBody == NULL || bodySize <= 0)
        {
            return NULL;
        }

        if(!_getInput(std::string((char*)pBody, bodySize), input_e, key))
        {
            return NULL;
        }
        do
        {
            if(input_e.name.size() > 0)
            {
                if(input_e.name.find(' ') != std::string::npos || input_e.name.find('\t') != std::string::npos)
                {
                    out_root["code"] = ERR_NAME;
                    out_root["msg"] = err_str[ERR_NAME - err_str_offset];
                    break;
                }
            }

            if(Config::owner_show_name.size() > 0 && Config::owner_show_name == input_e.name)
            {
                out_root["code"] = NAME_IS_OCCUPIED;
                out_root["msg"] = err_str[NAME_IS_OCCUPIED - err_str_offset];;
                break;
            }

            ErrCode rc = _dbm->insertComment(key, input_e);
            if(SUCCEED == rc)
            {
                out_root["code"] = 0;
                out_root["msg"] = "succeed";
            }
            else
            {
                out_root["code"] = (int)rc;
                out_root["msg"] = err_str[(int)rc - err_str_offset];
            }
        }
        while(false);

        _json_string = writer.write(out_root);
        return &_json_string;
    }
    std::string* Service::onReceiveGetData(const std::string& uri, std::map<std::string, std::string>& header, std::map<std::string, std::string>& queryStrings, bool& out_handled)
    {
        int page = 0;
        int max_page = 0;
        int start_floor = 1;
        Json::StyledWriter writer;
        Json::Value out_root;
        _json_string.clear();
        out_handled = true;
        if(uri != Config::service_uri)
        {
            out_handled = false;
            return NULL;
        }

        if(queryStrings["key"].size() <= 0)
        {
            return NULL;
        }
        if(queryStrings["page"].size() > 0)
        {
            page = atoi(queryStrings["page"].c_str());
            if(page < 0 )
                page = 0;
        }
        CommentsData data;
        ErrCode rc = _dbm->getComments(queryStrings["key"], page, data, max_page);

        if(SUCCEED == rc)
        {
            out_root["code"] = 0;
            out_root["msg"] = "succeed";
            out_root["page"] = page;
            out_root["max_page"] = max_page;
            out_root["comments_count_per_page"] = Config::max_comments_per_page;
            std::list<CommentsData::FloorData*>& comments = data.getComments();

            for(std::list<CommentsData::FloorData*>::iterator it = comments.begin(); it != comments.end(); ++ it)
            {
                if(*it)
                {
                    Json::Value floor_comments;
                    for(CommentsData::FloorData::iterator jit = (*it)->begin(); jit != (*it) -> end(); ++ jit)
                    {
                        Json::Value one_comment;
                        if(Config::owner_name.size() > 0 && Config::owner_email.size() > 0 &&
                                Config::owner_name == jit->name && Config::owner_email == jit->email)
                        {
                            one_comment["name"] = Config::owner_show_name;
                            one_comment["is_owner"] = true;
                        }
                        else
                        {
                            if(jit->name.empty())
                            {
                                one_comment["name"] = "$-";
                            }
                            else
                            {
                                one_comment["name"] = jit->name;
                            }

                            one_comment["is_owner"] = false;
                        }
                        one_comment["comment_id"] = jit->commit_id;
                        one_comment["website"] = jit->website;
                        one_comment["content"] = jit->content;
                        one_comment["reply_to"] = jit->reply_to;
                        one_comment["time"] = jit->time;
                        floor_comments.append(one_comment);
                    }
                    out_root["comments"].append(floor_comments);
                }
            }
            if(Config::owner_avatar.size() > 0)
            {
                //std::stringstream format_stream;
                //format_stream << "http://192.168.1.102:" <<Config::db_port << "/" << Config::owner_avatar;
                out_root["owner_avatar"] = Config::owner_avatar;
            }
        }
        else
        {
            if(rc == ERR_NO_COMMENTS)
            {
                out_root["code"] = 0;
                out_root["msg"] = "succeed";
                out_root["page"] = page;
                out_root["max_page"] = 0;
                out_root["comments_count_per_page"] = Config::max_comments_per_page;
            }
            else
            {
                out_root["code"] = (int)rc;
                out_root["msg"] = err_str[(int)rc - err_str_offset];
            }
        }
        _json_string = writer.write(out_root);
        return &_json_string;
    }
}
