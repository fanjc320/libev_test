#pragma once 
#include "dtype.h" 
#include "data/game_data.h" 
 
// tolua_begin
struct item_skill : public game_data 
{
    std::string comment; //
// tolua_end

    item_skill( )
    {
    }
    virtual bool read(int32_t index) override;
    virtual bool read_end(int32_t index) override;

// tolua_begin
};
// tolua_end
