#include "lua-state.h"
#include "../vm/lua-gc.h"

typedef struct LX {
    LuByte extra_[LUA_EXTRA_SPACE];
    LuaState l;
} LX;

typedef struct LG {
    LX l;
    GlobalState g;
} LG;

struct LuaState *lua_new_state(LuaAlloc alloc, void *ud) {
    struct GlobalState *g;
    struct LuaState *L;
    
    struct LG *lg = (struct LG *) (*alloc)(ud, NULL, LUA_T_THREAD, sizeof(LG));
    if (!lg) {
        return NULL;
    }
    g = &lg->g;
    g->ud = ud;
    g->free_alloc = alloc;
    g->panic = NULL;
    
    L = &lg->l.l;
    L->tt_ = LUA_T_THREAD;
    L->n_ci = 0;
    G(L) = g;
    g->main_thread = L;
    
    // gc init
    
    return L;
}

void lua_close(struct LuaState *L) {
    struct GlobalState *g = G(L);
    struct LuaState *l1 = g->main_thread;
    
    luaC_free_all_objects(L);
}

void set_i_value(StkId target, int integer) {
    target->value_.i = integer;
    target->tt_ = LUA_NUM_INT;
}

void set_f_value(StkId target, LuaCFunction f) {
    target->value_.f = f;
    target->tt_ = LUA_T_FUNCTION;
}

void set_fl_value(StkId target, float number) {
    target->value_.n = number;
    target->tt_ = LUA_NUM_FLT;
}

void set_b_value(StkId target, bool b) {
    target->value_.b = b;
    target->tt_ = LUA_T_BOOLEAN;
}

void set_nil_value(StkId target) {
    target->tt_ = LUA_T_NIL;
}

/**
 * 设置用户数据
 * @param target 目标对象
 * @param p 轻用户数据
 */
void set_p_value(StkId target, void *p) {
    target->value_.p = p;
    target->tt_ = LUA_T_LIGHT_USERDATA;
}

void set_gco(StkId target, struct GCObject *gco) {
    target->value_.gc = gco;
    target->tt_ = gco->tt_;
}

/**
 * 复制值对象
 * @param target 目标对象
 * @param value 源数据
 */
void set_obj(StkId target, StkId value) {
    target->value_ = value->value_;
    target->tt_ = value->tt_;
}

/**
 * 增加top位置
 * @param L 状态机
 */
void increase_top(struct LuaState *L) {
    L->top++;
    assert(L->top <= L->stack_last);
}

/**
 * 入栈函数
 * @param L 状态机
 * @param f 函数
 */
void lua_push_c_function(struct LuaState *L, LuaCFunction f) {
    set_f_value(L->top, f);
    increase_top(L);
}

/**
 * 入栈整数
 * @param L 状态机
 * @param integer 值
 */
void lua_push_integer(struct LuaState *L, int integer) {
    set_i_value(L->top, integer);
    increase_top(L);
}

void lua_push_number(struct LuaState *L, float number) {
    set_fl_value(L->top, number);
    increase_top(L);
}

void lua_push_boolean(struct LuaState *L, bool b) {
    set_b_value(L->top, b);
    increase_top(L);
}

void lua_push_nil(struct LuaState *L) {
    set_nil_value(L->top);
    increase_top(L);
}

void lua_push_light_userdata(struct LuaState *L, void *p) {
    set_p_value(L->top, p);
    increase_top(L);
}

void lua_push_string(struct LuaState *L, const char *str) {
    unsigned int l = strlen(str);
    // struct TString *ts=
    increase_top(L);
}

int lua_create_table(struct LuaState *L) {
    return 1;
}

int lua_set_table(struct LuaState *L, int idx) {
    return 1;
}

int lua_get_table(struct LuaState *L, int idx) {
    return 1;
}

int lua_get_global(struct LuaState *L) {
    return 1;
}

/**
 * 将索引位置值转换为整数
 * @param L 状态机
 * @param idx 位置
 * @param is_num 是否为字符串
 * @return 转换结果
 */
LuaInteger lua_to_integer_x(struct LuaState *L, int idx, int *is_num) {
    LuaInteger ret = 0;
    TValue *addr = index2addr(L, idx);
    if (addr->tt_ == LUA_NUM_INT) {
        *is_num = 1;
        ret = addr->value_.n;
    } else {
        *is_num = 0;
        LUA_ERROR(L, "can not convert to integer!\n")
    }
    return ret;
}

/**
 * lua value 转换为整数
 * @param L 状态机
 * @param idx 索引位置
 * @param is_num 转换状态
 * @return 转换结果
 */
LuaNumber lua_to_number_x(struct LuaState *L, int idx, int *is_num) {
    LuaNumber ret = 0.0f;
    TValue *addr = index2addr(L, idx);
    if (addr->tt_ == LUA_NUM_FLT) {
        *is_num = 1;
        ret = addr->value_.n;
    } else {
        *is_num = 0;
        LUA_ERROR(L, "can not convert to number!\n")
    }
    return ret;
}

/**
 * lua value 转换为布尔值
 * @param L 状态机
 * @param idx 索引位置
 * @return 转换结果
 */
bool lua_to_boolean(struct LuaState *L, int idx) {
    TValue *addr = index2addr(L, idx);
    return !(addr->tt_ == LUA_T_NIL || addr->value_.b == 0);
}

/**
 * 设置idx位置值为nil
 * @param L 状态机
 * @param idx 目标位置
 * @return 执行结果
 */
int lua_is_nil(struct LuaState *L, int idx) {
    TValue *addr = index2addr(L, idx);
    return addr->tt_ == LUA_T_NIL;
}

/**
 * lua value 转换为字符串
 * @param L 状态机
 * @param idx 位置
 * @return 转换结果
 */
char *lua_to_string(struct LuaState *L, int idx) {
    TValue *addr = index2addr(L, idx);
    if (no_variant(addr) != LUA_T_STRING) {
        return NULL;
    }
    return "111";
}

/**
 * 设置栈顶信息
 * idx大于0小于stack_last则直接修改top位置,中间填充nil
 * idx小于0大于ci->func-top的结果则直接修改top位置
 * @param L 状态机
 * @param idx 位置信息
 * @return
 */
void lua_set_top(struct LuaState *L, int idx) {
    StkId func = L->ci->func;
    if (idx >= 0) {
        assert(idx <= L->stack_last - (func + 1));
        while (L->top < (func + 1) + idx) {
            set_nil_value(L->top++);
        }
        L->top = func + 1 + idx;
    } else {
        assert(L->top + idx > func);
        L->top = L->top + idx;
    }
}

int lua_get_top(struct LuaState *L) {
    return cast(int, L->top - (L->ci->func + 1));
}

/**
 * 状态机出栈
 * @param L 状态机
 */
void lua_pop(struct LuaState *L) {
    lua_set_top(L, -1);
}

/**
 * 通过索引查找地址
 * 索引大于0小于top从ci->func位置开始
 * 索引小于0从top位置向下查找
 * @param L 状态机
 * @param idx 索引位置
 * @return 返回结果
 */
TValue *index2addr(struct LuaState *L, int idx) {
    if (idx >= 0) {
        assert(L->ci->func + idx < L->ci->top);
        return L->ci->func + idx;
    } else {
        assert(L->top + idx > L->ci->func);
        return L->top + idx;
    }
}