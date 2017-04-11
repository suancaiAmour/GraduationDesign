// 作者: 高焕堂 金永华 酸菜Amour

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


// 根类
ABS_CTOR(rootClass)
END_ABS_CTOR
