#include<stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    printf("Hello world! This is CS6233 Spring 2022\n");

    srand(time(0));

    int lower = 0, upper = 199;
    int num = (rand() % (upper - lower + 1)) + lower;

    printf("Random number: %d\n\n", num);

    return 0;
}
