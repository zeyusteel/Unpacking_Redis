#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    /*
    char *p = strdup("hhhhh");
    argv[0] = p;
    */
    printf("%s\n", getprogname());

    strcpy(argv[0], "change");
    while (1);
    return 0;
}
