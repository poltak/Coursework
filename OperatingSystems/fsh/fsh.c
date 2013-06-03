#include "fsh.h"

/* global environment variables */
char *user;
char *pwd;
char *home;
char *shell;

/* global variables unrelated to environment */
int run_signal = TRUE;
char *cmd_arr[] = {
    "clr",
    "dir",
    "echo",
    "environ",
    "help",
    "pause",
    "quit",
    "cd"
};

int main(int argc, char *argv[])
{
    /* allocate memory to environment variables; give them limit of 512 bytes inclusive of '\0' at end of string */
    user  = (char *) malloc(BYTE_LIM);
    pwd   = (char *) malloc(BYTE_LIM);
    home  = (char *) malloc(BYTE_LIM);
    shell = (char *) malloc(BYTE_LIM);

    int return_val = run_shell();

    if (return_val == EOF)
    {
        fprintf(stderr, "\nIO Error encountered, exiting program.\n");
        return EOF;
    }
    if (return_val == OOM)
    {
        fprintf(stderr, "Out of memory, exiting program.\n");
        return OOM;
    }

    printf("Goodbye.\n");
    return 0;
}

int run_shell()
{
    char *command;
    char *commandtmp;
    char *cmd_token;
    char *arg_token;
    int cmd_arr_length = sizeof(cmd_arr) / sizeof(char *);  /* gets length of command array */
    int prompt = '$';
    int i;
    int invalid_cmd;
    int bytes_read;
    int nbytes = BYTE_LIM;

    set_env_vars();

    printf("Welcome to failshell!\n");

    /* allocates appropriate mem to command, limiting user's command and setting up temporary container string to split */
    command = (char *) malloc(nbytes);
    commandtmp = (char *) malloc(sizeof(command));
    if (commandtmp == NULL)     return OOM;     /* if no space left to allocate (highly unlikely...) */

    /* the main loop that powers the shell */
    while (run_signal != EOF)
    {
        invalid_cmd = TRUE;
        printf("%s %s %c ", user, pwd, prompt);

        /* get command string from user and place into address pointed at by *command */
        bytes_read = getline(&command, (size_t *) &nbytes, stdin);
        if (bytes_read == EOF)      /* bytes_read will only = EOF if some IO error or interrupt (including Ctrl+D) */
            return EOF;

        strcpy(commandtmp, command);

        cmd_token = (char *) malloc(sizeof(command));
        arg_token = (char *) malloc(sizeof(command));

        /* split entered command into tokens for command argument and second argument i.e., $0 and $1 */
        cmd_token = strtok(commandtmp, DELIMITER);
        arg_token = strtok(NULL, DELIMITER);

        /* had problems with '\n' value being put into strings, which messed up comparisons later; this takes them out if they exist */
        if (arg_token == NULL)  trim_line_break(cmd_token);   /* if no argument */
        else                    trim_line_break(arg_token);

        /* compares entered command with all valid commands, allowing handler to execute command if valid */
        for (i = 0; i < cmd_arr_length; i++)
        {
            if (strcmp(cmd_token, cmd_arr[i]) == 0)
            {
                invalid_cmd = FALSE;
                cmd_handler(i, arg_token);
            }
        }

        /* don't print error message if user entered valid command or if they didn't enter anything */
        if (invalid_cmd && strlen(cmd_token) > 1)
            printf("Command \"%s\" is invalid. Type \"%s\" for assistance.\n", cmd_token, cmd_arr[4]);
    }
    return 0;
}

/*
 * sets all the environment variables for fsh to point to the strings for the environment variables of the environment from which fsh was run from
 * (except *shell which I've made just a string constant)
 */
void set_env_vars()
{
    pwd = getenv("PWD");
    home = getenv("HOME");
    user = getenv("USER");
    shell = "fsh";
}

/*
 * really basic function to trim out line-breaks~ was having issues with line-breaks in my strings
 * not very portable and not recommended for use, but it is all I need for this context
 */
void trim_line_break(char *str)
{
    int last = strlen(str) -1;
    if (str[last] == '\n')  str[last] = '\0';
}

void cmd_handler(int cmd, char *arg)
{
    if (cmd < 6) /* handles first lot of commands as they all can be implemented by forking a child process easily */
    {
        pid_t pid = fork();

        if (pid > CHILD_PID)        /* parent process wait for child to finish before exiting function and going back to prompt */
        {
            wait(&pid);
        } else if (pid == CHILD_PID)  /* forked child process runs entered command */
        {
            switch (cmd)
            {
                case 0:
                    run_clr();
                    break;
                case 1:
                    run_dir(arg);
                    break;
                case 2:
                    run_echo(arg);
                    break;
                case 3:
                    run_environ();
                    break;
                case 4:
                    run_help();
                    break;
                case 5:
                    run_pause();
                    break;
                default:
                    printf("Congratulations! You've broken my code.");  /* I really don't think it's possible to get here... */
            }
        } else
        {
            fprintf(stderr, "Failed to fork process. Fatal error, exiting program.\n");
            exit(1);
        }
    } else    /* handles last commands: because of the way I've implemented the execution of the shell, it would be very inconvenient to handle them in a child process (without using shared memory) */
    {
        switch (cmd)
        {
            case 6:
                run_quit();
                break;
            case 7:
                run_cd(arg);
                break;
            default:
                printf("Again! You did it again!");         /* again, not really possible */
        }
    }
}

void run_clr() { execlp("/usr/bin/clear", "clear", NULL); }

/* allows calling with or without argument specified; without arg just lists pwd directory */
void run_dir(char *path)
{
    if (path == NULL)   execlp("/usr/bin/dir", "dir", "-hal", pwd, NULL);
    execlp("/usr/bin/dir", "dir", "-hal", path, NULL);
}

void run_echo(char *str) { execlp("/bin/echo", "echo", str, NULL); }

/* could have just executed system "printenv" command but it would print out environment variables not applicable to this shell; therefore I think this will suffice */
void run_environ()
{
    printf("SHELL=%s\nUSER=%s\nHOME=%s\nPWD=%s\n", shell, user, home, pwd);
    kill(getpid(), SIGTERM);
}

void run_help()
{
    int i = 0;
    printf("Jonathan Poltak Samosir's fsh, version 0.1\nValid commands:\n");
    /* prints all valid commands from cmd array */
    while (cmd_arr[i +2] != NULL)
        printf("%s\n", cmd_arr[i++]);

    kill(getpid(), SIGTERM);
}

void run_pause()
{
    printf("Press \"Enter\" button when ready to continue...\n");
    getchar();

    kill(getpid(), SIGTERM);
}

/* the way I have my while loop set up in run_shell(), this is the most simple way of quitting the program without outright killing the process; no real use for fork here */
void run_quit() { run_signal = EOF; }

/*
 *  I spent hours on this one. Because of the way I've implemented PWD as an env variable, forking a child process and changing
 *  directory from there would do mean nothing once control was passed back to the parent process. This is because their memory
 *  spaces are separate and even if the child updated PWD through a pointer, it would only update the PWD variable in the child's
 *  memory space. It was either implementing shared memory (IPC) so I could alter the parent's PWD variable through the child but
 *  this would become very messy and a lot of hard work for a single variable. In the end I got it working how I wanted without forking
 *  a new process in this function.
 */
void run_cd(char *path)
{
    struct stat st;

    if (path == NULL)           /* reports current directory as arg is not present (see PDF requirements) */
    {
        printf("Current directory is: %s\n", pwd);
        return;
    } else if (path[0] != '/')    /* checks and handles if user enters relative directory path */
    {
        printf("Please enter absolute directory path.\n");
        return;
    } else if (is_mult_slash(path))  /* checks for multiple /'s as it ruins my implementation of PWD */
    {
        printf("Please don't enter multiple '/' characters.\n");
        return;
    }

    /* checks if directory specified in path exists in system using stat syscall */
    if (stat(path, &st) == 0) strcpy(pwd, path);
    else    fprintf(stderr, "%s: %s: %s: No such directory or is a file.\n", shell, cmd_arr[0], path);
}

/*
 * simple function to check string for clusters of forward-slashes
 * I needed to make this function as Linux accepts multiple forward-slashes to mean a single slash,
 * although it messes up my PWD environment variable if I allow it in this shell
 */
int is_mult_slash(char* str)
{
    int slash = FALSE;
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '/' && slash == FALSE)        slash = TRUE;
        else if (str[i] == '/' && slash == TRUE)    return FALSE;   /* if there is any double slash found then return */
        else                                        slash = FALSE;  /* if there is no double slash so far */
        i++;
    }
}
