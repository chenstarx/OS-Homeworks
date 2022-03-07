#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    pid_t ppid;

    ppid = getpid();
    pid = fork();

    if (pid < 0) 
    {
        printf("Error happend\n");
        return 1;
    }
    else if (pid == 0)
    {
        printf("Child: parent pid is %d\n", ppid);
    }
    else
    {
        wait(NULL);
        printf("Parent and child finished\n");
    }

    return 0;
}
