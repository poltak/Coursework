#include <stdio.h>
#include <stdlib.h>         /* used for pseudo-random number generator */
#include <time.h>           /* for use as a random seed that will be different each time program is run */
#include "test_factors.h"   /* all my factors used for testing; please edit! */

/* global constants */
#define EMPTY       -1
#define FRAG        -2


/* used to define a type for processes */
typedef struct {
    int name;
    int size;
    int time;
} process;

/* used to define type for bools */
typedef int bool;
#define true    1
#define false   0

/* global variables */
int memory[MAX_BLOCKS];         /* array to represent memory; each index representing a single block of 1KB size */

/* function declarations */
void init_mem();
void pseudo_random_fit(process []);



/* Initialise all values of memory to EMTPY value. */
void init_mem()
{
    int i;

    for (i = 0; i < MAX_BLOCKS; i++)
        memory[i] = EMPTY;
}

/* Initialises a process and returns it. */
process init_process(int init_name, int init_size, int init_time)
{
    process p;

    p.name = init_name;
    p.size = init_size;
    p.time = init_time;

    return p;
}

/* Implements the pseudo-random fit algorithm.
 * Pretty much the most horrible thing ever.
 */
void pseudo_random_fit(process p_arr[])
{
    process p, p_allocated[MAX_JOBS];
    int i, random_block, job_counter;
    bool success;

    srand(time(NULL));      /* makes random number seed different each time program is run */
    job_counter = 0;

    while (job_counter < MAX_JOBS)
    {
        p = p_arr[job_counter];
        random_block = get_rand(MAX_BLOCKS);    /* get a random block to allocate to */

        success = allocate(p, random_block);
        if (!success)       /* if job didn't get allocated (not enough contiguous free memory */
        {
            printf("Process %d (%dKB) not allocated to random block %d: not enough contiguous space.\n\n", p.name, p.size, random_block);
            job_counter++;
            continue;   /* try again with different random number */
        }

        p_allocated[job_counter] = p;   /* add allocated process to array that stores the allocated processes */
        printf("Process %d (%dKB) allocated succesfully to random block %d.\n\n", p.name, p.size, random_block);

        job_counter++;      /* go onto next job as current is allocated */
    }
}


/* Allocates the given process to memory.
 * returns true if there is room and allocated
 * returns false if no room
 */
bool allocate(process p, int random_block)
{
    int i;
    int rounded_size;

    if ((p.size & (p.size -1)) == 0)        /* if p.size is power of 2 */
        rounded_size = p.size;
    else
        rounded_size = upper_power_of_two(p.size);


    if (enough_contig_space(random_block, rounded_size))
    {
        for (i = 0; i < p.size; i++)
            memory[random_block +i] = p.name;
        for (; i < rounded_size; i++)
            memory[random_block +i] = FRAG;

        return true;
    }

    return false;       /* not enough contiguous space to allocate process */
}

/* Deallocates the given process from memory.
 * Returns true if process exists in memory and is deleted.
 * Returns false if process doesn't exist in memory.
 */
bool deallocate(process p)
{
    int i, start_index, rounded_size;

    start_index = EMPTY;
    if ((p.size & (p.size -1)) == 0)
        rounded_size = p.size;
    else
        rounded_size = upper_power_of_two(p.size);

    for (i = 0; i < MAX_BLOCKS; i++)    /* find start block of process */
    {
        if (memory[i] == p.name)
        {
            start_index = i;
            break;
        }
    }

    if (start_index == EMPTY)       /* process not found */
        return false;

    for (i = start_index; i < (start_index + rounded_size); i++)  /* actual purging of process */
        memory[i] = EMPTY;

    return true;
}

/* Returns closest power of 2 rounding up from v using bitwise ops. */
int upper_power_of_two(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

/*
 * Checks to make sure the _whole_ process can fit in space starting at start_index.
 * So from memory[start_index] to memory[start_index + size] must be free to return true.
 */
bool enough_contig_space(int start_index, int size)
{
    int i;

    for (i = start_index; i < (start_index + size); i++)
        if (memory[i] != EMPTY)
            return false;

    return true;
}

/* Simple print function to see contents of memory. */
void print_mem()
{
    int i;

    printf("\nMemory's final state:\n");
    for (i = 0; i < MAX_BLOCKS; i++)
    {
        printf("Block %d:\t", i);

        if (memory[i] == EMPTY)
            printf("EMPTY\n");
        else if (memory[i] == FRAG)
            printf("UNUSED\n");
        else
            printf("%d\n", memory[i]);
    }
}

/* Returns a pseudo-random number in interval [0,max]. */
int get_rand(int max)
{
    return (rand() % max);
}

int main(int argc, char *argv[])
{
    int i;
    process p_arr[MAX_JOBS];

    init_mem();

    for (i = 0; i < MAX_JOBS; i++)
        p_arr[i] = init_process(i, get_rand(MAX_JOB_SIZE) +2, get_rand(MAX_RUNTIME) +1);

    pseudo_random_fit(p_arr);
    print_mem();
}

