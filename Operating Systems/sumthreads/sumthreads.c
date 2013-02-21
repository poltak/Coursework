#include <stdio.h>
#include <stdlib.h>     /* for use of exit() function */
#include <pthread.h>    /* for use of POSIX threads, use -lpthread gcc argument */
#include <math.h>       /* for use of pow() function, use -lm gcc argument */
#include <sys/time.h>   /* for use of gettimeofday() function and timeval structs */

/* global variables */
int increment;          /* needed by each thread to work out what part of the interval to operate on */
int total;              /* holds subtotal for all parallel summations (added to each time a thread finishes) */

/* function declarations */
void sum_threads(int);
void * handle_new_thread(void *);
void print_thread_info(int, int, int, int);


int main(int argc, char *argv[])
{
    int numthreads;

    if (argc != 3)      /* makes sure both arguments are entered */
    {
        fprintf(stderr, "Usage: %s <numthreads> <increment>\n\tnumthreads - how many threads to create\n\tincrement - # of int values thread will work with\n", argv[0]);
        return 1;
    }
    numthreads = atoi(argv[1]);
    increment = atoi(argv[2]);

    sum_threads(numthreads);

    return 0;
}

/*
 *  handles creating threads and printing the total sum from all the created threads
 */
void sum_threads(int numthreads)
{
    int rc, i;
    total = 0;
    pthread_t tid;

    tid = pthread_self();

    for (i = 0; i < numthreads; i++)
    {
        rc = pthread_create(&tid, NULL, handle_new_thread, (void *) i); /* creates new thread which runs handle_new_thread() passing the thread number to it */
        if (rc)
        {
            fprintf(stderr, "ERROR: return code from pthread_create is %d\n", rc);
            exit(1);
        }
        /* uncomment this to allow sequencial running of threads */
        /*pthread_join(tid, NULL); */
    }
    /*pthread_join(tid, NULL);   ** this was the original design to wait for all threads by joining to the last created thread,
     * but because they were running concurrently, occasionally there would be an earlier thread that took longer time which the join would not wait for
     * (most likely due to system overheads and other system processes running) and thus prematurely terminated the program before
     * all sub-calculations could be summed together into a total.
     * So in the end I chose to just sleep for 1 second to wait for all the threads as even for thousands of threads, 1 second would be enough time
     * (on the development machine). I have left it here as an explanation of why I didn't use pthread_join() to wait
     */
    sleep(1);

    printf("Total sum: %d\n", total);   /* print out the total summation of all the threads values */
}

/*
 *  sets up variables and bounds for the given thread before passing control to the main computation function pow_summation()
 *  also adds the given thread's summation total to the overall total
 *  parameters- the thread's number in sequence
 */
void * handle_new_thread(void *data)
{
    int lowerbound, upperbound, thread_number, s;

    thread_number = (int) data;                    /* this is the thread's number (first thread is 0, last is 'numthreads') */
    lowerbound = thread_number * increment;        /* gets the lower bound of the summation interval for this thread */
    upperbound = (thread_number +1) * increment;   /* gets the upper bound ''               ''                   ''  */
    struct timeval start_time, end_time;
    long int running_time;

    gettimeofday(&start_time, NULL);

    pthread_detach(pthread_self());                /* lets storage for this given thread be reclaimed (useful as we might want many threads) */

    s = pow_summation(lowerbound, upperbound);
    /*
     * only enable this if you want to print output from each thread. I have disabled it as the output
     * will be messy due ot all the threads running parallel and the sequence of output will be unpredictable
     */
    /* print_thread_info(thread_number, lowerbound, upperbound, s);  */
    total += s;

    gettimeofday(&end_time, NULL);
    running_time = end_time.tv_usec - start_time.tv_usec;   /* get difference between times */

    printf("thread %d- time taken: %ldus\n", thread_number, running_time);  /* print out running time in microseconds */
    pthread_exit(NULL);
}


/*
 *  main computation function which gets the summation of squares between the interval [lowerbound, upperbound]
 *  returns the summation
 */
int pow_summation(int lowerbound, int upperbound)
{
    int i;
    int s = 0;

    for (i = lowerbound; i < upperbound; i++)
        s += pow(i,2);

    return s;
}

/*
 *  handles all the pretty output. not really necessary but makes it easier to see and understand what is happening without reading code
 *  parameters- self explanatory; needed for making pretty output
 *  optional function; NOT USED UNLESS UNCOMMENTED
 */
void print_thread_info(int thread_number, int lowerbound, int upperbound, int s)
{
    int difference = upperbound - lowerbound;

    printf("Thread %d, ", thread_number);
    switch (difference)     /* handles formatting the calculation depending on the interval */
    {
        case 0:
            printf("not calculating anything... ");
            break;
        case 1:
            printf("calculating %d^2: ", lowerbound);
            break;
        case 2:
            printf("calculating (%d^2 + %d^2): ", lowerbound, upperbound -1);
            break;
        case 3:
            printf("calculating (%d^2 + %d^2 + %d^2): ", lowerbound, lowerbound +1, upperbound -1);
            break;
        default:
            printf("calculating (%d^2 + %d^2 + ... + %d^2): ", lowerbound, lowerbound +1, upperbound -1);
    }
    printf("%d\n", s);
}
