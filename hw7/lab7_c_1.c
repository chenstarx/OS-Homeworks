#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define NAME "lab7_c"
#define IN  pidx[0]
#define OUT pidx[1]

#define BUF_LEN 3

long n = 2;

int main(int argc, char *argv[])
{
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
    printf("Address of n: %p\n", &n);

    // open a shared memory segment
    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);

    // limit the size
    int buf_size_byte = BUF_LEN * sizeof(float) + 2 * sizeof(int);
    ftruncate(shm_fd, buf_size_byte);

    // map the shared memory to current process address space
    arr = mmap(0, buf_size_byte, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    printf("Start address of shared buffer: %p\n", &arr);

    // get the pointer for IN and OUT
    int* pidx = (int*) &arr[BUF_LEN];

    // init IN and OUT
    IN = 0;
    OUT = 0;

    printf("Consumer start\n");

    int count = 0;

    while (1)
    {
        /* wait till in != out*/
        while (IN == OUT);

        printf("Consumer received: %f\n", arr[OUT]);

        OUT = (OUT + 1) % BUF_LEN;

        count += 1;
        if (count >= n) break;

    }

    shm_unlink(NAME);
    printf("Consumer finished\n");

    return 0;
}
