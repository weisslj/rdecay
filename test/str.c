#include <stdio.h>
#include <errno.h>

int main()
{
    double a, b;
    int x, y;

    x = 2;
    y = 0;

    a = 2.3;
    b = 2.0;

    if (printf("%s: %.*f, %s: %.0f\n", "a", x, a, "b", b) < 0)
        perror("printf");

    return 0;
}
