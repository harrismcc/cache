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

#define cache_size 10

/* Notes:

I think we will use write-back + write-allocate from the 3rd cache lecture?

If l is the length of the address (in bits), then the number of tag bits is t = l − b − s
*/
struct set{
    char * address, value, tag, block;
    int index, priority;
};


//TODO make a constructor to make cache_size equal to whatever our input was from the command line
struct cache{
    int hits, misses, evictions;
    struct set * our_sets;
};

//cache constructor
struct cache * cache_new(int x) {
    struct cache *c = malloc(sizeof *c);
    c->hits = 0;
    c->misses = 0;
    c->evictions = 0;
    struct set s[x];
    c->our_sets = s;
    return c;
}

/* delete struct */
void cache_delete(struct cache * c) {
    free(c);
}

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

//supposed 
struct cache * place_in_cache(struct cache * our_cache, char * address){
    for(int i = 0; i < cache_size; i++){
        if(our_cache->our_sets[i].priority == cache_size){
            update_priority(our_cache, &our_cache->our_sets[i]);
            our_cache->our_sets[i].address = address;
            return our_cache;
        }
    }
    return our_cache;
}

//returns 1 if the address is in the our_cache
//returns 0 if the address is not in our cache
int in_cache(struct cache * our_cache, char * address){
    int i;
    for(i = 0; i < cache_size; i++){
        char * comparison_address = our_cache->our_sets[i].address;
        if(comparison_address == address){
            printf("hello\n");
            return 1;
        }
    }
    return 0;
};

struct cache * helper_func(char * operation, char * address, int size, struct cache * our_cache){
    int opt = *operation;
    int add = 0;
    //adjust hits an misses for our cache in this switch statement
    switch(opt){
        case 'I':
            break;
        case 'S':
            //TODO a miss, needs to use write-allocate and needs to implement a dirty bit
            //I think it's supposed to record a miss or hit, and then either way, we need
            //to label a dirty bit so that it gets copied to main memory when it gets evicted
            //
            add = in_cache(our_cache, address);
            our_cache->hits = our_cache->hits + add;
            our_cache->misses = our_cache->misses + 1 - add;
            break;
        case 'L':
            //
            add = in_cache(our_cache, address);
            our_cache = place_in_cache(our_cache, address);
            our_cache->hits = our_cache->hits + add;
            our_cache->misses = our_cache->misses + 1 - add;
            our_cache->evictions = our_cache->evictions + 1 - add;
            break;
        case 'M':
            add = in_cache(our_cache, address);
            our_cache = place_in_cache(our_cache, address);
            our_cache->hits = our_cache->hits + add;
            our_cache->misses = our_cache->misses + 1 - add;
            our_cache->evictions = our_cache->evictions + 1 - add;
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
    struct cache * our_cache = cache_new(s);
    

    //read file line by line from text file and process
    FILE *file = fopen(t, "r");
    char operation[2], address[8];
    int size;
    while(fscanf(file, " %s %s, %d", operation, address, &size) != -1){
        our_cache = helper_func(&operation[0], &address[0], size, our_cache);
        printf("operation, address, size: %s %s %d\n", operation, address, size);
    }
    

    printf("s, E, b, t: %d %d %d %s\n",s,E,b,t);

    printSummary(our_cache->hits, our_cache->misses, our_cache->evictions);

    cache_delete(our_cache);
    return 0;
}
