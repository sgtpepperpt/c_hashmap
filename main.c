/*
 * A unit test and example of how to use the simple C hashmap
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <sys/time.h>
#include <time.h>

#include "hashmap.h"

long util_time_elapsed(struct timeval start, struct timeval end) {
    long secs_used,micros_used;

    secs_used = (end.tv_sec - start.tv_sec); //avoid overflow by subtracting first
    micros_used = ((secs_used*1000000) + end.tv_usec) - (start.tv_usec);
    return micros_used;
}

#define KEY_LENGTH (64)
#define KEY_COUNT (256*256)

typedef struct data_struct_s {
    char key_string[KEY_LENGTH];
    int number;
} data_struct_t;

int main(int argc, char** argv) {
    int error;
    data_struct_t* value;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    map_t mymap = hashmap_new(KEY_LENGTH);

    /* First, populate the hash map with ascending values */
    for (unsigned i = 0; i < KEY_COUNT; i++) {
        /* Store the key string along side the numerical value so we can free it later */
        value = malloc(sizeof(data_struct_t));
        snprintf(value->key_string, KEY_LENGTH, "%063d", i);
        printf("%s %d\n", value->key_string, i);
        value->number = i;

        error = hashmap_put(mymap, value->key_string, value);
        assert(error==MAP_OK);
    }

    char key_string[KEY_LENGTH];

    /* Now, check all of the expected values are there */
    for (unsigned i = 0; i < KEY_COUNT; i++) {
        snprintf(key_string, KEY_LENGTH, "%063d", i);

        error = hashmap_get(mymap, key_string, (void**)(&value));

        /* Make sure the value was both found and the correct number */
        assert(error==MAP_OK);
        assert(value->number==i);
    }

    /* Make sure that a value that wasn't in the map can't be found */

    snprintf(key_string, KEY_LENGTH, "%063d", KEY_COUNT);

    error = hashmap_get(mymap, key_string, (void**)(&value));

    /* Make sure the value was not found */
    assert(error==MAP_MISSING);

    /* Free all of the values we allocated and remove them from the map */
    for (unsigned i = 0; i < KEY_COUNT; i++) {
        snprintf(key_string, KEY_LENGTH, "%063d", i);

        error = hashmap_get(mymap, key_string, (void**)(&value));
        assert(error==MAP_OK);

        error = hashmap_remove(mymap, key_string);
        assert(error==MAP_OK);

        free(value);
    }

    /* Now, destroy the map */
    hashmap_free(mymap);

    gettimeofday(&end, NULL);
    long total = util_time_elapsed(start, end);
    printf("%ld\n", total/1000);

    return 1;
}
