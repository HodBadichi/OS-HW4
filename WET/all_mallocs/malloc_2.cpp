#include "malloc_2.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <stdio.h>

struct MallocMetaData
{
    
    size_t size;
    bool is_free;
    MallocMetaData* next;
    MallocMetaData* prev;
};

#define TAGS_SIZE sizeof(MallocMetaData)

static MallocMetaData* blocks_list_head{nullptr} ;
static MallocMetaData* blocks_list_tail{nullptr};
static size_t num_free_blocks{0},num_free_bytes{0},num_allocated_blocks{0},num_allocated_bytes{0},num_meta_data_bytes{0},size_meta_data{0};

 MallocMetaData* SearchFreeBlock(size_t block_size);
 void InsertNewBlock(MallocMetaData* new_block);

long long int pow(int base,int power)
{
    int res = 1;
    if (power == 0)
    {
        return 1;
    }

    for (int i=0;i<power;i++)
    {
        res=res*base;
    }
    return res;
}

MallocMetaData* SearchFreeBlock(size_t block_size)
{
    MallocMetaData* temp_head = blocks_list_head;
    while (temp_head != nullptr )
    {
        if (temp_head->is_free &&  temp_head->size > block_size)
        {
            break;
        }
        else
        {
            temp_head=temp_head->next;
        }
    }
    return temp_head;
}

void InsertNewBlock(MallocMetaData* new_block)
{
    MallocMetaData* temp_head = blocks_list_head;
    if (!blocks_list_head) // in case its first request
    {
        blocks_list_head=new_block;
        blocks_list_tail = new_block;
        new_block->next = nullptr;
        new_block->prev = nullptr;
        return;
    }

    if ( &new_block > &blocks_list_tail) // in case its the new tail
    {
        blocks_list_tail->next = new_block;
        new_block->prev = blocks_list_tail;
        new_block->next = nullptr;
        blocks_list_tail=new_block;
        return;
    }

    if (&new_block < &blocks_list_head) // in case its the new head
    {
        blocks_list_head->prev = new_block;
        new_block->next = blocks_list_head;
        new_block->prev = nullptr;
        blocks_list_head = new_block;
        return;
    }

    while(temp_head) // in case its a middle node and not head \ tail 
    {
        if (&new_block > &temp_head)
        {
            temp_head = temp_head->next;
        }
        else
        {
            new_block->next= temp_head;
            new_block->prev = temp_head->prev;
            temp_head->prev->next = new_block;
            break;
        }
    }
    return;
}
void* smalloc(size_t size)
{
    if (size <= 0 || size > pow(10,8))
        return nullptr;

    MallocMetaData* p_new_block = SearchFreeBlock(size);
    if (p_new_block == nullptr) // if there`s no free block use sbrk()
    {
        void* p  = sbrk(0);
        void* temp = sbrk(size+TAGS_SIZE);
        if (temp != (void*)-1)//  brk() success
        {
            p_new_block = (MallocMetaData*)p;
            p_new_block->is_free = 0;
            p_new_block->size = size;
            InsertNewBlock(p_new_block);
            num_allocated_blocks++;
            num_allocated_bytes+=size;
            return (p_new_block+1);
        }
        else // brk() failed
        {
            return nullptr;
        }
    } 
    else // in case there`s a matching free block
    {
        num_free_blocks--;
        num_free_bytes-=p_new_block->size;
        return (p_new_block+1);
    }
}

void sfree(void* p)
{
    if (p == nullptr)
    {
        return;
    }
    MallocMetaData* p_released_block = (MallocMetaData*) p;
    p_released_block = p_released_block-1;
    if (p_released_block->is_free == true)
    {
        return;
    }
    p_released_block->is_free = true;
    num_free_blocks++;
    num_free_bytes +=p_released_block->size;
    return;
}

void* scalloc(size_t num, size_t size)
{ 
    void* new_block = smalloc(size*num);
    if (new_block == nullptr)
        return nullptr;
    memset(new_block,0,size*num);
    return new_block;
}
void* srealloc(void* oldp, size_t size)
{
    if (oldp == nullptr)
    {
        return smalloc(size);
    }

    MallocMetaData* old_block = (MallocMetaData*)oldp;
    old_block = old_block -1;
    if (old_block->size >= size)
        return (old_block+1);
    void* new_block = smalloc(size);
    if (new_block == nullptr) // if finding / allocating failed return null
            return nullptr;
    size_t min = old_block->size > size ? size : old_block->size;
    memcpy(new_block,old_block+1,old_block->size);
    sfree(old_block+1);
    return new_block;
}

size_t _num_free_blocks()
{
    return num_free_blocks;
}
size_t _num_free_bytes()
{
    return num_free_bytes;
}
size_t _num_allocated_blocks()
{
    return num_allocated_blocks;
}
size_t _num_allocated_bytes()
{
    return num_allocated_bytes;
}
size_t _num_meta_data_bytes()
{
    return num_allocated_blocks*(_size_meta_data());
}
size_t _size_meta_data()
{
    return sizeof(MallocMetaData);
}