//
// Created by xiaofeng on 1/9/18.
//

#ifndef SQLITE3TEST_TESTCASELOADER_H
#define SQLITE3TEST_TESTCASELOADER_H
#include "CommentElement.h"
#include <vector>
namespace nifeng
{
    class TestCaseLoader
    {
    public:
        typedef struct {
            InputElement element;
            int comment_id;
        } InputCaseElement;
        static bool loadInputElements(const std::string path, std::vector<InputElement>& out);
    };
}
#endif //SQLITE3TEST_TESTCASELOADER_H
