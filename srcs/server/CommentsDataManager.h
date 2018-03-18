//
// Created by xiaofeng on 1/3/18.
//

#ifndef SQLITE3TEST_COMMENTSDATAMANAGER_H
#define SQLITE3TEST_COMMENTSDATAMANAGER_H

#include <string>
#include "Config.h"
#include "CommentsCacheManager.h"
#include "sqlite3.h"
#include "ErrorCode.h"
#include "CommentsData.h"

namespace nifeng
{
    class CommentsDataManager
    {
    public:
        CommentsDataManager();

        ~CommentsDataManager();

        ErrCode getComments(const std::string& uri, int page, CommentsData& out_data, int& out_max_page);

        ErrCode insertComment(const std::string& uri, const InputElement& inputComment);

        ErrCode deleteComments(const std::string& uri);

        ErrCode deleteComment(const std::string& uri, int comment_id);

        bool init(const std::string& db_path, unsigned int cache_buffer_max_size = 0);

        void dropAllData(const std::string& db_path);
    private:
        sqlite3* _initDB(const std::string& db_path);

        void _finalize(sqlite3_stmt** p_stmt);

        int _createUriIfNotExist(const std::string& uri, int& floor_count);

        int _queryUriIsExist(const std::string& uri, int& floor_count);

        bool _queryParentIsExist(int parent_id);

        int _queryReplyTo(int parentID, int reply_to_sub_num);

        bool _queryReplyToIDisExist(int reply_to_id);

        int _insertCommentToDB(const StorageElement& element);

        int _queryComments(int group_id, int page, std::list<StorageElement>& outComments);

        int _updateFloorCount(int group_id, int value);
    private:
        CommentsCacheManager*   _cache_manager;
        bool                    _init_state;
        sqlite3*                _db;
    };
}



#endif //SQLITE3TEST_COMMENTSDATAMANAGER_H
