#define MAX_BLOCKS      64      /* size of memory in KB */
#define MAX_JOBS        10      /* number of processes to fit in memory */
#define MAX_RUNTIME     15      /* max time units a process will take to complete */
#define MAX_JOB_SIZE    12      /* max size a process will take up in KB (gets rounded up to nearest pow of 2 to allocate correctly, but takes into account internal fragmentation in allocate() function) */
