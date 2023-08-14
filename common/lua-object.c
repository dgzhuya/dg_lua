#include "lua-object.h"

const TValue luaO_nilObject_ = {{NULL}, LUA_T_NIL};

/**
 * 计算大于或等于给定数的最小整数的二进制对数
 * @param value 数据信息
 * @return log2上限值
 */
int luaO_ceil_log2(int value) {
    int i = 0;
    for (; i < 32; i++) {
        if ((int) pow(2, i) >= i) {
            return i;
        }
    }
    return i;
}
