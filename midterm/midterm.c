#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_TRIALS 100000
#define NAME "midterm"
#define LNAME "lock"

void runner(int n, int* shm_hit, sem_t* shm_lock);

int main(int argc, char *argv[])
{
    int n;
    int shm_fd_hit;
    int shm_fd_lock;
    int *shm_hit;
    sem_t *shm_lock;

    if (argc != 2)
    {
        printf("Please provide an argument for number of students\n");
        return 1;
    }

    n = strtol(argv[1], NULL, 10);

    if (!(n > 0))
    {
        printf("Please provide a valid argument larger than 0\n");
        return 1;
    }

    printf("Students = %d\n", n);

    // open a shared memory segment
    shm_fd_hit = shm_open(NAME, O_CREAT | O_RDWR, 0666);
    shm_fd_lock = shm_open(LNAME, O_CREAT | O_RDWR, 0666);

    // limit the size
    ftruncate(shm_fd_hit, sizeof(int));
    ftruncate(shm_fd_lock, sizeof(sem_t));

    // map the shared memory to current process address space
    shm_hit = (int *) mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd_hit, 0);
    shm_lock = (sem_t*) mmap(0, sizeof(sem_t), PROT_WRITE, MAP_SHARED, shm_fd_lock, 0);

    sem_init(shm_lock, 1, 1);

    int flag = 0;
    pid_t pids[3];
    // start children
    for (int i = 0; i < 3; i++) {
        if ((pids[i] = fork()) < 0) {
            printf("Error happend\n");
            return 1;
        } else if (pids[i] == 0) {
            runner(n, shm_hit, shm_lock);
            return 0;
        } else {
            // use flag to make sure parent only run once
            if (flag == 0) {
                runner(n, shm_hit, shm_lock);
                flag = 1;
            }
        }
    }

    // wait for children to exit
    for (int i = 0; i < 3; ++i) {
        wait(NULL);
    }

    printf("Number of hits %d\n", *shm_hit);
    float result = (float) *shm_hit / (float) NUM_TRIALS / 4.0;
    printf("The result is %.2f\n", result);

    sem_destroy(shm_lock);

    shm_unlink(NAME);
    shm_unlink(LNAME);

    return 0;
}

void runner(int n, int* shm_hit, sem_t* shm_lock)
{
    printf("Process start simulation\n");
    srand(time(NULL));

    for (int i = 0; i < NUM_TRIALS; i++)
    {
        int arr[365] = {0}; // an array with 365 zeros
        int birthday;

        // for n students
        for (int j = 0; j < n; j++)
        {
            birthday = rand() % 364;

            // check if there is student with the same birthday
            if (arr[birthday] == 1)
            {
                sem_wait(shm_lock);
                *shm_hit += 1; // critical operation here
                sem_post(shm_lock);
                break;
            }
            else
            {
                arr[birthday] = 1;
            }
        }
    }
}