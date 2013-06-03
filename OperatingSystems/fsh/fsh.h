#include <stdio.h>
#include <string.h>         /* for string functiosn */
#include <stdlib.h>         /* used for mem allocation for given env variables */
#include <signal.h>         /* only used for SIGTERM resolution */
#include <unistd.h>         /* process management */
#include <sys/stat.h>       /* access to stat syscall for checking if directory exists in system */
#include <sys/wait.h>

/* function prototypes */
void cmd_handler(int, char *);
void set_env_vars(void);
void trim_line_break(char *);

void run_clr(void);
void run_dir(char *);
void run_environ(void);
void run_echo(char *);
void run_help(void);
void run_pause(void);
void run_quit(void);
void run_cd(char *);
int  run_shell();
int  is_mult_slash(char *);

/* global constants */
#define DELIMITER   " "
#define BYTE_LIM    512
#define OOM         1
#define TRUE        1
#define FALSE       0
#define CHILD_PID   0
