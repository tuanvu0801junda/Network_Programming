#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_SIZE 512

int main(int argc, char *argv[]){
    int sockfd, numbytes;
    struct sockaddr_in serverAddr; 
    unsigned int addr_len = sizeof(struct sockaddr_in);

    struct hostent *he; 
    char buff[BUFF_SIZE], reply[BUFF_SIZE];

    if (argc != 3){
        printf("[x] Need 3 arguments!\n");
        printf("[x] Retry with './client <server_address> <port_number>'\n");
        return -1;
    }

    if ((he = gethostbyname(argv[1])) == NULL){ 
        printf("[x] GetHostByName: FAILED!\n");
        return -1;
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("[x] Socket: FAILED!\n");
        return -1;
    }

    serverAddr.sin_family = AF_INET;            
    serverAddr.sin_port = htons(atoi(argv[2])); 
    serverAddr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    bzero(&(serverAddr.sin_zero), 8); 

    while (1){
        memset(reply, '\0', (strlen(reply) + 1));
        printf("[o] Press \"Enter\" to escape.\n");
        printf("[o] Insert ip or domain name: ");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        if (strcmp(buff, "\n") == 0) break;

        numbytes = sendto(sockfd, buff, strlen(buff), 0,
                               (struct sockaddr *)&serverAddr, addr_len);
        if (numbytes < 0){
            printf("[x] Cannot send to server");
            return -1;
        }

        numbytes = recvfrom(sockfd, reply, BUFF_SIZE, 0,
                                 (struct sockaddr *)&serverAddr, &addr_len);
        if (numbytes < 0){
            printf("[x] Cannot receive from server");
            return -1;
        }
        reply[numbytes] = '\0';
        printf("[o] Reply from server: \n%s\n",reply);
    }
    close(sockfd);
}
