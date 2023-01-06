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
    sets *set;

    if (argc == 4)
    {
        int w;
        int s;
        int b;
        sscanf(argv[1], "%d", &w);
        sscanf(argv[2], "%d", &s);
        sscanf(argv[3], "%d", &b);

        cach = calloc(1, sizeof(cache));
        cach->set = calloc(s, sizeof(sets));
        for (int i = 0; i < s; i++)
        {
            cach->set[i].block = calloc(w, sizeof(blocks) + b);
        }


        if (w <= 16) {
            cach->w = w;
        } else {
            printf("W error");
            return -1;
        }

        if ((s % 2) == 0) {
            cach->s = s;
        } else {
            printf("S error");
            return -1;
        }

        if (b >= 4 && (b % 2) == 0) {
            cach->b = b;
        } else {
            printf("B error");
            return -1;
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
        int offset;

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
                    printf("%c %s %s INIT\n", op, addr, data);
                    memory_wr_w(mem, addr_int, data_int);
                    break;

                case 'r':
                    // m = log(cach->b) / log(2);
                    // n = log(cach->s) / log(2);
                    // printf("m: %d, n: %d\n", m, n);

                    // set_index = (addr_int >> m) & ~(~0 << n);
                    // printf("%d\n", set_index);

                    // tag_index = (addr_int >> (m + n + 1));
                    // printf("%d\n", tag_index);

                    // offset = (addr_int & (m - 1));

                    // sets found_set = cach->set[set_index];
                    // blocks *found_block = NULL;

                    // for (int i = 0; i < cach->w; i++)
                    // {
                    //     if (found_set.block[i].tag == tag_index) {
                    //         printf("op addr data HIT lru-order\n");
                    //         found_block = &cach->set[set_index].block[i];
                    //         break;
                    //     }
                    // }

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

                    offset = (addr_int & ~(~0 << m));


                    sets found_set = cach->set[set_index];
                    blocks *found_block = NULL;

                    for (int i = 0; i < cach->w; i++)
                    {
                        if (found_set.block[i].tag == tag_index) {
                            printf("op addr data HIT lru-order\n");
                            found_block = &cach->set[set_index].block[i];
                            break;
                        }
                    }

                    if (found_block != NULL && found_block->valid == 1)
                    {
                        printf("INSERTED\n");
                        found_block->data[offset] = data_int;
                    } else {
                        printf("op addr data FILL lru-order\n");
                        int lowest_LRU = cach->w;
                        blocks *new_block;
                        for (int i = 0; i < cach->w; i++)
                        {
                            if (found_set.block[i].LRU < lowest_LRU) {
                                lowest_LRU = found_set.block[i].LRU;
                                new_block = &found_set.block[i];
                            }
                        }
                        // for (int i = 0; i < cach->b / 4; i++)
                        // {
                        //     new_block->data[i] = memory_rd_w(mem, addr_int + i * 4);
                        //     /* code */
                        // }
                        int mem_data = calloc(1, cach->b);
                        mem_data = memory_rd_w(mem, addr_int);

                        new_block->data = &mem_data;
                        new_block->valid = 1;
                        new_block->tag = tag_index;
                        new_block->LRU = cach->w + 1;
                        printf("Inserted new block\n");

                        for (int i = 0; i < cach->w; i++)
                        {
                            found_set.block[i].LRU -= 1;
                        }
                        printf("LRU updated in set\n");

                        if (new_block == NULL)
                        {
                            printf("BLOCK NOT INIT");
                        }

                        printf("%d\n", new_block);

                        new_block->data[offset] = data_int;

                    }

                    /*
                    HIT: hvis  vi finder blokken i read eller write og den er valid
                    FILL: Tjek om der 0'er
                    EVICT: idk?
                    DISCARD: idk?


                    */
                    break;

                default:
                    break;
            }
        }
    }
}

