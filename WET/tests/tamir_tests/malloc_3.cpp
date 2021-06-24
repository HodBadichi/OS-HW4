#include "malloc_2.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <cassert> 
#include <sys/mman.h>

struct MallocMetaData
{
    size_t size;
    bool is_free;
    MallocMetaData* next;
    MallocMetaData* prev;
    MallocMetaData* next_inbin;
    MallocMetaData* prev_inbin;
};
typedef MallocMetaData* block_t;

#define TAGS_SIZE sizeof(MallocMetaData)
#define KB 1024
#define NUM_OF_BINS 128

static block_t blocks_list_head{nullptr} ;
static block_t blocks_list_tail{nullptr};
static block_t mmap_list_head{nullptr} ;
static block_t mmap_list_tail{nullptr};
static block_t bins_arr[NUM_OF_BINS] {nullptr};
static size_t num_free_blocks{0},num_free_bytes{0},num_allocated_blocks{0},num_allocated_bytes{0},num_meta_data_bytes{0},size_meta_data{0};

block_t reallocCaseD(block_t old_block, size_t size);
block_t reallocCaseC(block_t old_block,size_t size);
block_t reallocCaseB(block_t old_block,size_t size);
void MmapFree(block_t p);
void SbrkFree(block_t p_released_block);
block_t SearchFreeBlock(size_t block_size);
void InsertNewBlock(block_t new_block);
block_t SplitBlocks(block_t full_block,size_t size);
void* MmapSrealloc(block_t old_block,size_t size);
void* SbrkSrealloc(block_t old_block,size_t size);
void* SbrkAllocation(size_t size);
void* MmapAllocation(size_t size);
long long int pow(int base,int power);
void DeleteBinNode(block_t node,int bin_num);
void AddBinNode(block_t node,int bin_num);
bool isNextBlockFree(block_t block);
bool isPreBlockFree(block_t block);

block_t reallocCaseD(block_t old_block, size_t size)
{
            DeleteBinNode(old_block->prev,old_block->prev->size/KB);
            DeleteBinNode(old_block->next,old_block->next->size/KB);
            block_t res_block = old_block->prev;
            res_block->size +=old_block->next->size+old_block->size+2*TAGS_SIZE;
            memmove(res_block+1,old_block+1,old_block->size);
            num_allocated_bytes+=2 * TAGS_SIZE;
            num_allocated_blocks-=2;
            res_block->is_free = false;
             res_block = SplitBlocks(res_block,size);
             return res_block+1;
}
block_t reallocCaseC(block_t old_block,size_t size)
{
        DeleteBinNode(old_block->next,old_block->next->size/KB);
        block_t ret_block = old_block;                    
        ret_block->size+=old_block->next->size+TAGS_SIZE;
        if (old_block->next->next)
        {
            old_block->next->next->prev = ret_block;
        }
        ret_block->next = old_block->next->next;
        memmove(ret_block+1,old_block+1,size);
        num_allocated_bytes+=TAGS_SIZE;
        num_allocated_blocks--;
        ret_block = SplitBlocks(ret_block,size);
        return ret_block+1;
}

block_t reallocCaseB(block_t old_block,size_t size)
{
        DeleteBinNode(old_block->prev,old_block->prev->size/KB);
        block_t ret_block = old_block->prev;
        ret_block->size +=old_block->size+TAGS_SIZE;
        ret_block->is_free = false;
        ret_block->next = old_block->next;
        if (old_block->next)
        {
            old_block->next->prev = ret_block;
        }
        memmove(ret_block+1,old_block+1,size);
        num_allocated_bytes+=TAGS_SIZE;
        num_allocated_blocks--;
        ret_block = SplitBlocks(ret_block,size);
        return ret_block+1;
}

bool isNextBlockFree(block_t block)
{
    return (block->next && (block->next)->is_free);
}
bool isPreBlockFree(block_t block)
{
    return (block->prev && (block->prev)->is_free);
}
long long int pow(int base,int power)
{
    int res=1;
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

/*
    This function recieve two blocks and merge them togther
    */
block_t MergeAdjacent(block_t first_block,block_t second_block )
{
    DeleteBinNode(second_block,second_block->size/KB);
    DeleteBinNode(first_block,first_block->size/KB);
    first_block->size+=second_block->size+TAGS_SIZE;
    first_block->next=second_block->next;
if (second_block->next )
{
    second_block->next->prev=first_block;
}
    num_allocated_blocks--;
    num_allocated_bytes+=TAGS_SIZE;
    AddBinNode(first_block,first_block->size/KB);
    return first_block;
}

void DeleteBinNode(block_t node,int bin_num)
{
    bool prev_exists{0},next_exists{0};
    if (node->prev_inbin!=nullptr)
        prev_exists = true;
    if (node->next_inbin!=nullptr)
        next_exists = true;

    if (prev_exists && next_exists) // if the noded deleted is in the middle
    {
        node->prev_inbin->next_inbin=node->next_inbin;
        node->next_inbin->prev_inbin=node->prev_inbin;
    }
    else if(prev_exists && !next_exists) // if the node deleted is the tail 
    {
        node->prev_inbin->next_inbin=nullptr;
    }
    else if (!prev_exists && next_exists) // if the node deleted is the head
    {
        bins_arr[bin_num]=node->next_inbin;
        node->next_inbin->prev_inbin = nullptr;
    }
    else //( !prev_exists && !next_exists)
    {
        bins_arr[bin_num] = nullptr;
    }
    num_free_bytes-=node->size;
    num_free_blocks--;
    node->next_inbin = nullptr;
    node->prev_inbin = nullptr;
    return;

    
}

void AddBinNode(block_t node,int bin_num)
{
    block_t bin_head = bins_arr[bin_num];
    block_t prev; 
    if (bin_head == nullptr) // in case the bin is empty
    {
        bins_arr[bin_num]=node;
    }
    else
    {
        while(bin_head)
        {
            if (bin_head->size<node->size)
            {
                prev=bin_head;
                bin_head=bin_head->next_inbin;
                continue;
            }
            break;
            
        }
        if(bin_head == nullptr)
        {
            prev->next_inbin = node;
            node->prev_inbin =  prev;  
            node->next_inbin=nullptr;
        }
        else if (bin_head->prev_inbin == nullptr) // if its the new head
        {
            bins_arr[bin_num] = node;
            node->prev_inbin = nullptr;
            node->next_inbin = bin_head;
            bin_head->prev_inbin = node;

        }
        else //if its in the middle of the list
        {
            node->prev_inbin = bin_head->prev_inbin;
            node->next_inbin = bin_head;
            bin_head->prev_inbin->next_inbin = node;
            bin_head->prev_inbin = node;

        }      
    }
    num_free_bytes+=node->size;
    num_free_blocks++;
    return;
}

block_t SearchFreeBlock(size_t block_size)
{

        block_t temp_head;
        if (block_size <= NUM_OF_BINS*KB)
        {
            int starting_index = block_size/KB;
            for (int i = starting_index;i<NUM_OF_BINS;i++) // loop through bin array starting from the most fitting index.
            {
                temp_head = bins_arr[i];
                while(temp_head)
                {
                    if (temp_head->size >= block_size) // most fitting 
                    {
                        return temp_head;
                    }
                    else
                    {
                        temp_head=temp_head->next_inbin;
                    }
                }
            }
        }
        return nullptr;
}

void* SbrkAllocation(size_t size)
{
        block_t p_new_block = SearchFreeBlock(size);
        if (p_new_block == nullptr) // if there`s no free block use sbrk()
        {
            if ((blocks_list_tail) && blocks_list_tail->is_free)// if the "wilderness chunk" is free like 'challenge 3'
            {
                void* temp = sbrk(size-blocks_list_tail->size);
                if (temp != (void*)-1)//brk() success
                {
                    DeleteBinNode(blocks_list_tail,blocks_list_tail->size/KB);
                    num_allocated_bytes+=size-blocks_list_tail->size;
                    blocks_list_tail->size=size; // update size field of the wilderness chunk to the new one
                    blocks_list_tail->is_free = false;
                    return blocks_list_tail+1;
                }
                return nullptr;
            }
            else                          // if the wilderness chunk isn`t free allocate regulary
            {
                void* p = sbrk(0);
                void* temp = sbrk(size+TAGS_SIZE);
                if (temp != (void*)-1)//  brk() success
                {
                    p_new_block = (block_t)p;
                    p_new_block->is_free = false; 
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
        } 
        else                                      // in case there`s a matching free block
        {
            block_t res_block =  SplitBlocks(p_new_block,size);
            DeleteBinNode(res_block,res_block->size/KB);
            res_block->is_free = false;
            return res_block+1;
        }            
}

void* MmapAllocation(size_t size)
{
    void *addr = mmap(NULL, size + TAGS_SIZE,
            PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE,
            0, 0);
    if (addr == MAP_FAILED) return NULL;
    block_t meta = (block_t)(addr);
    meta->size = size;
    meta->is_free = false;
    num_allocated_blocks++;
    num_allocated_bytes+=size;
    return meta+1;
}

block_t SplitBlocks(block_t full_block,size_t size)
{
    if (full_block->size - size <128 +TAGS_SIZE) // in case the leftover block is too small return the normal block
    {
        return full_block;
    }

    block_t first_block=full_block+1; 
    char* bytes_first_block = (char*)(first_block);
    first_block--;
    char* bytes_second_block = bytes_first_block+size;
    block_t second_block = (block_t)(bytes_second_block);
    second_block->is_free = true;
    num_allocated_blocks++;
    num_allocated_bytes-=TAGS_SIZE;
    second_block->size = full_block->size-size-TAGS_SIZE;

    if ( full_block->is_free)
    {   

        DeleteBinNode(full_block,full_block->size/KB); // Handle bin arr
        first_block->size = size;
        AddBinNode(first_block,first_block->size/KB);

    }
    else //wierd edge case DON`T TOUCH !!
    {
        first_block->size = size;
    }
    second_block->next_inbin = nullptr;
    second_block->prev_inbin = nullptr;
    AddBinNode(second_block,second_block->size/KB);
                                   
                                        //Handle List
    second_block->next=full_block->next;
    if(full_block->next)
    {
        full_block->next->prev = second_block;
    }
    first_block->next=second_block;
    second_block->prev=first_block;
    if (first_block == blocks_list_tail)
    {
         blocks_list_tail=second_block; 
    }
    return first_block;
}
void InsertNewBlock(block_t new_block)
{
    block_t temp_head = blocks_list_head;
    if (!blocks_list_head) // in case its first request
    {
        blocks_list_head=new_block;
        blocks_list_tail = new_block;
        new_block->next = nullptr;
        new_block->prev = nullptr;
        return;
    }

    if ( &new_block > &blocks_list_tail) // in case it`s the new tail
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

    if (size >= 128*KB)
    {
        return MmapAllocation(size);
    }
    else
    {
        return SbrkAllocation(size);
    }
}

void MmapFree(block_t p)
{
    num_allocated_blocks--;
    num_allocated_bytes-=p->size;
    munmap(p, (TAGS_SIZE + p->size));
    return;
}

void SbrkFree(block_t p_released_block)
{
    if (!(isPreBlockFree(p_released_block)) && !(isNextBlockFree(p_released_block)))
    {
        AddBinNode(p_released_block,p_released_block->size/KB);
        return;
    }
    AddBinNode(p_released_block,p_released_block->size/KB);
    if (isNextBlockFree(p_released_block))
    {
        p_released_block=MergeAdjacent(p_released_block,p_released_block->next);
    }
    if (isPreBlockFree(p_released_block))
    {
        p_released_block=MergeAdjacent(p_released_block->prev,p_released_block);
    
    }
    if ((p_released_block)->next == nullptr)
    {
        blocks_list_tail = p_released_block;
    }
    return;
}

void sfree(void* p)
{
    if (p == nullptr)
    {
        return;
    }
    block_t p_released_block = (block_t) p;
    p_released_block = p_released_block-1;
    if (p_released_block->is_free == true)
    {
        return;
    }
    p_released_block->is_free = true;
    if (p_released_block->size >= 128*KB)
    {
        MmapFree(p_released_block);
    }
    else
    {
        SbrkFree(p_released_block);
      
    }
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

    block_t old_block = (block_t)(oldp);
    old_block=old_block-1;
    if (size >= 128*KB)
    {
        return MmapSrealloc(old_block,size);
    }
    else
    {
        return SbrkSrealloc(old_block,size);
    }
}

void* MmapSrealloc(block_t old_block,size_t size)
{
    
    void* ptr = smalloc(size);
    if (ptr == nullptr)
    {
        return nullptr;
    }   
    size_t min=size;
    if (size>old_block->size)
    {
        min= old_block->size;
    }
    memmove(ptr,old_block+1,min);
    sfree(old_block+1);
    return ptr;
}

void* SbrkSrealloc(block_t old_block,size_t size)
{
    if (size <= old_block->size) // case a
    {
        return SplitBlocks(old_block,size)+1;
    }

    if ((old_block->prev) && ((old_block->prev)->is_free) && 
    ((old_block->size + (old_block->prev)->size)+TAGS_SIZE >= size)) // MERGE WITH PREVIOUS BLOCK
    {
        return reallocCaseB(old_block,size);
    }

    if ((old_block->next) && ((old_block->next)->is_free) &&
     ((old_block->size + (old_block->next)->size)+TAGS_SIZE >= size)) // MERGE WITH NEXT BLOCK
    {
        return reallocCaseC(old_block,size);
    }

        if ((old_block->next) && ((old_block->next)->is_free) && (old_block->prev) &&           //MERGE WITH BOTH BLOCKS
        ((old_block->prev)->is_free) &&
        (old_block->size + ((old_block->next)->size) +
         ((old_block->prev)->size) +2*TAGS_SIZE>= size))
         {
             return reallocCaseD(old_block,size);
         }

    if( old_block->next == nullptr)
    {
        void* temp = sbrk(size-old_block->size);
        if (temp == (void*)-1)
        {
            return nullptr;
        }
        num_allocated_bytes+=size-old_block->size;
        old_block->size=size;
        return old_block+1;
    }
    
    void* new_p = smalloc(size);
    if (new_p !=nullptr)
    {
        memcpy(new_p,old_block+1,size);
        sfree(old_block+1);
        return new_p;
    }
    return nullptr;

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
