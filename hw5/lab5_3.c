#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define READ_END 0
#define WRITE_END 1

#define NUM_THREAD 4

#define NUM_TRIALS 100000

long nhits = 0;

void *runner(void *n);

int main(int argc, char *argv[])
{
    int n;

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

    pthread_t workers[NUM_THREAD];
    pthread_attr_t attr;

    printf("Simulation start with n = %d\n", n);

    pthread_attr_init(&attr);

    for (int i = 0; i < NUM_THREAD; i++)
    {
        pthread_create(&workers[i], &attr, runner, &n);
        pthread_join(workers[i], NULL);
    }

    printf("Number of hits %ld\n", nhits);
    float result = (float) nhits / (float) NUM_TRIALS / 4.0;
    printf("The result is %.2f\n", result);

    return 0;
}

void *runner(void *n)
{
    printf("Thread start\n");

    int *students = (int *) n;
    unsigned int rand_state = (unsigned int) time(NULL) + pthread_self();

    for (int i = 0; i < NUM_TRIALS; i++)
    {
        int arr[365] = {0}; // an array with 365 zeros
        int birthday;

        // for n students
        for (int j = 0; j < *students; j++)
        {
            birthday = rand_r(&rand_state) % 364;

            // check if there is student with the same birthday
            if (arr[birthday] == 1)
            {
                nhits += 1;
                break;
            }
            else
            {
                arr[birthday] = 1;
            }
        }
    }
}