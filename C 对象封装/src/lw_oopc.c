// Copyright (C) 2008,2009,2010 by Tom Kao & MISOO Team & Yonghua Jin. All rights reserved.
// Released under the terms of the GNU Library or Lesser General Public License (LGPL).
// Author: Tom Kao(蒽懚𦍑羶瞿繙繡盖酵䁱)瞿竅MISOO㮼繞䆐竅Yonghua Jin(蒽懚𦍑羶瞿繙翻簸𥖄罈穠)
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

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "lw_oopc.h"

#define lw_oopc_dbginfo printf

// 描述对象的字符串只能有 20 个字符
#define LW_OOPC_MAX_PATH   20

typedef struct LW_OOPC_MemAllocUnit
{
    char describe[LW_OOPC_MAX_PATH];
    void* addr;                   
    int size;
    struct LW_OOPC_MemAllocUnit* next;     
} LW_OOPC_MemAllocUnit;

static LW_OOPC_MemAllocUnit* lw_oopc_memAllocList = 0;

void* lw_oopc_malloc(int size, const char* type, const char* describe)
{
    void* addr = malloc(size);
    if (addr != 0)
    {
        LW_OOPC_MemAllocUnit* pMemAllocUnit = malloc(sizeof(LW_OOPC_MemAllocUnit));
        if (!pMemAllocUnit)
        {
            lw_oopc_dbginfo("lw_oopc: error! malloc alloc unit failed.\n");
        }

        strcpy(pMemAllocUnit->describe, describe);

        pMemAllocUnit->addr = addr;
        pMemAllocUnit->size = size;
        pMemAllocUnit->next = lw_oopc_memAllocList;
        lw_oopc_memAllocList = pMemAllocUnit;

        lw_oopc_dbginfo("lw_oopc: alloc memory in %p, size: %d, object type: %s, describe: %s\n", addr, size, type, describe);
    }

    return addr;
}

void lw_oopc_free(void* memblock)
{
    LW_OOPC_MemAllocUnit* prevUnit = 0;
    LW_OOPC_MemAllocUnit* currUnit = lw_oopc_memAllocList;

    while(currUnit != 0)
    {
        if (currUnit->addr == memblock)
        {
            lw_oopc_dbginfo("lw_oopc: free memory in %p, size: %d\n", currUnit->addr, currUnit->size);
            if (prevUnit == 0)
            {
                lw_oopc_memAllocList = currUnit->next;
                free(currUnit->addr);
                return;
            }
            
            prevUnit->next = currUnit->next;
            return;
        }
        else
        {
            prevUnit = currUnit;
            currUnit = currUnit->next;
        }
    }

    if (currUnit == 0)
    {
        lw_oopc_dbginfo("lw_oopc: error! you attemp to free invalid memory.\n");
    }
}

void lw_oopc_report()
{
    
    LW_OOPC_MemAllocUnit* currUnit = lw_oopc_memAllocList;

    if (currUnit != 0)
    {
        lw_oopc_dbginfo("lw_oopc: memory leak:\n");

        while(currUnit != 0)
        {
            lw_oopc_dbginfo("memory leak in: %p, size: %d, describe: %s\n", currUnit->addr, currUnit->size, currUnit->describe);
            currUnit = currUnit->next;
        }
    }
    else
    {
        printf("lw_oopc: no memory leak.\n");
    }
}
