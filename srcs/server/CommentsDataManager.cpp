//
// Created by xiaofeng on 1/3/18.
//

#include "CommentsDataManager.h"
#include <ctime>
#include <stdio.h>
const char* create_uri_table_cmd =
        "CREATE TABLE IF NOT EXISTS uri("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "uri TEXT NOT NULL,"
        "count INTEGER NOT NULL"
        ");";
const char* create_comments_table_cmd =
        "CREATE TABLE IF NOT EXISTS comments("
        "comment_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "group_id INTEGER NOT NULL,"
        "reply_to_id INTEGER NOT NULL,"
        "parent_id INTEGER NOT NULL,"
        "name TEXT,"
        "email TEXT,"
        "website TEXT,"
        "content TEXT NOT NULL,"
        "time INTEGER NOT NULL,"
        "FOREIGN KEY(group_id) REFERENCES uri(id)"
        ");";

namespace nifeng
{
    CommentsDataManager::CommentsDataManager():
            _init_state(false),
            _cache_manager(NULL),
            _db(NULL)
    {

    }
    CommentsDataManager::~CommentsDataManager()
    {
        if(_cache_manager)
        {
            delete _cache_manager;
        }
        if(_db)
        {
            sqlite3_close(_db);
        }
    }
    bool CommentsDataManager::init(const std::string& db_path, unsigned int cache_buffer_max_size)
    {
        if(!_init_state)
        {
            _db = _initDB(db_path);
            if(!_db)
                return false;
            if(!_cache_manager && cache_buffer_max_size > 0)
            {
                _cache_manager = new CommentsCacheManager(cache_buffer_max_size);
            }
            _init_state = true;
        }
        return true;

    }
    sqlite3* CommentsDataManager::_initDB(const std::string& db_path)
    {
        sqlite3* db = NULL;
        bool sql_ret = false;
        do
        {
            char* err_msg = NULL;
            if(sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
            {
                fprintf(stderr,"open db failed:%s\n",sqlite3_errmsg(db));
                break;
            }

            if(sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL, &err_msg) != SQLITE_OK)
            {
                fprintf(stderr,"PRAGMA foreign_keys ON failed:%s\n",sqlite3_errmsg(db));
                break;
            }

            if(sqlite3_exec(db, create_uri_table_cmd, NULL, NULL, &err_msg) != SQLITE_OK)
            {
                fprintf(stderr,"create_uri_table failed:%s\n",sqlite3_errmsg(db));
                break;
            }

            if(sqlite3_exec(db, create_comments_table_cmd, NULL, NULL, &err_msg) != SQLITE_OK)
            {
                fprintf(stderr,"create_comments_table failed:%s\n",sqlite3_errmsg(db));
                break;
            }
            sql_ret = true;
        }
        while(false);

        if(!sql_ret)
        {
            if(db)
            {
                sqlite3_close(db);
                db = NULL;
            }
        }
        return db;
    }
    ErrCode CommentsDataManager::getComments(const std::string& uri, int page, CommentsData& out_data, int& out_max_page)
    {
        std::list<StorageElement> elements;
        int floor_count = 0;
        if(_init_state && uri.size() && uri.size() < 255 && page >= 0)
        {
            int group_id = _queryUriIsExist(uri, floor_count);
            if(group_id > 0)
            {
                if(Config::enable_pagination)
                {
                    if(page == 0)
                    {
                        page = 1;
                    }
                    else if( floor_count <= (page - 1) * Config::max_comments_per_page)
                    {
                        return ERR_NO_COMMENTS;
                    }
                    out_max_page = floor_count / Config::max_comments_per_page + ((floor_count % Config::max_comments_per_page > 0) ? 1 : 0);
                }
                else
                {
                    out_max_page = 1;
                    page = 0;
                }
                if(_cache_manager && _cache_manager->getComments(uri, page, out_data))
                {
                    return SUCCEED;
                }
                if (_queryComments(group_id, page, elements) < 0)
                {
                    return ERR_GET_COMMENTS_FAILED;
                }
                if(elements.size() <= 0)
                {
                    return ERR_NO_COMMENTS;
                }
                out_data.clear();

                for(std::list<StorageElement>::iterator it = elements.begin(); it != elements.end(); ++it)
                {
                    out_data.appendFloor(*it);
                }
                if(_cache_manager)
                {
                    _cache_manager->pushToCache(uri, out_data, page);
                }
                return SUCCEED;
            }
            else
            {
                return ERR_NO_COMMENTS;
            }
        }
        return ERR_PARAMS_INVALID;
    }
    ErrCode CommentsDataManager::insertComment(const std::string &uri, const InputElement& inputComment)
    {
        if(_init_state &&
                uri.size() && uri.size() < 255 &&
                inputComment.content.size() > 0 && inputComment.content.size() < Config::max_comment_char_num &&
                inputComment.email.size() < 50 &&
                inputComment.website.size() < 50 &&
                inputComment.name.size() < 30 &&
                inputComment.reply_to_id >= 0 &&
                inputComment.parent_id >=0 )
        {
            int group_id = 0;
            int floor_count = 0;
            group_id = _createUriIfNotExist(uri, floor_count);
            if(group_id <= 0)
            {
                return ERR_CREATE_URI_TABLE_FAILED;
            }
            if(inputComment.parent_id > 0)
            {
                if(!_queryParentIsExist(inputComment.parent_id))
                {
                    return ERR_FLOOR_ID;
                }
            }
            if(inputComment.reply_to_id > 0)
            {
                if(!_queryReplyToIDisExist(inputComment.reply_to_id))
                {
                    return ERR_REPLY_TO;
                }
            }
            StorageElement e(inputComment.name,
                             inputComment.website,
                             inputComment.content,
                             inputComment.email,
                             group_id,
                             inputComment.reply_to_id,
                             inputComment.parent_id,
                             std::time(NULL)
            );
            if (0 != _insertCommentToDB(e))
            {
                return ERR_INSERT_COMMENT_FAILED;
            }
            if (0 == inputComment.reply_to_id  && 0 != _updateFloorCount(group_id, floor_count + 1))
            {
                return ERR_UPDATE_COUNT_FAILED;
            }
            if(_cache_manager)
            {
                _cache_manager->deleteCache(uri);
            }
            return SUCCEED;
        }
        return ERR_PARAMS_INVALID;
    }
    void CommentsDataManager::dropAllData(const std::string& db_path)
    {
        sqlite3* db = NULL;
        do
        {
            char *err_msg = NULL;
            if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
            {
                fprintf(stderr, "open db failed:%s\n", sqlite3_errmsg(db));
                break;
            }
            if(sqlite3_exec(db, "DROP TABLE IF EXISTS comments", NULL, NULL, &err_msg) != SQLITE_OK)
            {
                fprintf(stderr,"drop_comments_table failed:%s\n",sqlite3_errmsg(db));
                break;
            }
            if(sqlite3_exec(db, "DROP TABLE IF EXISTS uri", NULL, NULL, &err_msg) != SQLITE_OK)
            {
                fprintf(stderr,"drop_uri_table failed:%s\n",sqlite3_errmsg(db));
                break;
            }
        }
        while(false);

        if(db)
        {
            sqlite3_close(db);
        }
    }
    ErrCode CommentsDataManager::deleteComments(const std::string &key)
    {
        //TODO
        return SUCCEED;
    }

    ErrCode CommentsDataManager::deleteComment(const std::string &key, int comment_id)
    {
        //TODO
        return SUCCEED;
    }
    void CommentsDataManager::_finalize(sqlite3_stmt** p_stmt)
    {
        if(*p_stmt)
        {
            sqlite3_finalize(*p_stmt);
            *p_stmt = NULL;
        }
    }

    int CommentsDataManager::_createUriIfNotExist(const std::string& uri, int& floor_count)
    {
        sqlite3_stmt* p_stat = NULL;
        int ret = -1;
        floor_count = 0;
        if(_db && uri.size() > 0)
        {
            do
            {
                int sql_ret = sqlite3_prepare_v2(_db, "SELECT id, count FROM uri WHERE uri == ?", -1, &p_stat, 0);
                if (sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed to prepare in query uri: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                sql_ret = sqlite3_bind_text(p_stat, 1, uri.c_str(), -1, NULL);
                if (sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed to bind_text in query uri: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                int step = sqlite3_step(p_stat);

                if (step == SQLITE_ROW || step == SQLITE_DONE)
                {
                    ret = sqlite3_column_int(p_stat, 0);
                    if(ret > 0)
                    {
                        floor_count = sqlite3_column_int(p_stat, 1);
                        break;
                    }
                    ret = -1;
                    _finalize(&p_stat);
                    sql_ret = sqlite3_prepare_v2(_db, "INSERT INTO uri VALUES (NULL, ?, 0)", -1, &p_stat, 0);
                    if (sql_ret != SQLITE_OK)
                    {
                        fprintf(stderr, "Failed insert to uri table: %s\n", sqlite3_errmsg(_db));
                        break;
                    }
                    sql_ret = sqlite3_bind_text(p_stat, 1, uri.c_str(), -1, NULL);
                    if (sql_ret != SQLITE_OK)
                    {
                        fprintf(stderr, "Failed to bind_text in insert uri: %s\n", sqlite3_errmsg(_db));
                        break;
                    }
                    if (sqlite3_step(p_stat) != SQLITE_DONE)
                    {
                        fprintf(stderr, "Failed to  insert uri: %s\n", sqlite3_errmsg(_db));
                        break;
                    }
                    ret = sqlite3_last_insert_rowid(_db);
                }
                else
                {
                    break;
                }
            }
            while(false);
        }
        _finalize(&p_stat);
        return ret;
    }
    bool CommentsDataManager::_queryReplyToIDisExist(int reply_to_id)
    {
        bool ret = false;

        sqlite3_stmt* p_stat = NULL;

        if(_db && reply_to_id > 0)
        {
            do
            {
                int sql_ret = sqlite3_prepare_v2(_db, "SELECT comment_id FROM comments WHERE comment_id == ?", -1, &p_stat, 0);
                if(sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed query comment_id: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                sqlite3_bind_int(p_stat, 1, reply_to_id);
                int step = sqlite3_step(p_stat);
                if (step == SQLITE_ROW || step == SQLITE_DONE)
                {
                   ret = true;
                }
            }
            while(0);
        }
        _finalize(&p_stat);
        return ret;
    }
    int CommentsDataManager::_updateFloorCount(int group_id, int value)
    {
        sqlite3_stmt* p_stat = NULL;
        int ret = -1;

        if(_db && group_id > 0 && value > 0)
        {
            do
            {
                int sql_ret = sqlite3_prepare_v2(_db,
                                                 "UPDATE uri SET count = ? WHERE id == ?",
                                                 -1, &p_stat, 0);
                if(sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed update count of comments: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                sqlite3_bind_int(p_stat, 1, value);
                sqlite3_bind_int(p_stat, 2, group_id);
                if (sqlite3_step(p_stat) != SQLITE_DONE)
                {
                    fprintf(stderr, "Failed update count of comments: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                ret = 0;
            }
            while(false);
        }
        _finalize(&p_stat);
        return ret;
    }

    int CommentsDataManager::_insertCommentToDB(const StorageElement& e)
    {
        sqlite3_stmt* p_stat = NULL;
        int ret = -1;

        if(_db && e.content.size() > 0 && e.group_id > 0 && e.reply_to_id >= 0 && e.parent_id >= 0)
        {
            do
            {
                int sql_ret = sqlite3_prepare_v2(_db,
                                                 "INSERT INTO comments VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?)",
                                                 -1, &p_stat, 0);
                if(sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed insert to comments table: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                sqlite3_bind_int(p_stat, 1, e.group_id);
                sqlite3_bind_int(p_stat, 2, e.reply_to_id);
                sqlite3_bind_int(p_stat, 3, e.parent_id);
                sqlite3_bind_text(p_stat, 4, e.name.c_str(), -1, NULL);
                sqlite3_bind_text(p_stat, 5, e.email.c_str(), -1, NULL);
                sqlite3_bind_text(p_stat, 6, e.website.c_str(), -1, NULL);
                sqlite3_bind_text(p_stat, 7, e.content.c_str(), -1, NULL);
                sqlite3_bind_int(p_stat, 8, e.time);
                if (sqlite3_step(p_stat) != SQLITE_DONE)
                {
                    fprintf(stderr, "Failed to  insert comments: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                ret = 0;
            }
            while(false);
        }
        _finalize(&p_stat);
        return ret;
    }
    bool CommentsDataManager::_queryParentIsExist(int parent_id)
    {
        sqlite3_stmt* p_stat = NULL;
        int ret = false;

        if(_db && parent_id > 0)
        {
            do
            {
                int sql_ret = sqlite3_prepare_v2(_db, "SELECT reply_to_id, parent_id FROM comments WHERE comment_id == ?", -1, &p_stat, 0);
                if(sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed query parent_comment_id: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                sqlite3_bind_int(p_stat, 1, parent_id);
                int step = sqlite3_step(p_stat);
                if (step == SQLITE_ROW || step == SQLITE_DONE)
                {
                    int temp_reply_to_id = sqlite3_column_int(p_stat, 0);
                    int temp_parent_id = sqlite3_column_int(p_stat, 1);
                    if(temp_reply_to_id == 0 && temp_parent_id == 0)
                    {
                        ret = true;
                    }
                }
                else
                {
                    fprintf(stderr, "Failed to  query comment_id: %s\n", sqlite3_errmsg(_db));
                    break;
                }
            }
            while(0);
        }
        _finalize(&p_stat);
        return ret;
    }
//    int CommentsDataManager::_queryReplyTo(int parentID, int reply_to_sub_num)
//    {
//        sqlite3_stmt* p_stat = NULL;
//        int ret = -1;
//
//        if(_db && parentID && reply_to_sub_num > 0)
//        {
//            do
//            {
//                int sql_ret = sqlite3_prepare_v2(_db, "SELECT comment_id FROM comments WHERE parent_id == ?  LIMIT 1 OFFSET ?", -1, &p_stat, 0);
//                if(sql_ret != SQLITE_OK)
//                {
//                    fprintf(stderr, "Failed query parent id: %s\n", sqlite3_errmsg(_db));
//                    break;
//                }
//                sqlite3_bind_int(p_stat, 1, parentID);
//                sqlite3_bind_int(p_stat, 2, reply_to_sub_num - 1);
//                int step = sqlite3_step(p_stat);
//                if (step == SQLITE_ROW || step == SQLITE_DONE)
//                {
//                    int temp_id = sqlite3_column_int(p_stat, 0);
//                    if(temp_id > 0)
//                        ret = temp_id;
//                }
//                else
//                {
//                    fprintf(stderr, "Failed to query parent id: %s\n", sqlite3_errmsg(_db));
//                    break;
//                }
//            }
//            while(false);
//        }
//        _finalize(&p_stat);
//        return ret;
//    }
    int CommentsDataManager::_queryUriIsExist(const std::string& uri, int& floor_count)
    {
        sqlite3_stmt* p_stat = NULL;
        int ret = -1;
        floor_count = 0;
        if(_db && uri.size() > 0)
        {
            do
            {
                int sql_ret = sqlite3_prepare_v2(_db, "SELECT id, count FROM uri WHERE uri == ?", -1, &p_stat, 0);
                if (sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed to prepare in query uri: %s\n", sqlite3_errmsg(_db));
                    break;
                }
                sql_ret = sqlite3_bind_text(p_stat, 1, uri.c_str(), -1, NULL);
                if (sql_ret != SQLITE_OK)
                {
                    fprintf(stderr, "Failed to bind_text in query uri: %s\n", sqlite3_errmsg(_db));
                    break;
                }

                int step = sqlite3_step(p_stat);
                if (step == SQLITE_ROW || step == SQLITE_DONE)
                {
                    int temp_id = sqlite3_column_int(p_stat, 0);
                    if(temp_id > 0)
                    {
                        ret = temp_id;
                        floor_count = sqlite3_column_int(p_stat, 1);
                    }
                }
                else
                {
                    fprintf(stderr, "Failed to query parent id: %s\n", sqlite3_errmsg(_db));
                    break;
                }
            }
            while(false);
        }
        _finalize(&p_stat);
        return ret;
    }

    int CommentsDataManager::_queryComments(int group_id, int page, std::list<StorageElement>& outComments)
    {
        sqlite3_stmt* p_stat = NULL;
        int ret = -1;
        int sql_ret = -1;
        if(_db && group_id > 0 && page >= 0)
        {
            do
            {
                outComments.clear();
                if(page > 0)
                {
                    int offset =  (page - 1) * Config::max_comments_per_page;
                    sql_ret = sqlite3_prepare_v2(_db, "SELECT comment_id, reply_to_id, parent_id, name, email, website, content, time "
                            "FROM comments WHERE group_id == ? AND parent_id == 0 LIMIT ? OFFSET ?", -1, &p_stat, 0);
                    if (sql_ret != SQLITE_OK)
                    {
                        fprintf(stderr, "Failed to prepare in get comments: %s\n", sqlite3_errmsg(_db));
                        break;
                    }

                    sqlite3_bind_int(p_stat, 1, group_id);
                    sqlite3_bind_int(p_stat, 2, Config::max_comments_per_page);
                    sqlite3_bind_int(p_stat, 3, offset);

                    std::list<int> parentList;

                    do
                    {
                        sql_ret = sqlite3_step(p_stat);
                        if(sql_ret == SQLITE_ROW)
                        {
                            int id = sqlite3_column_int(p_stat, 0);
                            outComments.push_back(StorageElement((const char*)sqlite3_column_text(p_stat, 3),
                                                                 (const char*)sqlite3_column_text(p_stat, 5),
                                                                 (const char*)sqlite3_column_text(p_stat, 6),
                                                                 (const char*)sqlite3_column_text(p_stat, 4),
                                                                 0,
                                                                 sqlite3_column_int(p_stat, 1),
                                                                 sqlite3_column_int(p_stat, 2),
                                                                 sqlite3_column_int(p_stat, 7),
                                                                 id
                            ));
                            parentList.push_back(id);
                        }
                    }
                    while(sql_ret != SQLITE_DONE);

                    for(std::list<int>::iterator it = parentList.begin(); it != parentList.end(); ++it)
                    {
                        if(*it > 0)
                        {
                            _finalize(&p_stat);
                            sql_ret = sqlite3_prepare_v2(_db, "SELECT comment_id, reply_to_id, parent_id, name, email, website, content, time "
                                    "FROM comments WHERE group_id == ? AND parent_id == ?", -1, &p_stat, 0);

                            if (sql_ret != SQLITE_OK)
                            {
                                fprintf(stderr, "Failed to prepare in get comments: %s\n", sqlite3_errmsg(_db));
                                return ret;
                            }

                            sqlite3_bind_int(p_stat, 1, group_id);
                            sqlite3_bind_int(p_stat, 2, *it);

                            do
                            {
                                sql_ret = sqlite3_step(p_stat);
                                if(sql_ret == SQLITE_ROW)
                                {
                                    outComments.push_back(StorageElement((const char*)sqlite3_column_text(p_stat, 3),
                                                                         (const char*)sqlite3_column_text(p_stat, 5),
                                                                         (const char*)sqlite3_column_text(p_stat, 6),
                                                                         (const char*)sqlite3_column_text(p_stat, 4),
                                                                         0,
                                                                         sqlite3_column_int(p_stat, 1),
                                                                         sqlite3_column_int(p_stat, 2),
                                                                         sqlite3_column_int(p_stat, 7),
                                                                         sqlite3_column_int(p_stat, 0)
                                    ));
                                }
                            }
                            while(sql_ret != SQLITE_DONE);
                        }
                    }
                    ret = 0;
                }
                else
                {
                    sql_ret = sqlite3_prepare_v2(_db, "SELECT comment_id, reply_to_id, parent_id, name, email, website, content, time "
                            "FROM comments WHERE group_id == ?", -1, &p_stat, 0);
                    if (sql_ret != SQLITE_OK)
                    {
                        fprintf(stderr, "Failed to prepare in get comments: %s\n", sqlite3_errmsg(_db));
                        break;
                    }
                    sqlite3_bind_int(p_stat, 1, group_id);
                    do
                    {
                        sql_ret = sqlite3_step(p_stat);
                        if(sql_ret == SQLITE_ROW)
                        {
                            outComments.push_back(StorageElement((const char*)sqlite3_column_text(p_stat, 3),
                                                                 (const char*)sqlite3_column_text(p_stat, 5),
                                                                 (const char*)sqlite3_column_text(p_stat, 6),
                                                                 (const char*)sqlite3_column_text(p_stat, 4),
                                                                 0,
                                                                 sqlite3_column_int(p_stat, 1),
                                                                 sqlite3_column_int(p_stat, 2),
                                                                 sqlite3_column_int(p_stat, 7),
                                                                 sqlite3_column_int(p_stat, 0)
                            ));
                        }
                    }
                    while(sql_ret != SQLITE_DONE);
                    ret = 0;
                }
            }
            while(false);
        }
        _finalize(&p_stat);
        return ret;
    }
}