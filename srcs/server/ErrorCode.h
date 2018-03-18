//
// Created by xiaofeng on 1/5/18.
//

#ifndef SQLITE3TEST_ERRORCODE_H
#define SQLITE3TEST_ERRORCODE_H
#include <string>

namespace nifeng
{
    typedef enum
    {
        SUCCEED = 0,
        ERR_PARAMS_INVALID = 10000,
        ERR_CREATE_URI_TABLE_FAILED,
        ERR_INSERT_COMMENT_FAILED,
        ERR_REPLY_TO,
        ERR_GET_COMMENTS_FAILED,
        ERR_NO_COMMENTS,
        ERR_UPDATE_COUNT_FAILED,
        ERR_NAME,
        NAME_IS_OCCUPIED,
        ERR_FLOOR_ID
    } ErrCode;

    const int err_str_offset = (int)ERR_PARAMS_INVALID;
    const std::string err_str[] = {
            "query params invalid",
            "key table create failed",
            "add comment failed",
            "input replay params error",
            "get comments failed",
            "empty comments",
            "update count failed",
            "input name err",
            "please use other name",
            "floor_id not exist"
    };
}
#endif //SQLITE3TEST_ERRORCODE_H

