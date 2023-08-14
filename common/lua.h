#ifndef DG_LUA_LUA_H
#define DG_LUA_LUA_H

#include <stdarg.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <math.h>

#if defined(LLONG_MAX)
#define LUA_INTEGER long /* lua 整型 */
#define LUA_NUMBER double /* lua 浮点数类型 */
#else
#define LUA_INTEGER int
#define LUA_NUMBER float
#endif

#define LUA_UNSIGNED unsigned LUA_INTEGER /* lua 无符号整型 */
#define lua_assert(c) ((void)0) /* lua 断言 */
#define check_exp(c, e) (lua_assert(c),e) /* lua 错误检查 */

// 错误码
#define LUA_OK 0 /* lua 执行成功 */
#define LUA_ERR_ERR 1 /* lua 通用错误 */
#define LUA_ERR_MEM 1 /* lua 内存错误 */
#define LUA_ERR_RUN 1 /* lua 运行时错误 */

/**
 * 类型转换
 * @param t 目标类型
 * @param s 源数据
 */
#define cast(t, exp) ((t)(exp))
/**
 * 计算o在L栈指针中的位置距离
 * @param L 状态机
 * @param o 查找目标
 */
#define save_stack(L, o) ((o) - (L)->stack)
/**
 * 根据o恢复L栈指针位置
 * @param L 状态机
 * @param o 目标距离
 */
#define restore_stack(L, o) ((L)->stack+(o))
/**
 * 将指针p转换为uint类型
 * @param p 指针数据
 */
#define point2uint(p) ((unsigned int)((size_t)p & UINT_MAX))
/**
 * 通过提取tt_低四位确认o的类型
 * @param o 目标数据
 */
#define no_variant(o) ((o)->tt_ & 0xf)


#define LUA_T_NUMBER 1 /* lua tt_ 整型 */
#define LUA_T_LIGHT_USERDATA 2 /* lua tt_ 轻用户数据 */
#define LUA_T_BOOLEAN 3 /* lua tt_ 布尔值 */
#define LUA_T_STRING 4 /* lua tt_ 字符串 */
#define LUA_T_NIL 5 /* lua tt_ nil类型 */
#define LUA_T_TABLE 6 /* lua tt_ 表类型 */
#define LUA_T_FUNCTION 7 /* lua tt_ 函数类型 */
#define LUA_T_THREAD 8 /* lua tt_ 线程类型 */
#define LUA_T_NONE 9 /* lua tt_ 无效或者未初始化对象 */
#define LUA_NUMS LUA_T_NONE /* lua tt_ 数据类型数量 */
#define LUA_T_DEAD_KEY (LUA_NUMS + 1) /* lua tt_ 死键 */

// stack 定义

#define LUA_MIN_STACK 20 /* lua 最小栈大小 */
#define LUA_STACK_SIZE (2 * LUA_MIN_STACK) /* lua 可用栈大小 */
#define LUA_EXTRA_STACK 5 /* 额外栈空间,提供缓存 */
#define LUA_MAX_STACK 15000 /* lua 最大栈大小 */
#define LUA_ERROR_STACK 200 /* lua 错误栈大小 */
#define LUA_MUL_RET (-1) /* 如果返回值为LUA_MUL_RET则根据期望来确定实际的返回值数量 */
#define LUA_MAX_CALLS 200 /* lua 函数调用嵌套深度 */

/**
 * 错误信息打印
 * @param L 状态机
 * @param s 错误信息
 */
#define LUA_ERROR(L, s) printf("LUA ERROR:%s",s);

typedef size_t LuMem; /* lua 内存定义 */
typedef ptrdiff_t LMem; /* lua 内存之间距离定义 */
typedef LUA_UNSIGNED LuaUnsigned; /* lua 无符号整数 */

#define MAX_LU_MEM ((lu_mem)(~(lu_mem)0)) /* lua 最大无符号整数值 */
#define MAX_L_MEM (MAX_LU_MEM >> 1) /* lua 最大内存 */

#endif //DG_LUA_LUA_H
