#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "memory.h"

enum Boolean {
    FALSE,
    TRUE
};

typedef struct Blocks
{
    enum Boolean valid;
    int tag;
    char *data;
    int LRU;
} blocks;

typedef struct Sets {
    blocks *block;
} sets;

typedef struct Cache {
    sets *set;
    int w;
    int s;
    int b;
} cache;

int main(int argc, char *argv[])
{
    char op;
    char *addr;
    char *data;

    struct memory *mem = memory_create();
    cache *cach;

    if (argc == 4)
    {
        int w;
        int s;
        int b;
        sscanf(argv[1], "%d", &w);
        sscanf(argv[2], "%d", &s);
        sscanf(argv[3], "%d", &b);

        cach = calloc(1, w * s * (b + 0.5));

        if (w <= 16) {
            cach->w = w;
        } else {
            printf("W error");
        }

        if ((s % 2) == 0) {
            cach->s = s;
        } else {
            printf("S error");
        }

        if (b >= 4 && (b % 2) == 0) {
            cach->b = b;
        } else {
            printf("B error");
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("Input: OP ADDR DATA\n\n");

    while ((read = getline(&line, &len, stdin)) != -1) {
        // printf("Retrieved line of length: %zu\n", read);
        // printf("%s", line);

        int m;
        int n;
        int addr_int;
        int data_int;
        int set_index;
        int tag_index;

        op = (char)*strtok(line, " ");
        addr = strtok(NULL, " ");
        addr_int = atoi(addr);
        data = strtok(NULL, " ");
        data_int = atoi(data);

        if (op == NULL || addr == NULL) {
            printf("3 arguments for OP i or w. 2 arguments for r\n");
        } else {
            printf("op: %c, addr: %s, data: %s\n", op, addr, data);

            switch (op)
            {
                case 'i':
                    memory_wr_w(mem, addr_int, data_int);
                    printf("Inserted into mem\n");
                    break;

                case 'r':
                    /* code */
                    break;

                case 'w':
                    //Find set
                    m = log(cach->b) / log(2);
                    n = log(cach->s) / log(2);
                    printf("m: %d, n: %d\n", m, n);

                    set_index = (addr_int >> m) & ~(~0 << n);
                    printf("%d\n", set_index);

                    tag_index = (addr_int >> (m + n + 1));
                    printf("%d\n", tag_index);

                    if (cach->set[set_index].block[tag_index].valid != 0) {
                        printf("Not valid");
                    }

                    // printf("%d", cach->set[set_index].block[tag_index]);

                    for (int i = 0; i < cach->w; i++)
                    {
                        // if (chosen_set.block[i].tag == tag_index) {
                        //     printf("Found em\n");
                        // }
                        printf("%d\n", i);
                    }

                    /*


                    Find least recently used block

                    cache->set[SET INDEX] (Se https://github.com/diku-compSys/compSys-e2022-pub/blob/main/lectures/220926_caching/memory_hierarchy.pdf ---- address structure)

                    for-loop gennem block og find rigtigt tag

                    brug offset i blocken

                    code */
                    break;

                default:
                    break;
            }
        }
    }
}


/*
Cache struktur:

Cache
Sets
Lines
Blocks

*/
