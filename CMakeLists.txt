cmake_minimum_required(VERSION 3.26)
project(dg_lua C)

set(CMAKE_C_STANDARD 11)

add_executable(dg_lua main.c
        common/lua.h
        common/lua-object.c
        common/lua-object.h
        common/lua-state.c
        common/lua-state.h
        vm/lua-gc.c
        vm/lua-gc.h
        common/lua-mem.c
        common/lua-mem.h
        common/lua-string.c
        common/lua-string.h
        vm/lua-vm.c
        vm/lua-vm.h
        common/lua-table.c
        common/lua-table.h
        vm/lua-do.c
        vm/lua-do.h)
