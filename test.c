#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.c"


int main(int argc, char **argv) {
    int position, voltage, current;
    while (scanf("%d %d %d\n", &position, &current, &voltage) != EOF) {
        process(position, current, voltage);
        printf("%d %d %d ", position, current, voltage);
        for (int i = 0; i < POSITIONS; ++i) {
            printf("%d ", info[i].state);
        }
        printf("\n");
    }
}
