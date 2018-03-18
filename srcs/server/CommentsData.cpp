//
// Created by xiaofeng on 1/8/18.
//

#include "CommentsData.h"
namespace nifeng
{
    CommentsData::CommentsData()
    {

    }
    CommentsData::~CommentsData()
    {
        clear();
    }
    void CommentsData::clear()
    {
        _comment_name_table.clear();
        _floor_table.clear();
        for(std::list<FloorData*>::iterator it = _data.begin(); it != _data.end(); ++it)
        {
            if((*it) != NULL)
            {
                delete (*it);
                *it = NULL;
            }
        }
        _data.clear();
    }
    void CommentsData::appendFloor(FloorData& floor_data)
    {
        FloorData* new_floor = new FloorData;
        for(FloorData::iterator it = floor_data.begin(); it != floor_data.end(); ++it)
        {
            new_floor->push_back(*it);
        }
        _data.push_back(new_floor);
    }
    void CommentsData::appendFloor(StorageElement& element)
    {
        _comment_name_table.insert(std::pair<int, std::string*>(element.comment_id, &element.name));
        if(element.parent_id == 0)
        {
            FloorData* floor = new FloorData;
            floor->push_back(ShowElement(element.comment_id, element.name, element.website, element.email, element.content, "", element.time));
            _data.push_back(floor);
            _floor_table.insert(std::pair<int ,FloorData*>(element.comment_id, floor));
        }
        else if(element.parent_id > 0)
        {
            std::map<int, FloorData *>::iterator floor_it = _floor_table.find(element.parent_id);
            if (floor_it != _floor_table.end())
            {
                FloorData *floor = floor_it->second;
                if (floor)
                {
                    std::map<int, std::string *>::iterator name_it = _comment_name_table.find(element.reply_to_id);
                    if( _floor_table.find(element.reply_to_id) != _floor_table.end() ||
                            name_it == _comment_name_table.end())
                    {
                        floor->push_back(ShowElement(element.comment_id, element.name, element.website, element.email,
                                                     element.content, "", element.time));
                    }
                    else
                    {
                        floor->push_back(ShowElement(element.comment_id, element.name, element.website, element.email,
                                                     element.content, *name_it->second, element.time));
                    }
                }
            }
        }
    }
    std::list<CommentsData::FloorData*>& CommentsData::getComments()
    {
        return _data;
    }
}