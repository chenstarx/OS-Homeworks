#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>

#define PORT 5000

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

    struct sockaddr_in serv_addr;
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // creat child process
    pid = fork();

    if (pid < 0)
    {

        printf("Error: fork() failed\n");
        return 1;

    } else if (pid == 0) {
    
        printf("Child start\n");
        
        // create socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);

        usleep(1000000);

        if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("Error: Connect failed \n");
            return 1;
        }

        printf("Connect success \n");

        for (int i = 0; i < n; i++)
        {

            float sent = 0.25 * i * i;
            // send_buf[0] = sent;
            printf("\nChild sending %f\n", sent);

            write(sock, &sent, sizeof(float));

            int nanosec = rand() % 5000000;

            printf("Child sleeping %.3fs\n", (float) nanosec / 1000000.0);
            usleep(nanosec);

        }

        printf("Child finished\n");

    } else {

        // create socket
        int sock = socket(AF_INET, SOCK_STREAM, 0);

        bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

        listen(sock, 10);

        printf("Parent listening at 127.0.0.1:%d\n", PORT);

        int conn = accept(sock, (struct sockaddr*)NULL, NULL);

        while(1)
        {

            float recv_buf[1];
            int bytes = read(conn, recv_buf, sizeof(float));

            if (bytes < 0) {
                printf("Error: nothing read\n");
                exit(1);
            }

            if (bytes == 0) {
                break;
            }

            printf("Parent received: %f\n", recv_buf[0]);
        }

        close(conn);
        printf("Parent finished\n");
    }

    return 0;
}
