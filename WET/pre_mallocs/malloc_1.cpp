#include <sys/types.h>
#include <unistd.h>

long long int pow(int base,int power)
{
    int res;
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

void* smalloc(size_t size)
{
    void* p_allocated_block  = sbrk(size);
    if (p_allocated_block == (void*)-1 || size <= 0 || size > pow(10,8))
    {
        return NULL;
    }
    return sbrk(0);
}