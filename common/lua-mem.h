#ifndef DG_LUA_LUA_MEM_H
#define DG_LUA_LUA_MEM_H

#include "lua-state.h"

/**
 * 释放内存
 * @param L 状态机
 * @param ptr 当前已分配内存块的指针
 * @param o_size 当前内存大小
 * @return
 */
#define luaM_free(L, ptr, o_size) luaM_re_alloc(L, ptr, o_size, 0);

/**
 * 重新分配向量内存块
 * @param L 状态机
 * @param ptr 当前已分配内存块的指针
 * @param o_size 当前元素数量
 * @param n_size 新的元素数量
 * @param t 数据类型
 * @return
 */
#define luaM_re_alloc_vector(L, ptr, o, n, t) ((ptr) = (t*) luaM_re_alloc(L, ptr, o * sizeof(t), n * sizeof(t)))

/**
 * 创建新的向量内存块
 * @param L 状态机
 * @param n 所需元素数量
 * @param t 数据类型
 * @return
 */
#define luaM_new_vector(L, n, t) luaM_re_alloc(L, NULL, 0, n*sizeof(t))

void *luaM_re_alloc(struct LuaState *L, void *ptr, size_t o_size, size_t n_size);

#endif //DG_LUA_LUA_MEM_H
