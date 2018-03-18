//
// Created by xiaofeng on 1/3/18.
//

#include <cstring>
#include "CommentsDataCache.h"
#include "Config.h"

namespace nifeng
{
    CommentsDataCache::CommentsDataCache():
            _floor_size(1000),
            _mem_size(0),
            _comments_data(NULL)
    {
        _comments_data = new CommentsData::FloorData*[_floor_size];
        for(int i = 0 ; i < _floor_size; i++)
        {
            _comments_data[i] = NULL;
        }
    }

    void CommentsDataCache::update(int page, CommentsData& inputData)
    {
        std::list<CommentsData::FloorData*>& d_list = inputData.getComments();
        int floorMaxSize = d_list.size();
        int start_index = 0;
        if(page > 0)
        {
            floorMaxSize = (page - 1) * Config::max_comments_per_page + d_list.size();
            start_index = (page - 1) * Config::max_comments_per_page;
        }

        if(floorMaxSize > _floor_size)
        {
            CommentsData::FloorData** new_commentData = new CommentsData::FloorData * [floorMaxSize];
            CommentsData::FloorData* pfs;
            memcpy(new_commentData, _comments_data, sizeof(pfs) * _floor_size);
            memset(new_commentData + sizeof(pfs) * _floor_size, 0, floorMaxSize - _floor_size);
            clearMem();
            _comments_data = new_commentData;
            _floor_size = floorMaxSize;
        }

        for(std::list<CommentsData::FloorData*>::iterator it = d_list.begin(); it!= d_list.end(); ++it, ++start_index)
        {
            if(_comments_data[start_index])
            {
                delete _comments_data[start_index];
            }
            _comments_data[start_index] = new CommentsData::FloorData;

            CommentsData::FloorData* one_floor = *it;
            for(CommentsData::FloorData::iterator p_it = one_floor->begin(); p_it != one_floor->end(); ++p_it)
            {
                _comments_data[start_index]->push_back(*p_it);
                _mem_size += p_it->mem_size();
            }
        }
    }
    void CommentsDataCache::clearMem()
    {
        if(_comments_data)
        {
            for(int i = 0; i < _floor_size; i++)
            {
                if(_comments_data[i])
                {
                    delete _comments_data[i];
                }
            }
            delete [] _comments_data;
            _comments_data = NULL;
        }
    }
    bool CommentsDataCache::getComments(int page, CommentsData& out_data)
    {
        bool ret = false;

        if(page > 0)
        {
            int start_index = (page - 1) * Config::max_comments_per_page;

            if(start_index  < _floor_size)
            {

                int loop_max = start_index +  Config::max_comments_per_page;
                if(loop_max > _floor_size)
                    loop_max = _floor_size;
                out_data.clear();
                for(int i = start_index; i < loop_max; i++)
                {
                    if(_comments_data[i])
                    {
                        out_data.appendFloor(*_comments_data[i]);
                    }
                }
                if(out_data.getComments().size() > 0)
                {
                    ret = true;
                }
            }
        }
        return ret;
    }
    CommentsDataCache::~CommentsDataCache()
    {
        clearMem();
    }
    unsigned int CommentsDataCache::memSize()
    {
        return _mem_size;
    }
}