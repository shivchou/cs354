////////////////////////////////////////////////////////////////////////////////
// Main File: csim.c file (the cache simulator) does not have a main function.
// This File: csim.c
// Other Files: this file used to run trace files 
// Semester: CS 354 Lecture 03, Fall 2024
// Grade Group: gg19 (See canvas.wisc.edu/groups for your gg#)
// Instructor: Hina Mahmood
//
// Author: Shivani Choudhary
// Email: choudhary26@wisc.edu
// CS Login: shivani
//
/////////////////////////// WORK LOG //////////////////////////////
// Document your work sessions on your copy http://tiny.cc/work-log
// Download and submit a pdf of your work log for each project.
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
// Persons: Identify persons by name, relationship to you, and email.
// Describe in detail the the ideas and help they provided.
//
// Online sources: avoid web searches to solve your problems, but if you do
// search, be sure to include Web URLs and description of
// of any information you find.
//
// AI chats: save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2024 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes.
// Used by permission SPRING 2024, CS354-deppeler
//
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Copyright 2013,2019-2024
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission for Fall 2024
////////////////////////////////////////////////////////////////////////////////

/**
 * csim.c:  
 * Simulatate the contents of a cache with given configuration and 
 * count the number of hits, misses, and evictions for a given 
 * sequence of memory accesses for a program.
 *
 * If you want to create your own memory access traces, you can 
 * use valgrind to output traces that this simulator can analyze.
 *
 * What is the replacement policy implemented?  ___________________________
 *      LRU
 * Implementation and assumptions:
 *  1. (L) load or (S) store cause at most one cache miss and a possible eviction.
 *  2. (I) Instruction loads are ignored.
 *  3. (M) Data modify is treated as a load followed by a store to the same
 *     address. Hence, an (M) operation can result in two cache hits, 
 *     or a miss and a hit plus a possible eviction.
 *
 * Find the TODO tags to see where to make chanes for your simulator.
 */  

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/******************************************************************************/
/* DO NOT MODIFY THESE VARIABLE NAMES and TYPES                               */
/* DO UPDATE THEIR VALUES AS NEEDED BY YOUR CSIM                              */

//Globals set by command line args.
int b = 0; //number of (b) bits
int s = 0; //number of (s) bits
int E = 0; //number of lines per set

//Globals derived from command line args.
int B; //block size in bytes: B = 2^b
int S; //number of sets: S = 2^s

//Global counters to track cache statistics in access_data().
int hit_cnt = 0;
int miss_cnt = 0;
int evict_cnt = 0;

//Global to control trace output
int verbosity = 0; //print trace if set
/******************************************************************************/


// Type mem_addr_t: stores an addresses or address masks.
typedef unsigned long long int mem_addr_t;

// Type cache_line_t: stores "overhead" (v-bit and tag) for a cache line
// The cache block's data is not needed or stored for this simulator.
//TODO - COMPLETE THIS TYPE
typedef struct cache_line {                    
    char valid;
    mem_addr_t tag;
    //Add a data member as needed by your implementation

    //counter to track recent usage
    long long counter;
} cache_line_t;

// Type cache_set_t: Stores a pointer to the first cache line in a set.
// Note: Each set is a pointer to a heap array of one or more cache lines.
typedef cache_line_t* cache_set_t;

// Type cache_t: Stores a pointer to the first set in a cache
// Note: The cache is a pointer to a heap array of one or more sets.
typedef cache_set_t* cache_t;

// Create the cache we're simulating. 
// Note: The cache is a pointer to a heap array of one or more sets.
cache_t cache;  

/*
 * init_cache:
 * Allocates the data structure for a cache with S sets and E lines per set.
 * Initializes all valid bits and tags with 0s.
 */                    
void init_cache() 
{    
    //calculate block size and number of sets
     S = 1 << s;
     B = 1 << b;

    //allocate cache with specified number of sets
    cache = (cache_set_t*)malloc(S * sizeof(cache_set_t));

    //check malloc
    if(cache == NULL)
    {
        printf("cache unable to be initialized \n");
        exit(1);
    }

    //allocate lines in sets in cache
    for(int i = 0; i < S; i++)
    {
        cache[i] = (cache_line_t*)malloc(E * sizeof(cache_line_t));

        //check malloc
        if(cache[i] == NULL)
        {
            printf("cache set unable to be initialized \n");
            exit(1);
        }

        //initialize valid, tag, and counter fields for cache lines
        for (int j = 0; j < E; j++) 
        {
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].counter = 0;
        }
    }
}


/*
 * free_cache:
 * Frees all heap allocated memory used by the cache.
 */                    
void free_cache() 
{   
    //free sets, then free cache 
    for(int i = 0; i < S; i++)
    {
        free(cache[i]);
        cache[i] = NULL;
    }

    free(cache);

    cache = NULL;
   
}

/* 
 * findMax:
 * find the biggest counter in the set
 */     
int findMax(int set)
{
    int max = cache[set][0].counter;
    for(int i = 0; i < E; i++)
    {
        for(int j = i; j < E; j++)
        {
            if(cache[set][j].counter > max)
            {
                max = cache[set][j].counter;
            }
        }
    }
    return max;
}

/* 
 * findMin:
 * find the smallest counter in the set for eviction purposes
 */   
int findMin(int set)
{
    int min = cache[set][0].counter;
    for (int i = 0; i < E; i++)
    {
        for(int j = i; j < E; j++)
        {
            if(cache[set][j].counter < min)
            {
                min = cache[set][j].counter;
            }
        }
    }
    return min;
}
 
/*
 * access_data:
 * Simulates data access at given "addr" memory address in the cache.
 *
 * If block containing addr is already in cache, increment hit_cnt
 * If block containing addr is not in cache, 
 *    cache it (set tag and valid), increment miss_cnt
 * If a block in a cache line is evicted, increment evict_cnt
 */                    
void access_data(mem_addr_t addr) 
{      
    //find tag bits
    int tBits = (int)addr >> (s+b);

    //find set
    int set = ((int)(addr) >> b) & (S - 1);


    //access set and tag
    //mem_addr_t tag = cache[tBits];

    int maxCounter = findMax(set);

    int minCounter = findMin(set);

    //check for a hit; if found, increment hit counter
    for (int i = 0; i < E; i++) 
    {
        if (cache[set][i].valid && cache[set][i].tag == tBits) 
        {
            hit_cnt++;
            cache[set][i].counter = maxCounter + 1;
            return;
        }
    }

    //if miss, increment miss counter
    miss_cnt++;

    //find empty line or replace using LRU policy
    for (int i = 0; i < E; i++){
        if (cache[set][i].valid == 0){
            cache[set][i].valid = 1;
            cache[set][i].tag = tBits;
            cache[set][i].counter = maxCounter + 1;
            return;
        }
    }
    
    //increase eviction count
    evict_cnt++;
    //use the empty line if found
    for (int i = 0; i < E; i++)
    {
        if (cache[set][i].counter == minCounter)
        {
            cache[set][i].valid = 1;
            cache[set][i].tag = tBits;
            cache[set][i].counter = maxCounter + 1;
            return;
        }
    }
}


/* TODO - FILL IN THE MISSING CODE
 * replay_trace:
 * Replays the given trace file against the cache.

 Example: subset of trace, shows type of access, address, size of access

 L 7ff0005b8,4
 S 7feff03ac,4
 M 7fefe059c,4
 
 * Reads the input trace file line by line.
 * Extracts the type of each memory access : L/S/M
 * TRANSLATE "L" as a Load i.e. one memory access
 * TRANSLATE "S" as a Store i.e. one memory access
 * TRANSLATE "M" as a Modify which is a load followed by a store, 2 mem accesses 
 */                    
void replay_trace(char* trace_fn) {           
    char buf[1000];  
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r"); 

    if (!trace_fp) { 
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);   
    }

    while (fgets(buf, 1000, trace_fp) != NULL) { 
        if (buf[1] == 'S' || buf[1] == 'L' || buf[1] == 'M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);

            if (verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

            // TODO - MISSING CODE
            // GIVEN: 1. addr has the address to be accessed
            //        2. buf[1] has type of acccess(S/L/M)
            // call access_data function here depending on type of access
			// may use nested if-else if or switch statement to select
            switch (buf[1]) {
                case 'L':
                    access_data(addr);
                    break;
                case 'S':
                    access_data(addr);
                    break;
                case 'M':
                    access_data(addr);  // Load
                    access_data(addr);  // Store
                    break;
            }

            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}  


/*
 * print_usage:
 * Print information on how to use csim to standard output.
 */                    
void print_usage(char* argv[]) {                 
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Verbose flag.\n");
    printf("  -s <num>   Number of s bits for set index.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of b bits for word and byte offsets.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}  


/*
 * print_summary:
 * Prints a summary of the cache simulation statistics to a file.
 */                    
void print_summary(int hits, int misses, int evictions) {                
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}  


/*
 * main:
 * parses command line args, 
 * makes the cache, 
 * replays the memory accesses, 
 * frees the cache and 
 * prints the summary statistics.  
 */                    
int main(int argc, char* argv[]) {                      
    char* trace_file = NULL;
    char c;

    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while ((c = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch (c) {
            case 'b':
                b = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'h':
                print_usage(argv);
                exit(0);
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbosity = 1;
                break;
            default:
                print_usage(argv);
                exit(1);
        }
    }

    // Make sure that all required command line args were specified.
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        print_usage(argv);
        exit(1);
    }

    // Initialize cache.
    init_cache();

    // Replay the memory access trace.
    replay_trace(trace_file);

    // Free memory allocated for cache.
    free_cache();

    // Print the statistics to a file.
    // DO NOT REMOVE: This function must be called for test_csim to work.
    print_summary(hit_cnt, miss_cnt, evict_cnt);
    return 0;   
}  

//			202409                                     

