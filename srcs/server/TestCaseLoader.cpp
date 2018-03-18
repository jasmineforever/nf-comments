//
// Created by xiaofeng on 1/9/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <list>
#include "TestCaseLoader.h"
namespace nifeng
{

    bool TestCaseLoader::loadInputElements(const std::string path, std::vector<InputElement>& out)
    {
        bool ret = false;
        char r_buffer[1024];
        FILE* fp = fopen(path.c_str(), "r");
        int lineCount = 1;
        std::list<InputCaseElement> InputCaseElements;
        out.clear();
        if(fp)
        {
            while(!feof(fp))
            {
                if(NULL != fgets(r_buffer, 1024, fp))
                {
                    if(r_buffer[0] == '#')
                    {
                        continue;
                    }
                    int len = strlen(r_buffer);
                    if(len >= 1024)
                    {
                        printf("error line %d: string too long", lineCount);
                        exit(1);
                    }
                    r_buffer[len - 1] = '|';
                    InputCaseElement e;
                    int b_index = 0;
                    int f_count = 0;
                    bool is_got = false;
                    for(int i = 0; i < len; i++)
                    {
                        if(r_buffer[i] == '|')
                        {
                            if(f_count == 0)
                            {
                                e.comment_id = atoi(std::string(r_buffer + b_index, i - b_index).c_str());
                            }
                            else if(f_count == 1)
                            {
                                e.element.name = std::string(r_buffer + b_index, i - b_index);
                            }
                            else if(f_count == 2)
                            {
                                e.element.website = std::string(r_buffer + b_index, i - b_index);
                            }
                            else if(f_count == 3)
                            {
                                e.element.content = std::string(r_buffer + b_index, i - b_index);
                            }
                            else if(f_count == 4)
                            {
                                e.element.email = std::string(r_buffer + b_index, i - b_index);
                            }
                            else if(f_count == 5)
                            {
                                e.element.reply_to_id = atoi(std::string(r_buffer + b_index, i - b_index).c_str());
                            }
                            else if(f_count == 6)
                            {
                                e.element.parent_id = atoi(std::string(r_buffer + b_index, i - b_index).c_str());
                            }
                            b_index = i + 1;
                            f_count += 1;
                            if(f_count == 7)
                            {
                                is_got = true;
                                break;
                            }
                        }
                    }
                    if(is_got)
                    {
                        if(InputCaseElements.size() > 0)
                        {
                            if(InputCaseElements.back().comment_id < e.comment_id)
                            {
                                InputCaseElements.push_back(e);
                            }
                            else
                            {
                                for(std::list<InputCaseElement>::iterator it = InputCaseElements.begin(); it != InputCaseElements.end(); ++it)
                                {
                                    if(it->comment_id > e.comment_id)
                                    {
                                        InputCaseElements.insert(it, e);
                                        break;
                                    }
                                }
                            }
                        }
                        else
                        {
                            InputCaseElements.push_back(e);
                        }

                    }
                    else
                    {
                        r_buffer[len - 1] = '\0';
                        printf("error line %d:%s\n", lineCount, r_buffer);
                    }
                    lineCount++;
                }
                else
                {
                    break;
                }
            }
            if(InputCaseElements.size() > 0)
            {
                for(std::list<InputCaseElement>::iterator it = InputCaseElements.begin(); it != InputCaseElements.end(); ++it)
                {
                    out.push_back(it->element);
                }
            }
            ret = true;
            fclose(fp);
        }
        return ret;
    }
}