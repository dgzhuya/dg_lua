#ifndef DG_LUA_LUA_STATE_H
#define DG_LUA_LUA_STATE_H

#include "lua-object.h"

#define LUA_EXTRA_SPACE sizeof(void*) /* 额外空间大小 */
#define G(L) ((L)->l_G) /* 从状态机L获取全局状态G */

#define STEP_MUL_ADJ 200 /* GC自适应模式下步进倍数 */
#define GC_STEP_MUL 200 /* GC单步步进倍数 */
#define GC_STEP_SIZE 1024 /* 单步gc内存回收大小 */
#define GC_PAUSE 100 /* GC暂停百分比 */

#define STR_CACHE_M 53 /* 字符串缓存M */
#define STR_CACHE_N 2 /* 字符串缓存N */

#define LUA_MAIN_THREAD_IDX 0 /* 注册表中主线程索引 */
#define LUA_GLOBAL_TBL_IDX 1 /* 注册表中全局环境索引 */

typedef TValue *StkId; /* TValue 别名 */

// 函数调用结构体
struct CallInfo {
    StkId func; /* 函数的栈位置，指向函数对象 */
    StkId top; /* 函数栈帧的栈顶 */
    int n_result; /* 期望返回值的数量 */
    int call_status; /* 调用状态 */
    struct CallInfo *next;  /* 前一个信息指针 */
    struct CallInfo *previous; /* 后一个调用信息指针 */
};

// lua 状态结构体
typedef struct LuaState {
    CommonHeader;
    StkId stack; /* 栈起始位置 */
    StkId stack_last; /* 栈最后位置 */
    StkId top; /* 栈顶指针 */
    int stack_size; /* 栈大小 */
    struct LuaLongJmp *error_jmp; /* 错误跳转 */
    int status; /* 状态 */
    struct LuaState *previous; /* 前一个状态机 */
    struct CallInfo base_ci; /* 主调用信息 */
    struct CallInfo *ci; /* 调用信息 */
    int n_ci; /* 函数调用数量 */
    struct GlobalState *l_G; /* 全局状态 */
    ptrdiff_t error_func; /* 错误处理函数 */
    int n_calls; /* C函数调用计数 */
    struct GCObject *gc_list; /* 垃圾回收链表 */
} LuaState;

// 短字符串表,存储和管理字符串
struct StringTable {
    struct TString **hash; /* 指向指针数组 */
    unsigned int n_use; /* 字符串表中已使用的项数 */
    unsigned int size; /* 字符串表的大小 */
};

// 全局GC状态
typedef struct GlobalState {
    struct LuaState *main_thread; /* 主线程 */
    LuaAlloc free_alloc; /* 内存函数分配指针 */
    void *ud; /* 用户自定义数据 */
    LuaCFunction panic; /* lua 错误处理函数 */
    
    struct StringTable str_tbl; /* String 表信息 */
    TString *str_cache[STR_CACHE_M][STR_CACHE_N];  /* 字符串缓存位置 */
    unsigned int seed;  /* 随机数种子 */
    TString *mem_err_msg;  /* 错误信息 */
    TValue l_registry;  /* 注册表 */
    
    LuByte gc_state;  /* 垃圾回收状态 */
    LuByte current_white;  /* 当前白色标记 */
    struct GCObject *all_gc; /* 所有垃圾回收对象链表 */
    struct GCObject **sweep_gc; /* 正在被清扫的垃圾回收对象链表 */
    struct GCObject *gray; /* 灰色对象链表 */
    struct GCObject *gray_again; /* 再次进入灰色对象链表 */
    struct GCObject *fix_gc; /* 固定内存块的对象链表 */
    LuMem total_bytes; /* 已分配的内存总数 */
    LMem gc_debt; /* 垃圾回收积累的债务 */
    LuMem gc_mem_traverse;  /* gc 内存遍历 */
    LuMem gc_estimate; /* gc 内存预估 */
    int gc_step_mul; /* 垃圾回收步进倍数 */
} GlobalState;

// 不同垃圾回收对象类型之间共用内存空间
union GCUnion {
    struct GCObject gc; /* 垃圾回收对象通用头部 */
    LuaState th; /* 线程类型垃圾回收对象 */
    TString ts; /* 字符串类型垃圾回收对象 */
    struct Table tbl; /* 表类型垃圾回收对象 */
};

struct LuaState *lua_new_state(LuaAlloc alloc, void *ud);

void lua_close(struct LuaState *L);

void set_i_value(StkId target, int integer);

void set_f_value(StkId target, LuaCFunction f);

void set_fl_value(StkId target, float number);

void set_b_value(StkId target, bool b);

void set_nil_value(StkId target);

void set_p_value(StkId target, void *p);

void set_gco(StkId target, struct GCObject *gco);

void set_obj(StkId target, StkId value);

void increase_top(struct LuaState *L);

void lua_push_c_function(struct LuaState *L, LuaCFunction f);

void lua_push_integer(struct LuaState *L, int integer);

void lua_push_number(struct LuaState *L, float number);

void lua_push_boolean(struct LuaState *L, bool b);

void lua_push_nil(struct LuaState *L);

void lua_push_light_userdata(struct LuaState *L, void *p);

void lua_push_string(struct LuaState *L, const char *str);

int lua_create_table(struct LuaState *L);

int lua_set_table(struct LuaState *L, int idx);

int lua_get_table(struct LuaState *L, int idx);

int lua_get_global(struct LuaState *L);

LuaInteger lua_to_integer_x(struct LuaState *L, int idx, int *is_num);

LuaNumber lua_to_number_x(struct LuaState *L, int idx, int *is_num);

bool lua_to_boolean(struct LuaState *L, int idx);

int lua_is_nil(struct LuaState *L, int idx);

char *lua_to_string(struct LuaState *L, int idx);

void lua_set_top(struct LuaState *L, int idx);

int lua_get_top(struct LuaState *L);

void lua_pop(struct LuaState *L);

TValue *index2addr(struct LuaState *L, int idx);

#endif //DG_LUA_LUA_STATE_H
