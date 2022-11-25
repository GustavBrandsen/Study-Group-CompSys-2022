#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct naive_data
{
    struct record *rs;
    int n;
};

struct naive_data *mk_naive(struct record *rs, int n)
{
    struct naive_data *data = malloc(sizeof(struct naive_data));
    data->rs = rs;
    data->n = n;
    return data;
}

void free_naive(struct naive_data *data)
{
    free(data);
}

const struct record *lookup_naive(struct naive_data *data, int64_t needle)
{
    int count = 0;
    while ((data->n) > count)
    {
        if ((data->rs[count].osm_id) == needle)
        {
            return &data->rs[count];
        }
        count++;
    }
    return NULL;
}

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

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

int compare_osm_id(const void *x, const void *y)
{
    int a = ((struct index_record *)x)->osm_id;
    int b = ((struct index_record *)y)->osm_id;
    return (a - b);
}

struct index_record *build_index_record(struct record *rs, int n)
{
    struct index_record *data = calloc(n, sizeof(struct index_record));
    for (int i = 0; i < n; i++)
    {
        data[i].record = &rs[i];
        data[i].osm_id = rs[i].osm_id;
    }
    qsort(data, n, sizeof(struct index_record), compare_osm_id);
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
    int low = 0;
    int high = (data->n) - 1;
    int mid = (low + high) / 2;
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
        mid = (low + high) / 2;
    }
    return NULL;
}

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

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

int compare_osm_id_binsort(const void *x, const void *y)
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
    qsort(data, n, sizeof(struct bin_record), compare_osm_id_binsort);
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

//
//
//
//
//
//
//
//
//

const char *check_ID(int argc, char **argv, mk_index_fn mk_index, free_index_fn free_index, lookup_fn lookup, int64_t needleArray[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        exit(1);
    }

    int n;

    struct record *rs = read_records(argv[1], &n);

    if (rs)
    {
        void *index = mk_index(rs, n);

        char *line = NULL;
        const char *nameArray[2000] = {};
        char *memArray = malloc(sizeof(nameArray));
        for (int i = 0; i < 2000; i++)
        {
            int64_t needle = needleArray[i];

            const struct record *r = lookup(index, needle);

            if (r)
            {
                // printf("%ld: %s %f %f\n", (long)needle, r->name, r->lon, r->lat);
                nameArray[i] = r->name;
                // printf("%s\n", nameArray[i]);
            }
            else
            {
                printf("%ld: not found\n", (long)needle);
            }
        }
        for (int i = 0; i < 2000; i++)
        {
            memArray[i] = nameArray[i];
            // printf("%s\n", nameArray[i]);
        }
        free(line);
        free_index(index);
        free_records(rs, n);
        return memArray;
    }
    else
    {
        fprintf(stderr, "Failed to read input from %s (errno: %s)\n",
                argv[1], strerror(errno));
        return "Error";
    }
}

int main(int argc, char **argv)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    int64_t read;
    int64_t needleArray[2000] = {};
    int counter = 0;

    fp = fopen("2K_ids", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        needleArray[counter] = atol(line);
        // printf("%lld\n", atol(line));
        counter++;
    }
    printf("%s", check_ID(argc, argv,
                          (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive,
                          (lookup_fn)lookup_naive,
                          needleArray));

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}
