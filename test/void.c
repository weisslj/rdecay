#include <stdio.h>

#define TEST1 /*void*/
#define TEST2 /*void*/

typedef void argh;

argh testx(int x)
{
    if (x)
        return TEST1;
    else
        return TEST2;
}

int main()
{
    testx(1);
    return 0;
}
