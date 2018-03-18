//
// Created by xiaofeng on 1/8/18.
//

#ifndef SQLITE3TEST_COMMENTSDATA_H
#define SQLITE3TEST_COMMENTSDATA_H
#include <string>
#include <list>
#include <map>
#include "CommentElement.h"

namespace nifeng
{
    class CommentsData
    {
    public:
        typedef std::list<ShowElement> FloorData;
        CommentsData();
        ~CommentsData();
        void clear();
        void appendFloor(StorageElement& element);
        void appendFloor(FloorData& floor_data);
        std::list<FloorData*>& getComments();
    private:
        std::list<FloorData*> _data;
        std::map <int, std::string* > _comment_name_table;
        std::map <int, FloorData* > _floor_table;
    };
}
#endif //SQLITE3TEST_COMMENTSDATA_H
