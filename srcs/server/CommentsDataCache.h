//
// Created by xiaofeng on 1/3/18.
//

#ifndef SQLITE3TEST_COMMENTSDATA_CACHE_H
#define SQLITE3TEST_COMMENTSDATA_CACHE_H
#include <vector>
#include <list>
#include "CommentElement.h"
#include "CommentsData.h"
namespace nifeng
{
    class CommentsDataCache
    {
    public:
        CommentsDataCache();
        ~CommentsDataCache();
        void update(int page, CommentsData& inputData);
        bool getComments(int page, CommentsData& out_data);
        unsigned int memSize();
        void clearMem();
    private:
        CommentsData::FloorData**   _comments_data;
        int                         _floor_size;
        int                         _mem_size;
    };
}



#endif //SQLITE3TEST_COMMENTSDATA_CACHE_H
