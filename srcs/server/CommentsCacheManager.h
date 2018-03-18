//
// Created by xiaofeng on 1/3/18.
//

#ifndef SQLITE3TEST_COMMENTSCACHEMANAGER_H
#define SQLITE3TEST_COMMENTSCACHEMANAGER_H
#include <map>
#include "CommentsData.h"
#include "CommentsDataCache.h"
#include "ErrorCode.h"

namespace nifeng
{
    class CommentsCacheManager
    {
    //策略： 缓存已满的时候，让24小时内访问频率低的50% 失效
    public:
        CommentsCacheManager(int cache_max_size_in_byte);
        ~CommentsCacheManager();
        bool getComments(const std::string& uri, int page, CommentsData& out_data);
        bool pushToCache(const std::string& uri, CommentsData& comments_data, int page);
        void deleteCache(const std::string& uri);
    private:
        std::map<std::string, CommentsDataCache*> _data;
        int _cache_size;
        int _cache_max_size;
    };
}



#endif //SQLITE3TEST_COMMENTSCACHEMANAGER_H
