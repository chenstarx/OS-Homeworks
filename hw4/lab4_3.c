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

int main(int argc, char *argv[])
{
    long n;
    int shm_fd;
    float *arr;
    pid_t pid;

    if (argc == 2)
    {
        n = strtol(argv[1], NULL, 10);
    }

    if (!(n > 0))
    {
        printf("Please provide a valid argument larger than 0\n");
        return 1;
    }

    // open a shared memory segment
    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);

    // limit the size
    int buf_size_byte = n * sizeof(float) + 2 * sizeof(int);
    ftruncate(shm_fd, buf_size_byte);

    // map the shared memory to current process address space
    arr = mmap(0, buf_size_byte, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // get the pointer for IN and OUT
    int* pidx = (int*) &arr[n];

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
            while (((IN + 1) % n) == OUT);

            float sent = 0.25 * i * i;

            arr[i] = sent;

            int nanosec = rand() % 5000000;

            usleep(nanosec);
            printf("\nChild slept %.3fs\n", (float) nanosec / 1000000.0);

            printf("Child sending %f\n", sent);
            IN = (IN + 1) % n;
        }

        printf("Child finished\n");

    } else {

        printf("Parent start\n");

        while (1)
        {
            /* wait till in != out*/
            while (IN == OUT);

            printf("Parent received: %f\n", arr[OUT]);

            OUT = (OUT + 1) % n;

            if (OUT == 0)
            {
                goto end;
            }
        }

        end:
        wait(NULL);
        shm_unlink(NAME);
        printf("Parent finished\n");
    }

    return 0;
}
