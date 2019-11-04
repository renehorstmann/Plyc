#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "plyc/ply.h"



int main() {
    puts("isalpha checker");
    char test;
    for(;;) {
        scanf("%c", &test);
        printf("char %c isalpha: %d\n", test, !isalpha(test));
    }

    return 0;
}
