//
// Created by xiaofeng on 1/3/18.
//

#include "CommentsCacheManager.h"
#include "Config.h"
namespace nifeng
{
    CommentsCacheManager::CommentsCacheManager(int cache_max_size_in_byte):
            _cache_max_size(cache_max_size_in_byte),
            _cache_size(0)
    {
    }
    CommentsCacheManager::~CommentsCacheManager()
    {
        for(std::map<std::string, CommentsDataCache*>::iterator it = _data.begin(); it!=_data.end();)
        {
            delete it->second;
            _data.erase(it++);
        }
    }
    void CommentsCacheManager::deleteCache(const std::string& uri)
    {
        if(uri.size() >= 0)
        {
            std::map<std::string, CommentsDataCache*>::iterator pFind =  _data.find(uri);
            if(pFind != _data.end())
            {
                if(pFind->second)
                {
                    delete pFind->second;
                }
                _data.erase(pFind);
            }
        }
    }
    bool CommentsCacheManager::pushToCache(const std::string& uri, CommentsData& comments_data, int page)
    {
        bool ret = false;
        if(uri.size() <= 0)
            return ret;

        if(_cache_size >= _cache_max_size)
        {
            int max_size = _cache_max_size / 2;
            int now_size = _cache_size;
            for(std::map<std::string, CommentsDataCache*>::iterator it = _data.begin(); it!=_data.end();)
            {
                if(it->second)
                {
                    now_size -= it->second->memSize();
                    delete it->second;
                }
                _data.erase(it++);
                if(now_size <= max_size)
                {
                    _cache_size = now_size;
                    break;
                }
            }
        }
        CommentsDataCache* pHandle = NULL;
        std::map<std::string, CommentsDataCache*>::iterator pFind =  _data.find(uri);
        if(pFind == _data.end())
        {
            pHandle = new CommentsDataCache;
            _data.insert(std::pair<std::string, CommentsDataCache*>(uri, pHandle));
        }
        else
        {
            pHandle = pFind->second;
        }
        if(pHandle)
        {
            int oldSize = pHandle->memSize();
            pHandle->update(page, comments_data);
            _cache_size += pHandle->memSize() - oldSize;
            ret = true;
        }
        return ret;
    }
    bool CommentsCacheManager::getComments(const std::string& uri, int page, CommentsData& out_data)
    {
        bool ret = false;
        if(uri.size() > 0)
        {
            std::map<std::string, CommentsDataCache*>::iterator pFind =  _data.find(uri);

            if(pFind != _data.end())
            {
                if(pFind->second)
                {
                    ret = pFind->second->getComments(page, out_data);
                }
            }
        }
        return ret;

    }
}