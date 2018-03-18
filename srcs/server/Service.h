//
// Created by xiaofeng on 1/10/18.
//

#ifndef SQLITE3TEST_SERVICE_H
#define SQLITE3TEST_SERVICE_H
#include "HttpServer.h"
#include "IHttpServerDelegate.h"
#include "CommentsDataManager.h"

namespace nifeng
{
    class Service : public IHttpServerDelegate
    {
    public:
        Service();
        ~Service();
        bool run();
        int join();
        std::string* onReceivePostData(const std::string& uri, std::map<std::string, std::string>& header, void* pBody, unsigned int bodySize);
        std::string* onReceiveGetData(const std::string& uri, std::map<std::string, std::string>& header, std::map<std::string, std::string>& queryStrings, bool& out_handled);
    private:
        bool _getInput(const std::string& in, InputElement& out_e , std::string& out_key);
    private:
        HttpServer*          _http_server;
        CommentsDataManager* _dbm;
        std::string          _json_string;
    };
}




#endif //SQLITE3TEST_SERVICE_H
