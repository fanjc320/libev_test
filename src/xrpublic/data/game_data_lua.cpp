/*
** Lua binding: game_data_lua
** Generated automatically by tolua++-1.0.93-lua53 on Wed Jun  8 11:10:57 2022.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_game_data_lua_open (lua_State* tolua_S);

#include "data/game_data.h"
#include "data/game/item_base.h"
#include "data/game/item_skill.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"game_data");
 tolua_usertype(tolua_S,"item_skill");
 tolua_usertype(tolua_S,"item_base");
}

/* get function: id of class  game_data */
#ifndef TOLUA_DISABLE_tolua_get_game_data_id
static int tolua_get_game_data_id(lua_State* tolua_S)
{
  game_data* self = (game_data*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'id'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->id);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: id of class  game_data */
#ifndef TOLUA_DISABLE_tolua_set_game_data_id
static int tolua_set_game_data_id(lua_State* tolua_S)
{
  game_data* self = (game_data*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'id'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->id = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: name of class  game_data */
#ifndef TOLUA_DISABLE_tolua_get_game_data_name
static int tolua_get_game_data_name(lua_State* tolua_S)
{
  game_data* self = (game_data*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'name'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->name);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: name of class  game_data */
#ifndef TOLUA_DISABLE_tolua_set_game_data_name
static int tolua_set_game_data_name(lua_State* tolua_S)
{
  game_data* self = (game_data*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'name'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->name = ((string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: key of class  game_data */
#ifndef TOLUA_DISABLE_tolua_get_game_data_key
static int tolua_get_game_data_key(lua_State* tolua_S)
{
  game_data* self = (game_data*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'key'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->key);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: key of class  game_data */
#ifndef TOLUA_DISABLE_tolua_set_game_data_key
static int tolua_set_game_data_key(lua_State* tolua_S)
{
  game_data* self = (game_data*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'key'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->key = ((long long)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: comment of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_item_base_comment
static int tolua_get_item_base_comment(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'comment'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->comment);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: comment of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_item_base_comment
static int tolua_set_item_base_comment(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'comment'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->comment = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: price of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_item_base_price
static int tolua_get_item_base_price(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'price'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->price);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: price of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_item_base_price
static int tolua_set_item_base_price(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'price'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->price = ((double)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: point of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_item_base_point
static int tolua_get_item_base_point(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'point'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->point);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: point of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_item_base_point
static int tolua_set_item_base_point(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'point'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->point = ((float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: num of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_item_base_num
static int tolua_get_item_base_num(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'num'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->num);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: num of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_item_base_num
static int tolua_set_item_base_num(lua_State* tolua_S)
{
  item_base* self = (item_base*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'num'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->num = ((long long)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: sid of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_game_data_lua_item_base_sid
static int tolua_get_game_data_lua_item_base_sid(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->sid[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: sid of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_game_data_lua_item_base_sid
static int tolua_set_game_data_lua_item_base_sid(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->sid[tolua_index] = ((int)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: sname of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_game_data_lua_item_base_sname
static int tolua_get_game_data_lua_item_base_sname(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushcppstring(tolua_S,(const char*)self->sname[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: sname of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_game_data_lua_item_base_sname
static int tolua_set_game_data_lua_item_base_sname(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->sname[tolua_index] = ((std::string)  tolua_tocppstring(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: scomment of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_game_data_lua_item_base_scomment
static int tolua_get_game_data_lua_item_base_scomment(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushcppstring(tolua_S,(const char*)self->scomment[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: scomment of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_game_data_lua_item_base_scomment
static int tolua_set_game_data_lua_item_base_scomment(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->scomment[tolua_index] = ((std::string)  tolua_tocppstring(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: sprice of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_game_data_lua_item_base_sprice
static int tolua_get_game_data_lua_item_base_sprice(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->sprice[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: sprice of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_game_data_lua_item_base_sprice
static int tolua_set_game_data_lua_item_base_sprice(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->sprice[tolua_index] = ((double)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: spoint of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_game_data_lua_item_base_spoint
static int tolua_get_game_data_lua_item_base_spoint(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->spoint[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: spoint of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_game_data_lua_item_base_spoint
static int tolua_set_game_data_lua_item_base_spoint(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->spoint[tolua_index] = ((float)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: snum of class  item_base */
#ifndef TOLUA_DISABLE_tolua_get_game_data_lua_item_base_snum
static int tolua_get_game_data_lua_item_base_snum(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
 tolua_pushnumber(tolua_S,(lua_Number)self->snum[tolua_index]);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: snum of class  item_base */
#ifndef TOLUA_DISABLE_tolua_set_game_data_lua_item_base_snum
static int tolua_set_game_data_lua_item_base_snum(lua_State* tolua_S)
{
 int tolua_index;
  item_base* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (item_base*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=3)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->snum[tolua_index] = ((long long)  tolua_tonumber(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: comment of class  item_skill */
#ifndef TOLUA_DISABLE_tolua_get_item_skill_comment
static int tolua_get_item_skill_comment(lua_State* tolua_S)
{
  item_skill* self = (item_skill*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'comment'",NULL);
#endif
  tolua_pushcppstring(tolua_S,(const char*)self->comment);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: comment of class  item_skill */
#ifndef TOLUA_DISABLE_tolua_set_item_skill_comment
static int tolua_set_item_skill_comment(lua_State* tolua_S)
{
  item_skill* self = (item_skill*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'comment'",NULL);
  if (!tolua_iscppstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->comment = ((std::string)  tolua_tocppstring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_game_data_lua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"game_data","game_data","",NULL);
  tolua_beginmodule(tolua_S,"game_data");
   tolua_variable(tolua_S,"id",tolua_get_game_data_id,tolua_set_game_data_id);
   tolua_variable(tolua_S,"name",tolua_get_game_data_name,tolua_set_game_data_name);
   tolua_variable(tolua_S,"key",tolua_get_game_data_key,tolua_set_game_data_key);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"item_base","item_base","game_data",NULL);
  tolua_beginmodule(tolua_S,"item_base");
   tolua_variable(tolua_S,"comment",tolua_get_item_base_comment,tolua_set_item_base_comment);
   tolua_variable(tolua_S,"price",tolua_get_item_base_price,tolua_set_item_base_price);
   tolua_variable(tolua_S,"point",tolua_get_item_base_point,tolua_set_item_base_point);
   tolua_variable(tolua_S,"num",tolua_get_item_base_num,tolua_set_item_base_num);
   tolua_array(tolua_S,"sid",tolua_get_game_data_lua_item_base_sid,tolua_set_game_data_lua_item_base_sid);
   tolua_array(tolua_S,"sname",tolua_get_game_data_lua_item_base_sname,tolua_set_game_data_lua_item_base_sname);
   tolua_array(tolua_S,"scomment",tolua_get_game_data_lua_item_base_scomment,tolua_set_game_data_lua_item_base_scomment);
   tolua_array(tolua_S,"sprice",tolua_get_game_data_lua_item_base_sprice,tolua_set_game_data_lua_item_base_sprice);
   tolua_array(tolua_S,"spoint",tolua_get_game_data_lua_item_base_spoint,tolua_set_game_data_lua_item_base_spoint);
   tolua_array(tolua_S,"snum",tolua_get_game_data_lua_item_base_snum,tolua_set_game_data_lua_item_base_snum);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"item_skill","item_skill","game_data",NULL);
  tolua_beginmodule(tolua_S,"item_skill");
   tolua_variable(tolua_S,"comment",tolua_get_item_skill_comment,tolua_set_item_skill_comment);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_game_data_lua (lua_State* tolua_S) {
 return tolua_game_data_lua_open(tolua_S);
};
#endif

