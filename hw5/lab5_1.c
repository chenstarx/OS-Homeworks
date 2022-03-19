#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define NAME "lab4"
#define IN  pidx[0]
#define OUT pidx[1]

#define BUF_LEN 3

int main(int argc, char *argv[])
{
    long n;
    int shm_fd;
    float *arr;
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

    printf("Buffer len: %d, n: %ld\n", BUF_LEN, n);

    // open a shared memory segment
    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);

    // limit the size
    int buf_size_byte = BUF_LEN * sizeof(float) + 2 * sizeof(int);
    ftruncate(shm_fd, buf_size_byte);

    // map the shared memory to current process address space
    arr = mmap(0, buf_size_byte, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // get the pointer for IN and OUT
    int* pidx = (int*) &arr[BUF_LEN];

    // init IN and OUT
    IN = 0;
    OUT = 0;

    // creat child process
    pid = fork();

    if (pid < 0)
    {
        printf("Error happend\n");
        return 1;

    } else if (pid == 0) {
    
        printf("Child start\n");

        for (int i = 0; i < n; i++)
        {
            while (((IN + 1) % BUF_LEN) == OUT);

            float sent = 0.25 * i * i;

            printf("\nChild sending %f\n", sent);
            arr[i%BUF_LEN] = sent;
            IN = (IN + 1) % BUF_LEN;

            int nanosec = rand() % 5000000;

            printf("Child sleeping %.3fs\n", (float) nanosec / 1000000.0);
            usleep(nanosec);

        }

        printf("Child finished\n");

    } else {

        printf("Parent start\n");

        while (1)
        {
            /* wait till in != out*/
            while (IN == OUT);

            printf("Parent received: %f\n", arr[OUT]);

            OUT = (OUT + 1) % BUF_LEN;

        }

        shm_unlink(NAME);
        printf("Parent finished\n");
    }

    return 0;
}
