#pragma once  
#include "dtype.h"

using namespace std;
// tolua_begin
struct game_data
{
    int	    	id;
    string	    name;
    long long   key;
// tolua_end

    game_data()
    {
        id = 0;
        key = 0;
    }
    virtual bool read(int32_t index);
    virtual bool read_end(int32_t index);
// tolua_begin
};
// tolua_end