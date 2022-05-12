#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/wait.h>
#define NUM_PROCS 5
#define RD_END 0
#define WR_END 1
#define MAX_MESS 100

int runner(int id, int fd_left[2], int fd_right[2])
{
    printf("Child %d start\n", id);
    int num[0];
    char buffer[MAX_MESS];
    while (1)
    {
        int size_num = read(fd_left[RD_END], num, sizeof(int));
        int size_str = read(fd_left[RD_END], buffer, MAX_MESS);

        if (size_num == 0 || size_str == 0) break;

        if (id != NUM_PROCS - 1) {
            write(fd_right[WR_END], num, size_num);
            write(fd_right[WR_END], buffer, size_str);
        }

        if (num[0] == -1) {
            printf("Child %d exit\n", id);
            break;
        };

        if (id == num[0])
            printf("Child %d received '%s'\n", id, buffer);

    }

}

int main(int argc,char *argv[])
{

    // create the ordinary pipes
    // HINT: This is created before any of the processes and is 
    // thus visible to all of them
    int pipes[NUM_PROCS][2];

    for (int i = 0; i < NUM_PROCS; i++) {
        /* create the pipe */
        if (pipe(pipes[i]) == -1) {
            fprintf(stderr, "Pipe failed");
            return -1;
        }
    }

    // create the child processes
    int flag = 0;
    pid_t pids[NUM_PROCS];
    // start children
    for (int i = 0; i < NUM_PROCS; i++) {
        if ((pids[i] = fork()) < 0) {
            printf("Error happend\n");
            return 1;
        } else if (pids[i] == 0) {
            if (i == NUM_PROCS - 1)
                runner(i, pipes[i], pipes[i]);
            else
                runner(i, pipes[i], pipes[i + 1]);
            return 0;
        }
    }

    // parent code
    int num;
    char mess_str[MAX_MESS];
    while (1) {
        
        sleep(1);
        printf("\nEnter destination and message string: ");
        scanf("%d %s", &num, mess_str);

        write(pipes[0][WR_END], &num,  sizeof(int));
        write(pipes[0][WR_END], mess_str,  strlen(mess_str) + 1);

        if (num == -1) break;

    }

    // wait for children to exit
    for (int i = 0; i < NUM_PROCS; ++i) {
        wait(NULL);
    }

    // parent closes all pipes
    for (int i = 0; i < NUM_PROCS; i++) {
        close(pipes[i][RD_END]);
        close(pipes[i][WR_END]);
    }

    printf("Program finished\n");
    return 0;
}