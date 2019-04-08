/*
Cache Lab
Riley Mangan, Harris McCullers, and Kevin Sasaki
*/

#include "cachelab.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

//If l is the length of the address (in bits), then the number of tag bits is t = l − b − s
//t = 64 - b - s
struct set{
    char * address, value, tag, block;
    int index, priority;
};

struct cache{
    int hits, misses, evictions;
    struct set our_sets[10];
};

// struct obj load(struct cache our_cache, int address, int size){
//     return cache;
// }

// struct obj store(struct cache our_cache, int address, int size){
//     return cache;
// }
int in_cache(struct cache * our_cache, char * address){
    int i;
    for(i = 0; i < sizeof(our_cache->our_sets)/sizeof(our_cache->our_sets[0]); i++){
        if(our_cache->our_sets[i].address == address){
            return 1;
        }
    }
    return 0;
};

//is something like this necessary?
//the idea is that we keep track of the order that the sets came in with an int priority in the set struct
//the next set to get kicked out is one with a priority number equal to the number of sets
//each priority number gets updated in the function, so anytime we modify the cache, we call this
struct cache* update_priority(struct cache * our_cache, struct set* our_set){
    int old_priority = our_set->priority;
    for(int i = 0; i < sizeof(our_cache->our_sets)/sizeof(our_cache->our_sets[0]); i++){
        if(our_cache->our_sets[i].address == our_set->address){
            our_cache->our_sets[i].priority = 0;
        }
        else if(our_cache->our_sets[i].priority < old_priority){
            our_cache->our_sets[i].priority++;
        }
    }

    return our_cache;
}

struct cache * helper_func(char * operation, char * address, int size, struct cache * our_cache){
    int opt = (*operation);
    int add = 0;

    //adjust hits an misses for our cache in this switch statement
    switch(opt){
        case 'I':
            break;
        case 'S':
            //if a miss, we need to add the new block to the cache
            add = in_cache(our_cache, address);
            our_cache->hits += add;
            our_cache->misses += 1 - add;

            //if a hit, we need to update the priority numbers
            break;
        case 'L':
            add = in_cache(our_cache, address);
            our_cache->hits += add;
            our_cache->misses += 1 - add;
            break;
        case 'M':
            add = in_cache(our_cache, address);
            our_cache->hits += add;
            our_cache->misses += 1 - add;
            break;
        default:
            abort();
    }

    return our_cache;
};

int main(int argc, char **argv)
{
    //parse the input string
    int opt, hflag, vflag, s, E, b;
    char* t;
    while( (opt = getopt(argc, argv, "h::v::s:E:b:t:")) != -1){
        switch(opt){
            case 'h':
                hflag = 1;
                break;
            case 'v':
                vflag = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = optarg;
                break;
            default:
                abort();
                
        }
    }

    //allocate enough space for our cache
    struct cache * our_cache = malloc(s*E*b);

    //read file line by line from text file and process
    FILE *file = fopen(t, "r");
    char operation[2], address[8];
    int size;
    while(fscanf(file, " %s %s, %d", operation, address, &size) != -1){
        our_cache = helper_func(operation, address, size, our_cache);
    }
    printf("%s %s %d\n", operation, address, size);

    printf("s, E, b, t: %d %d %d %s\n",s,E,b,t);

    printSummary(0, 0, 0);

    free(our_cache);
    return 0;
}
