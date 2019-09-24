//
// Created by xiaofeng on 6/21/17.
//

#include "HttpServer.h"
#include "Logger.h"
#include <sstream>
#include "Config.h"
#define MSLEEP(t) usleep((t) * 1000)
namespace nifeng
{
    HttpServer::HttpServer(int port):
        _port(port),
        _delegate(NULL),
        _mgr(NULL),
        _httpThread(-1)
    {
        //memset(&_http_server_opts, 1, sizeof(mg_serve_http_opts));
    }
    HttpServer::~HttpServer()
    {
        if(_mgr)
            mg_mgr_free(_mgr);
    }

    void HttpServer::setDelegate(IHttpServerDelegate* delegate)
    {
        _delegate = delegate;
    }
    void* HttpServer::_threadAgent(void* params)
    {
        HttpServer* pThis = (HttpServer*)params;
        if(pThis)
        {
            return pThis->_serviceHandler();
        }
        return 0;
    }
    bool HttpServer::start()
    {
        if(_port < 2)
        {
            return false;
        }
        if(_mgr)
        {
          return true;
        }
        _mgr = new mg_mgr;
        mg_connection *nc = NULL;
        mg_mgr_init(_mgr, this);
        std::stringstream format_stream;
        format_stream << _port;
        nc = mg_bind(_mgr, format_stream.str().c_str(), _httpHandler);
        if (nc == NULL)
        {
            return false;
        }
        mg_set_protocol_http_websocket(nc);
        //_http_server_opts.document_root = "./web_root";
        //_http_server_opts.enable_directory_listing = "yes";
        if (0 != pthread_create(&_httpThread, NULL, &HttpServer::_threadAgent, this))
        {
            LOG_FATAL("can't create http thread");
            return false;
        }
        return true;
    }
    int HttpServer::join()
    {
        int a = -1;
        if(_httpThread > 0)
            pthread_join(_httpThread, (void**)&a);
        return a;
    }
    void* HttpServer::_serviceHandler()
    {

        MSLEEP(200);
        for (;;)
        {
            mg_mgr_poll(_mgr, 50);
        }
        return 0;
    }
    void HttpServer::_httpHandler(struct mg_connection *nc, int ev, void *ev_data)
    {
        bool isHandled = false;
        std::string* handleRet = NULL;

        HttpServer* pThis = (HttpServer*)nc->mgr->user_data;
        if(!pThis->_delegate)
        {
            return ;
        }
        struct http_message *hm = (struct http_message *) ev_data;

        switch (ev)
        {
            case MG_EV_HTTP_REQUEST:
                if(hm->method.len == 3 && hm->method.p[0] == 'G' && hm->method.p[1] == 'E' && hm->method.p[2] == 'T')
                {
                    pThis->_getHeader(hm);
                    pThis->_getQueryStrings(hm);
                    handleRet = pThis->_delegate->onReceiveGetData(std::string(hm->uri.p, hm->uri.len), pThis->_headermap, pThis->_queryStrings, isHandled);

                    if(!isHandled)
                    {
                        //mg_serve_http(nc, hm, pThis->_http_server_opts);
                        mg_printf(nc, "%s%d%s%s", "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: ", (int)sizeof("Not Found"), "\r\n\r\n", "Not Found\r\n");
                        return ;
                    }
                    isHandled = true;
                }
                else if(hm->method.len == 4 && hm->method.p[0] == 'P' && hm->method.p[1] == 'O' && hm->method.p[2] == 'S'&& hm->method.p[3] == 'T')
                {
                    pThis->_getHeader(hm);
                    handleRet = pThis->_delegate->onReceivePostData(std::string(hm->uri.p, hm->uri.len), pThis->_headermap, (void*)hm->body.p, hm->body.len);
                    isHandled = true;
                }

                if(isHandled)
                {
                    if(handleRet)
                    {
                        //mg_printf(nc, "%s", handleRet);
                        if(Config::allow_origin.empty())
                        {
                            mg_printf(nc, "%s%d%s%s%s", "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ", (int)handleRet->size(), "\r\n\r\n", handleRet->c_str(),"\r\n");
                        }
                        else
                        {
                            mg_printf(nc, "%s%d%s%s%s%s%s", "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ", (int)handleRet->size(), "\r\nAccess-Control-Allow-Origin: ", Config::allow_origin.c_str(), "\r\nAccess-Control-Allow-Methods: GET,POST\r\n\r\n", handleRet->c_str(),"\r\n");
                        }

                    }
                    else
                    {
                        mg_printf(nc, "%s%d%s%s", "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: ", (int)sizeof("default return"), "\r\n\r\n", "default return\r\n");
                    }
                }
                else
                {
                    LOG_WARNING("Unknow method:%s", hm->method.p);
                    mg_printf(nc, "%s%d%s%s", "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/plain\r\nContent-Length: ", (int)sizeof("Method Not Allowed"), "\r\n\r\n", "Method Not Allowed\r\n");
                }
            default:
                break;
        }
    }
    void HttpServer::_getHeader(struct http_message * hm)
    {
        _headermap.clear();
        for(int i = 0; i < MG_MAX_HTTP_HEADERS;i++)
        {
            if(hm->header_names[i].p != NULL)
            {
                _headermap.insert(std::pair<std::string, std::string>(
                        std::string(hm->header_names[i].p, hm->header_names[i].len),
                        std::string(hm->header_values[i].p, hm->header_values[i].len)));
            }
            else
            {
                break;
            }
        }
    }
    void HttpServer::_getQueryStrings(struct http_message * hm)
    {
        _queryStrings.clear();
        char * p = (char*)hm->query_string.p;
        char temp[512] = {0};
        int codeRet = -1;
        std::string query;
        std::string value;
        if(hm->query_string.p)
        {
            int equalSignIndex = -1;
            int beginIndex = 0;
            for(int i = 0 ; i < hm->query_string.len; i++)
            {
                if(p[i] == '=')
                {
                    equalSignIndex = i;
                }
                else if(i == hm->query_string.len - 1)
                {
                    if(equalSignIndex > beginIndex && i > equalSignIndex)
                    {
                        codeRet = mg_url_decode(p + beginIndex, equalSignIndex - beginIndex, temp, sizeof(temp), 0);
                        if(codeRet)
                        {
                            query = std::string(temp, codeRet);
                            codeRet = mg_url_decode(p + equalSignIndex + 1, i - equalSignIndex, temp, sizeof(temp), 0);
                            if(codeRet)
                            {
                                value = std::string(temp, codeRet);
                                _queryStrings.insert(std::pair<std::string, std::string>(query, value));
                            }
                        }
                    }
                    break;
                }
                else if(p[i] == '&')
                {
                    if(equalSignIndex > beginIndex && i > equalSignIndex + 1)
                    {
                        codeRet = mg_url_decode(p + beginIndex, equalSignIndex - beginIndex, temp, sizeof(temp), 0);
                        if(codeRet)
                        {
                            query = std::string(temp, codeRet);
                            codeRet = mg_url_decode(p + equalSignIndex + 1, i - equalSignIndex - 1, temp, sizeof(temp), 0);
                            if(codeRet)
                            {
                                value = std::string(temp, codeRet);
                                _queryStrings.insert(std::pair<std::string, std::string>(query, value));
                            }
                        }
                    }
                    beginIndex = i + 1;
                    equalSignIndex = - 1;
                }
            }
        }
    }
}