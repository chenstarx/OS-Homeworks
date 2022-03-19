#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{
    long n;
    pid_t pid;

    if (argc != 2)
    {
        printf("Please provide a valid argument\n");
        return 1;
    }

    n = strtol(argv[1], NULL, 10);

    if (!(n > 0))
    {
        printf("Please provide a valid argument larger than 0\n");
        return 1;
    }

    int fd[2];

    /* create the pipe */
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return -1;
    }

    // init buffer
    float send_buf[0];
    float recv_buf[0];

    // creat child process
    pid = fork();

    if (pid < 0)
    {
        printf("Error happend\n");
        return 1;

    } else if (pid == 0) {
    
        printf("Child start\n");
        close(fd[READ_END]);

        for (int i = 0; i < n; i++)
        {

            float sent = 0.25 * i * i;
            send_buf[0] = sent;
            printf("\nChild sending %f\n", sent);

            write(fd[WRITE_END], send_buf, sizeof(float));

            int nanosec = rand() % 5000000;

            printf("Child sleeping %.3fs\n", (float) nanosec / 1000000.0);
            usleep(nanosec);

        }

        printf("Child finished\n");
        close(fd[WRITE_END]);

    } else {

        printf("Parent start\n");
        close(fd[WRITE_END]);

        while (1)
        {
            int size = read(fd[READ_END], recv_buf, sizeof(float));

            if (size == 0) break;

            printf("Parent received: %f\n", recv_buf[0]);

        }

        printf("Parent finished\n");
        close(fd[READ_END]);
    }

    return 0;
}
