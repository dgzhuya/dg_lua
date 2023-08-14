#include "lua-mem.h"
#include "../vm/lua-do.h"

/**
 * 重新分配内存块
 * @param L 状态机
 * @param ptr 当前已分配内存块的指针
 * @param o_size 当前内存块大小
 * @param n_size 新的内存块大小
 * @return
 */
void *luaM_re_alloc(struct LuaState *L, void *ptr, size_t o_size, size_t n_size) {
    struct GlobalState *g = G(L);
    int old_size = ptr ? o_size : 0;
    
    void *ret = (*g->free_alloc)(g->ud, ptr, old_size, n_size);
    
    if (ret == NULL && n_size > 0) {
        luaD_throw(L, LUA_ERR_MEM);
    }
    g->gc_debt = g->gc_debt - old_size + n_size;
    return ret;
}
