#ifndef DG_LUA_LUA_OBJECT_H
#define DG_LUA_LUA_OBJECT_H

#include "lua.h"

typedef struct LuaState LuaState; /* lua 状态机类型 */
typedef LUA_INTEGER LuaInteger; /* lua 整数类型 */
typedef LUA_NUMBER LuaNumber; /* lua 浮点数类型 */
typedef unsigned char LuByte; /* lua 字节类型 */

/**
 * lua c类型函数
 * @param L 状态机
 * @return 操作码
 */
typedef int (*LuaCFunction)(LuaState *L); /* lua c函数类型 */

/**
 * lua 内存分配函数类型
 * @param ud userdata 用户数据
 * @param *ptr 内存指向地址
 * @param o_size 旧内存大小
 * @param n_size 新的内存大小
 */
typedef void *(*LuaAlloc)(void *ud, void *ptr, size_t o_size, size_t n_size);

#define LUA_NUM_INT (LUA_T_NUMBER | (0 << 4)) /* lua 整数类型tt_数值 */
#define LUA_NUM_FLT (LUA_T_NUMBER | (1 << 4)) /* lua 浮点数类型tt_数值 */

#define LUA_T_LCL (LUA_T_FUNCTION | (0 << 4)) /* lua 函数闭包 */
#define LUA_T_LCF (LUA_T_FUNCTION | (1 << 4)) /* C语言闭包函数 */
#define LUA_T_CCL (LUA_T_FUNCTION | (2 << 4)) /* 轻型C语言函数 */

#define LUA_LNG_STR (LUA_T_STRING | (0 << 4)) /* lua 长字符串 */
#define LUA_SHR_STR (LUA_T_STRING | (1 << 4)) /* lua 短字符串 */
#define MAX_SHORT_STR 40 /* 最长短字符串长度 */

/**
 * @param next 下一个gc节点
 * @param tt_ gc类型
 * @param marked 当前gc状态
 */
#define CommonHeader struct GCObject* next; LuByte tt_; LuByte marked
#define LUA_GC_STEP_MUL 200 /* 垃圾回收步进倍数 */

#define luaO_nilObject (&luaO_nilObject_) /* lua 空对象 */

#define dummy_node (&dummy_node_)  /* 空节点 */
/**
 * 计算2的幂次方
 * @param l_size 次方大小
 */
#define two_to(l_size) (1 << l_size)
/**
 * 通过hash计算表node部分中的位置
 * @param hash hash数值
 * @param size table node部分大小
 */
#define l_mod(hash, size) (check_exp((size) & (size - 1) == 0), (hash) & (size - 1))
/**
 * lua 数字比较大小
 * @param a 参数1
 * @param b 参数2
 */
#define lua_num_eq(a, b) ((a)==(b))
/**
 * 确认数字是否合法
 * @param a 数字
 */
#define lua_num_isnan(a) (!lua_num_eq(a, a))
/**
 * 浮点数转整数
 * @param n 浮点数
 * @param p 整数指针
 */
#define lua_number_to_integer(n, p) \
    (n >= cast(LuaNumber,INT_MIN)) && \
    (n <= cast(LuaNumber,INT_MAX)) && \
    ((*p = cast(LuaInteger,n)),1)

#define tt_is_integer(o) ((o)->tt_ == LUA_NUM_INT)
#define tt_is_number(o) ((o)->tt_ == LUA_NUM_FLT)
#define tt_is_shr_str(o) ((o)->tt_ == LUA_SHR_STR)
#define tt_is_lng_str(o) ((o)->tt_ == LUA_LNG_STR)
#define tt_is_dead_key(o) ((o)->tt_ == LUA_T_DEAD_KEY)
#define tt_is_table(o) ((o)->tt_ == LUA_T_TABLE)
#define tt_is_nil(o) ((o)->tt_ == LUA_T_NIL)

// gc对象
struct GCObject {
    CommonHeader;
};

typedef union LuaValue {
    struct GCObject *gc; /* gc对象 */
    void *p; /* 指针类型 */
    int b; /* 布尔值类型 */
    LuaInteger i; /* 整数 */
    LuaNumber n; /* 浮点数类型 */
    LuaCFunction f; /* lua 函数类型 */
} Value;

typedef struct LuaTValue {
    Value value_; /* 值 */
    int tt_; /* 类型值 */
} TValue;

// lua 空对象
extern const TValue luaO_nilObject_;

// lua 字符串类型
typedef struct TString {
    CommonHeader;
    unsigned int hash; /* String Body经过hash计算后所得到的值 */
    /**
     * TString为长字符串时:
     *     当extra=0表示该字符串未进行hash运算
     *     当extra=1时表示该字符串已经进行过hash运算
     * TString为短字符串时:
     *     当extra=0时表示它需要被gc托管
     *     当extra=1时表示该字符串不会被gc回收
     */
    unsigned short extra;
    unsigned short shr_len; /* 短字符串的长度 */
    union {
        struct TString *h_next; /* 将冲突的TString实例链接成单向链表 */
        size_t lng_len; /* 长字符串的长度 */
    } u; /* 短字符串h_next有效,长字符串lng_len有效 */
    char data[0]; /* String Body起始位置 */
} TString;

// lua table key 类型
typedef union TKey {
    struct {
        Value value_;
        int tt_;
        int next; /* hash冲突下面下一个key位置指向 */
    } nk;
    TValue tvk;
} TKey;

// lua hash 节点信息
typedef struct Node {
    TKey key;
    TValue value;
} Node;

const Node dummy_node_;

// lua 表定义
struct Table {
    CommonHeader;
    TValue *array; /* 数组部分 */
    unsigned int array_size; /* table 数组大小 */
    Node *node; /* 哈希部分 */
    unsigned int l_size_node; /* 节点数组大小 */
    Node last_free; /* 哈希部分中最后一个未使用的节点 */
    struct GCObject *gc_list; /* 垃圾回收链表 */
};

int luaO_ceil_log2(int value);

#endif //DG_LUA_LUA_OBJECT_H
