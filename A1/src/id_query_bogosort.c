#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

#include <time.h>

struct index_record
{
    int64_t osm_id;
    const struct record *record;
};

struct indexed_data
{
    struct index_record *irs;
    int n;
};

int bogocount = 1;

int check_sorted(struct index_record *data, int n)
{
    while (--n >= 1)
    {
        if (data[n].osm_id < data[n - 1].osm_id)
        {
            return 0;
        }
    }
    return 1;
}

void randomize(struct index_record *data, int n)
{
    int random;
    struct index_record temporary;
    for (int i = 0; i < n; i++)
    {
        temporary = data[i];
        random = rand() % n;
        data[i] = data[random];
        data[random] = temporary;
    }
}

void bogosort(struct index_record *data, int n)
{
    while (!check_sorted(data, n))
    {
        bogocount++;
        randomize(data, n);
    }
    printf("Number of bogo sort iterations %d \n", bogocount);
}

struct index_record *build_index_record(struct record *rs, int n)
{
    struct index_record *data = calloc(n, sizeof(struct index_record));
    for (int i = 0; i < n; i++)
    {
        data[i].record = &rs[i];
        data[i].osm_id = rs[i].osm_id;
    }
    bogosort(data, n);
    return data;
}

struct indexed_data *mk_indexed(struct record *rs, int n)
{
    struct indexed_data *data = malloc(sizeof(struct indexed_data));
    data->irs = build_index_record(rs, n);
    data->n = n;
    return data;
}
void free_indexed(struct indexed_data *data)
{
    free(data);
}

const struct record *lookup_indexed(struct indexed_data *data, int64_t needle)
{
    int max_rand = data->n;
    srand(time(NULL));
    int r = rand() % max_rand;
    int count = 0;
    while (data->irs[r].osm_id != needle && count < 1000000000)
    {
        r = rand() % max_rand;
        count++;
    }
    printf("Number of random guesses: %d \n", count);
    if (data->irs[r].osm_id == needle)
    {
        return data->irs[r].record;
    }
    else
    {
        return NULL;
    }
}

int main(int argc, char **argv)
{
    return id_query_loop(argc, argv,
                         (mk_index_fn)mk_indexed,
                         (free_index_fn)free_indexed,
                         (lookup_fn)lookup_indexed);
}
