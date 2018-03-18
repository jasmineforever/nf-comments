//
// Created by xiaofeng on 6/21/17.
//

#ifndef PROJECT_HTTPSERVER_H
#define PROJECT_HTTPSERVER_H
#include <pthread.h>
#include "IHttpServerDelegate.h"
#include "mongoose.h"

namespace nifeng
{
    class HttpServer
    {
    public:
        HttpServer(int port);
        ~HttpServer();
        void setDelegate(IHttpServerDelegate* delegate);
        bool start();
        int join();
    private:
        static void _httpHandler(struct mg_connection *nc, int ev, void *ev_data);
        static void* _threadAgent(void*);
        void* _serviceHandler();
        void _getHeader(struct http_message * hm);
        void _getQueryStrings(struct http_message * hm);

    private:
        typedef enum{
            HTTP_REQUEST_NULL,
            HTTP_REQUEST_GET,
            HTTP_REQUEST_POST

        } HttpRequestMethod;
        mg_mgr*			        _mgr;
        IHttpServerDelegate*    _delegate;
        int                     _port;
        pthread_t               _httpThread;
        std::map<std::string, std::string>  _headermap;
        std::map<std::string, std::string>  _queryStrings;
        mg_serve_http_opts                  _http_server_opts;
    };
}



#endif //PROJECT_HTTPSERVER_H
