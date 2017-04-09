// Copyright (C) 2008,2009,2010 by Tom Kao & MISOO Team & Yonghua Jin. All rights reserved.
// Released under the terms of the GNU Library or Lesser General Public License (LGPL).
// Author: Tom Kao(中文名：高焕堂)，MISOO团队，Yonghua Jin(中文名：金永华)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//    * Redistributions of source code must retain the above copyright
//	notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
//	copyright notice, this list of conditions and the following disclaimer
//	in the documentation and/or other materials provided with the
//	distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef LW_OOPC_H_INCLUDED_
#define LW_OOPC_H_INCLUDED_

// 配置宏(两种配置选其一):
// LW_OOPC_USE_STDDEF_OFFSETOF          表示使用C标准定义的offsetof
// LW_OOPC_USE_USER_DEFINED_OFFSETOF    表示使用用户自定义的lw_oopc_offsetof宏
#define LW_OOPC_USE_STDDEF_OFFSETOF
//#define LW_OOPC_USE_USER_DEFINED_OFFSETOF

#ifdef LW_OOPC_USE_STDDEF_OFFSETOF
#include <stddef.h>
#define LW_OOPC_OFFSETOF offsetof
#endif

#ifdef LW_OOPC_USE_USER_DEFINED_OFFSETOF
// 有些环境可能不支持，不过，这种情形极少出现
#define LW_OOPC_OFFSETOF(s,m) (size_t)&(((s*)0)->m)
#endif

// 自定义的内存分配函数
void* lw_oopc_malloc(size_t size, const char* type, const char* file, int line);
// 自定义的内存释放函数
void lw_oopc_free(void* memblock);
// 检测是否内存泄露
void lw_oopc_report();

// 用来调试 找到内存泄露的地方 要求使用者输入
#define lw_oopc_file_line_params const char* file, int line

// 接口
#define INTERFACE(type)             \
typedef struct type type;           \
void type##_ctor(type* t);          \
int type##_dtor(type* t);           \
struct type

// 抽象类 不能创建对象
#define ABS_CLASS(type)             \
typedef struct type type;           \
void type##_ctor(type* t);          \
int type##_dtor(type* t);           \
void type##_release(type* t);       \
void type##_retain(type* t);        \
struct type                         \
{                                   

#define END_ABS_CLASS };

// 具体类 可以创建对象 添加引用计数
#define CLASS(type)                 \
typedef struct type type;           \
type* type##_new(lw_oopc_file_line_params); \
void type##_ctor(type* t);          \
int type##_dtor(type* t);           \
void type##_release(type* t);       \
void type##_retain(type *t);        \
struct type                         \
{                                   \
    int referenceCount;

#define END_CLASS  };

// 对类设置属性是函数的属性 并且设置引用计数初始是 1
// 使用 FUNCTION_SETTING 来创建内部对象的函数属性
#define CTOR(type)                                      \
    type* type##_new(const char* file, int line) {      \
    struct type *cthis;                                 \
    cthis = (struct type*)lw_oopc_malloc(sizeof(struct type), #type, file, line);   \
    cthis->referenceCount = 1;                          \
    if(!cthis)                                          \
    {                                                   \
        return 0;                                       \
    }                                                   \
    type##_ctor(cthis);                                 \
    return cthis;                                       \
}                                                       \
    void type##_release(type* cthis)                    \
    {                                                   \
        if(--cthis->referenceCount == 0)                \
        {                                               \
            type##_dtor(cthis);                          \
            lw_oopc_free(cthis);                        \
        }                                               \
    }                                                   \
    void type##_retain(type* cthis)                     \
    {                                                   \
        cthis->referenceCount++;                        \
    }                                                   \
void type##_ctor(type* cthis) {

// 由引用计数来判断是否释放 并且引用计数都减 1
#define END_CTOR	} \
    
 
// 使用 FUNCTION_RELEASE 来释放内部的对象
#define DTOR(type)                  \
void type##_dtor(type* cthis)       \
{

#define END_DTOR }

#define ABS_CTOR(type)              \
void type##_ctor(type* cthis) {

#define END_ABS_CTOR }

#define FUNCTION_SETTING(f1, f2)	cthis->f1 = f2;

#define FUNCTION_RELEASE(type, f2)  type##_release(f2);

#define IMPLEMENTS(type)	struct type type

#define EXTENDS(type)		struct type type

#define SUPER_PTR(cthis, father) ((father*)(&(cthis->##father)))

#define SUPER_PTR_2(cthis, father, grandfather) \
	SUPER_PTR(SUPER_PTR(cthis, father), grandfather)

#define SUPER_PTR_3(cthis, father, grandfather, greatgrandfather) \
	SUPER_PTR(SUPER_PTR_2(cthis, father, grandfather), greatgrandfather)

#define SUPER_CTOR(father) \
	father##_ctor(SUPER_PTR(cthis, father));

#define SUB_PTR(selfptr, self, child) \
	((child*)((char*)selfptr - LW_OOPC_OFFSETOF(child, self)))

#define SUB_PTR_2(selfptr, self, child, grandchild) \
	SUB_PTR(SUB_PTR(selfptr, self, child), child, grandchild)

#define SUB_PTR_3(selfptr, self, child, grandchild, greatgrandchild) \
	SUB_PTR(SUB_PTR_2(selfptr, self, child, grandchild), grandchild, greatgrandchild)

#define INHERIT_FROM(father, cthis, field)	cthis->father.field

#endif