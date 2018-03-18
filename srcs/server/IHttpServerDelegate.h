//
// Created by xiaofeng on 6/21/17.
//

#ifndef PROJECT_IHTTPSERVERDELEGATE_H
#define PROJECT_IHTTPSERVERDELEGATE_H
#include <string>
#include <map>
namespace nifeng
{
    class IHttpServerDelegate
    {
    public:
        virtual ~IHttpServerDelegate(){}
        virtual std::string* onReceivePostData(const std::string& uri, std::map<std::string, std::string>& header, void* pBody, unsigned int bodySize) = 0;
        virtual std::string* onReceiveGetData(const std::string& uri, std::map<std::string, std::string>& header, std::map<std::string, std::string>& queryStrings, bool& out_handled) = 0;
        //TODO other method
    protected:
        IHttpServerDelegate(){}
    };
}
#endif //PROJECT_IHTTPSERVERDELEGATE_H
