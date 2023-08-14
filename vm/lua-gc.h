#ifndef DG_LUA_LUA_GC_H
#define DG_LUA_LUA_GC_H
#include "../common/lua-state.h"

void luaC_free_all_objects(struct LuaState *L);

#endif //DG_LUA_LUA_GC_H
