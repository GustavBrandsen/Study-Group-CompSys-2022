#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "id_query.h"

struct bin_record
{
    int64_t osm_id;
    const struct record *record;
};

struct bined_data
{
    struct bin_record *irs;
    int n;
};

int compare_osm_id(const void *x, const void *y)
{
    int64_t a = ((struct bin_record *)x)->osm_id;
    int64_t b = ((struct bin_record *)y)->osm_id;
    if (a > b)
    {
        return 1;
    }
    if (a < b)
    {
        return -1;
    }
    return 0;
}

struct bin_record *build_binsort_record(struct record *rs, int n)
{
    struct bin_record *data = calloc(n, sizeof(struct bin_record));
    for (int i = 0; i < n; i++)
    {
        data[i].record = &rs[i];
        data[i].osm_id = rs[i].osm_id;
    }
    qsort(data, n, sizeof(struct bin_record), compare_osm_id);
    return data;
}

struct bined_data *mk_binsort(struct record *rs, int n)
{
    struct bined_data *data = malloc(sizeof(struct bined_data));
    data->irs = build_binsort_record(rs, n);
    data->n = n;
    return data;
}
void free_binsort(struct bined_data *data)
{
    free(data->irs);
    free(data);
}

const struct record *lookup_binsort(struct bined_data *data, int64_t needle)
{
    int low = 0;
    int high = (data->n) - 1;
    int mid = (low / 2 + high / 2);
    while (low <= high)
    {
        if (data->irs[mid].osm_id < needle)
            low = mid + 1;
        else if (data->irs[mid].osm_id == needle)
        {
            return data->irs[mid].record;
        }
        else
            high = mid - 1;
        mid = (low / 2 + high / 2);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    return id_query_loop(argc, argv,
                         (mk_index_fn)mk_binsort,
                         (free_index_fn)free_binsort,
                         (lookup_fn)lookup_binsort);
}
