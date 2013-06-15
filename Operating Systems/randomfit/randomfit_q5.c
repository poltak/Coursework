#include <stdio.h>
#include <stdlib.h>         /* used for pseudo-random number generator */
#include <time.h>           /* for use as a random seed that will be different each time program is run */
#include "test_factors.h"   /* all my factors used for testing; please edit! */

/* global constants */
#define EMPTY           -1
#define FRAG            -2


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
int mem_size;
int *memory;         /* array to represent memory; each index representing a single block of 1KB size */

/* function declarations */
void init_mem();
void pseudo_random_fit(process []);
void average_frag();


/* Initialise all values of memory to EMTPY value. */
void init_mem()
{
    int i;

    for (i = 0; i < mem_size; i++)
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
    int wait_counter;

    srand(time(NULL));      /* makes random number seed different each time program is run */
    job_counter = 0;
    wait_counter = 0;

    while (job_counter < MAX_JOBS)
    {
        p = p_arr[job_counter];
        random_block = get_rand(mem_size);    /* get a random block to allocate to */

        for (i = 0; i < job_counter; i++)       /* checks each allocated job each iteration, to see if any are completed and deallocates memory if so */
            if (p_allocated[i].time == 0)
                deallocate(p_allocated[i]);

        success = allocate(p, random_block);
        if (!success)       /* if job didn't get allocated (not enough contiguous free memory */
        {
            /*printf("Process %d not allocated to random block %d: not enough contiguous space.\n", p.name, random_block);*/

            for (i = 0; i < job_counter; i++)       /* decrement time for each allocated process */
                p_allocated[i].time--;

            wait_counter++;
            continue;   /* try again with different random number */
        }
        /*printf("Process %d (%dKB, %dsec): rerolled %d times\n\n", p.name, p.size, p.time, wait_counter);*/

        wait_counter = 0;
        for (i = 0; i < job_counter; i++)       /* decrement time for each allocated process */
            p_allocated[i].time--;

        p_allocated[job_counter] = p;   /* add allocated process to array that stores the allocated processes */

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

    for (i = 0; i < mem_size; i++)    /* find start block of process */
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
    for (i = 0; i < mem_size; i++)
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

    mem_size = (get_rand(MAX_BLOCKS) +100);
    memory = malloc(sizeof(int *) * mem_size);

    srand(time(NULL));
    init_mem();

    for (i = 0; i < MAX_JOBS; i++)
        p_arr[i] = init_process(i, get_rand(MAX_JOB_SIZE) +2, get_rand(MAX_RUNTIME) +1);

    pseudo_random_fit(p_arr);
    /*print_mem();*/
    average_frag(p_arr);
}

/*
 * Calculates the average internal fragmentation (in KB)
 * and the average external fragmentation (in a percentage of the total size of memory)
 */
void average_frag(process p_arr[])
{
    int i, j, no_of_processes, total_int_frag;
    bool in_process;

    no_of_processes = 0;
    total_int_frag = 0;
    in_process = false;

    /* average internal fragmentation calculations */
    for (i = 0; i < mem_size; i++)            /* gets number of processes allocated (in a horrible way) */
    {
        if (memory[i] != EMPTY && !in_process)  /* if process is encountered */
        {
            j = memory[i];
            in_process = true;
            no_of_processes++;
        }
        if ((memory[i] != j && memory[i] != FRAG) && in_process)    /* reset in_process status when leaving process */
            in_process = false;
    }

    for (i = 0; i < mem_size; i++)        /* gets total number of internal fragmentation in KB */
        if (memory[i] == FRAG)
            total_int_frag++;

    printf("Average internal fragmentation (per allocated process): %2.2fKB\n", ((double) total_int_frag / (double) no_of_processes));

    /* average external fragmentation calculations */
    int largest_block_size, tmp_block_size, total_free_blocks;
    long l;
    double avg_ext_frag;

    largest_block_size = 0;
    tmp_block_size = 0;
    for (i = 0; i < mem_size; i++)        /* gets largest free block size */
    {
        if (memory[i] == EMPTY)
            tmp_block_size++;
        if (memory[i] != EMPTY)
        {
            if (tmp_block_size > largest_block_size)
                largest_block_size = tmp_block_size;
            tmp_block_size = 0;
        }
    }
    if (tmp_block_size > largest_block_size)
        largest_block_size = tmp_block_size;

    for (i = 0; i < mem_size; i++)        /* gets total number of external fragmented blocks in KB */
        if (memory[i] == EMPTY)
            total_free_blocks++;

    avg_ext_frag = 1.0 - ((double) largest_block_size / (double) total_free_blocks);
    printf("Average external fragmentation (out of total size of memory): %3.2f%\n", (avg_ext_frag *100));
}
