#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAXLINE 4096 /*max text line length*/
//#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */
#define SAVE_FOLDER "output/"

int main (int argc, char **argv)
{
    if(argc != 2){
        printf("Usage: ./server <Port>\n");
        exit(1);
    }
    int serv_port = atoi(argv[1]);
    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;

    //creation of the socket
    listenfd = socket (AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(serv_port);

    bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen (listenfd, LISTENQ);

    printf("%s\n","Server running...waiting for connections.");
    FILE* fp = NULL;
    char filepath[256];
    for ( ; ; ) {
        // wait for request
        clilen = sizeof(cliaddr);
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

        printf("%s\n","Received request...");

        n = recv(connfd, buf, MAXLINE,0);
        if(buf[0] == '0'){
            printf("Service terminate signal received. Terminate process.\n");
            break;
        }
        buf[n] = 0;
        strcpy(filepath, SAVE_FOLDER);
        strcat(filepath, buf);
        printf("Push file: %s", filepath);
        if((fp = fopen(filepath, "r")) != NULL){
            printf(" -> Refuse\n");
            send(connfd, "0", 1, 0);
            fclose(fp);
        }
        else {
            printf(" -> Accept\n");
            send(connfd, "1", 1, 0);
            fp = fopen(filepath, "w");
            while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
                if(buf[0] == '1') // done transfering signal received
                    break;
                else fwrite(buf, n, 1, fp);
            }
            if (n < 0) {
                perror("Read error"); 
                exit(1);
            }
            fclose(fp);
            printf("Received from client: %s\n", filepath);
        }
        close(connfd);
    }
    //close listening socket
    close (listenfd); 
}
