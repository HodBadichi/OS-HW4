//
// Created by Avia on 19/06/2020.
//

#include <unistd.h>
#include <cstring>
#include <sys/mman.h>

struct MallocMetadata {
    size_t size;
    bool is_free;
    MallocMetadata *next;
    MallocMetadata *prev;
};

static MallocMetadata *list_start = NULL;
static MallocMetadata *mmap_list_start = NULL;


size_t _num_free_blocks() {
    size_t num_free = 0;
    MallocMetadata *meta = list_start;
    while (meta) {
        if (meta->is_free) num_free++;
        meta = meta->next;
    }
    return num_free;
}

size_t _num_free_bytes() {
    size_t num_free_bytes = 0;
    MallocMetadata *meta = list_start;
    while (meta) {
        if (meta->is_free) num_free_bytes += meta->size;
        meta = meta->next;
    }
    return num_free_bytes;
}

size_t _num_allocated_blocks() {
    size_t num_allocated = 0;
    MallocMetadata *meta = list_start;
    while (meta) {
        num_allocated++;
        meta = meta->next;
    }
    meta = mmap_list_start;
    while (meta) {
        num_allocated++;
        meta = meta->next;
    }
    return num_allocated;
}

size_t _num_allocated_bytes() {
    size_t num_allocated_bytes = 0;
    MallocMetadata *meta = list_start;
    while (meta) {
        num_allocated_bytes += meta->size;
        meta = meta->next;
    }
    meta = mmap_list_start;
    while (meta) {
        num_allocated_bytes += meta->size;
        meta = meta->next;
    }
    return num_allocated_bytes;
}

size_t _size_meta_data() {
    return sizeof(MallocMetadata);
}

size_t _num_meta_data_bytes() {
    size_t num_meta_data_bytes = 0;
    MallocMetadata *meta = list_start;
    while (meta) {
        num_meta_data_bytes += _size_meta_data();
        meta = meta->next;
    }
    meta = mmap_list_start;
    while (meta) {
        num_meta_data_bytes += _size_meta_data();
        meta = meta->next;
    }
    return num_meta_data_bytes;
}

static void *_mem_alloc(size_t size, MallocMetadata **meta_start) {
    MallocMetadata *meta_point;
    void *to_ret;
    meta_point = (MallocMetadata *) sbrk(sizeof(MallocMetadata));
    if (meta_point == (void *) -1) {
        return NULL;
    }
    to_ret = sbrk(size);
    if (to_ret == (void *) -1) {
        return NULL;
    }
    *meta_start = meta_point;
    return to_ret;
}

static MallocMetadata *_split_block(MallocMetadata *meta, size_t size) {
    size_t prev_size = meta->size;
    meta->size = size;
    meta->is_free = false;
    MallocMetadata *first_block = meta + 1;
    char *first_block_bytes = (char *) (first_block);
    char *second_block_meta_bytes = first_block_bytes + size;
    MallocMetadata *second_block_meta = (MallocMetadata *) second_block_meta_bytes;
    second_block_meta->next = meta->next;
    second_block_meta->prev = meta;
    meta->next = second_block_meta;
    second_block_meta->size = prev_size - size - _size_meta_data();
    second_block_meta->is_free = true;
    return meta;
}

void *smalloc(size_t size) {
    if (size == 0 || size > 100000000) return NULL;

    MallocMetadata *meta_start;
    void *block_start;

    if (size >= 128000) {
        void *addr = mmap(NULL, size + _size_meta_data(),
                          PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE,
                          0, 0);
        if (addr == MAP_FAILED) return NULL;
        MallocMetadata *meta = static_cast<MallocMetadata *>(addr);
        meta->size = size;
        meta->is_free = false;
        MallocMetadata *start = mmap_list_start;
        if (start) {
            start->prev = meta;
        }
        meta->next = start;
        meta->prev = NULL;
        mmap_list_start = meta;
        return (meta + 1);
    }

    if (!list_start) {
        block_start = _mem_alloc(size, &meta_start);
        if (!block_start) return NULL;
        meta_start->size = size;
        meta_start->is_free = false;
        meta_start->next = NULL;
        meta_start->prev = NULL;
        list_start = meta_start;
        return block_start;
    }
    meta_start = list_start;
    MallocMetadata *meta_start_prev = list_start;
    while (meta_start) {
        if (meta_start->is_free && (meta_start->size >= size)) {
            if (meta_start->size >=
                (128 + _size_meta_data() + size)) {
                meta_start = _split_block(meta_start, size);
                return (meta_start + 1);
            }
            meta_start->is_free = false;
            return (meta_start + 1);
        }
        meta_start_prev = meta_start;
        meta_start = meta_start->next;
    }

    if (meta_start_prev->is_free) {
        size_t to_add = size - meta_start_prev->size;
        void *point;
        point = sbrk(to_add);
        if (point == (void *) -1) {
            return NULL;
        }
        meta_start_prev->size += to_add;
        meta_start_prev->is_free = false;
        return (meta_start_prev + 1);
    }

    block_start = _mem_alloc(size, &meta_start);
    meta_start->size = size;
    meta_start->is_free = false;
    meta_start->prev = meta_start_prev;
    meta_start->next = NULL;
    meta_start_prev->next = meta_start;
    return block_start;
}

void *scalloc(size_t num, size_t size) {
    void *to_ret = smalloc(num * size);
    if (to_ret) {
        memset(to_ret, 0, num * size);
    }
    return to_ret;
}

void _two_block_union(MallocMetadata *first, MallocMetadata *second) {
    if (second->next) {
        (second->next)->prev = first;
    }
    first->next = second->next;
    (first->size) += (_size_meta_data() + second->size);
}

void sfree(void *p) {
    if (!p) return;
    MallocMetadata *metadata = (MallocMetadata *) p - 1;
    if (metadata->is_free) return;
    if (metadata->size >= 128000) {
        if (metadata->prev) {
            (metadata->prev)->next = metadata->next;
        } else {
            mmap_list_start = metadata->next;
        }
        if (metadata->next) {
            (metadata->next)->prev = metadata->prev;
        }
        munmap(metadata, (_size_meta_data() + metadata->size));
        return;
    }
    metadata->is_free = true;

    if ((metadata->next) && ((metadata->next)->is_free)) {
        _two_block_union(metadata, metadata->next);
    }
    if ((metadata->prev) && ((metadata->prev)->is_free)) {
        _two_block_union(metadata->prev, metadata);
    }
}

void *srealloc(void *oldp, size_t size) {
    if (size == 0 || size > 100000000) return NULL;
    if (!oldp) {
        return smalloc(size);
    }
    MallocMetadata *metadata = (MallocMetadata *) oldp - 1;

    if (size >= 128000) {
        void *point = smalloc(size);
        if (!point)
            return NULL;
        size_t min = (size > metadata->size) ? metadata->size : size;
        memmove(point, oldp, min);
        sfree(oldp);
        return point;
    }

    if (size <= metadata->size) {
        if (metadata->size >=
            (128 + _size_meta_data() + size)) {
           _split_block(metadata,size);
        }
        return oldp;
    }
    if (!metadata->next) {
        void *to_ret = sbrk((size - metadata->size));
        if (to_ret == (void *) -1) {
            return NULL;
        }
        metadata->size += (size - metadata->size);
        return oldp;
    }
    // Prev merge
    if ((metadata->prev) && ((metadata->prev)->is_free) &&
        ((metadata->size + (metadata->prev)->size) >= size)) {
        MallocMetadata *block = metadata->prev;
        _two_block_union(block, metadata);
        memmove((block) + 1, oldp, metadata->size);
        if (block->size >= (128 + _size_meta_data() + size)) {
            block = _split_block(block, size);
        }
        block->is_free = false;
        return (block + 1);
    }
    // Next merge
    if ((metadata->next) && ((metadata->next)->is_free) &&
        ((metadata->size + (metadata->next)->size) >= size)) {
        _two_block_union(metadata, metadata->next);
        if (metadata->prev->size >= (128 + _size_meta_data() + size)) {
            metadata->prev = _split_block(metadata, size);
        }
        metadata->is_free = false;
        return (metadata + 1);
    }
    // Double merge
    if ((metadata->next) && ((metadata->next)->is_free) && (metadata->prev) &&
        ((metadata->prev)->is_free) &&
        (metadata->size + ((metadata->next)->size) +
         ((metadata->prev)->size) >= size)) {
        MallocMetadata *block = metadata->prev;
        _two_block_union(block, metadata->next);
        block->size += (_size_meta_data() + metadata->size);
        if (block->size >= (128 + _size_meta_data() + size)) {
            block = _split_block(block, size);
        }
        block->is_free = false;
        return (block + 1);
    }

    void *to_ret = smalloc(size);
    if (to_ret) {
        memcpy(to_ret, oldp, metadata->size);
        sfree(oldp);
        return to_ret;
    }
    return NULL;
}