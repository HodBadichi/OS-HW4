#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define ThreshHold 131048

int main(int argc, char** argv)
{
    write(1,"Before:ThreshHold-1",19);
    malloc(ThreshHold-1);
    write(1,"After:ThreshHold-1",18);
    write(1,"Before:ThreshHold",17);
    malloc(ThreshHold);
    write(1,"After:ThreshHold",16);
    write(1,"Before:ThreshHold+1",17);
    malloc(ThreshHold+1);
    write(1,"After:ThreshHold+1",16);



    return 0;
}

