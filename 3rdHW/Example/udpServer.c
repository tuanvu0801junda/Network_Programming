#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#define BUFF_SIZE 255
#define SERV_PORT 1225

int main(){
    int sockfd, rcvBytes, sendBytes;
    socklen_t len;
    char buff[BUFF_SIZE+1];
    struct sockaddr_in servaddr, cliaddr;

    //Step 1: Construct socket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Error: ");
        return 0;
    }
    
    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) <0 ){
        perror("Error: ");
        return 0;
    }
    printf("Server started.\n\n");

    //Step 3: Communicate with client
    for ( ; ; ) {
        len = sizeof(cliaddr);
        rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
        if(rcvBytes < 0){
            perror("Error: ");
            return 0;
        }

        buff[rcvBytes] = '\0';
        if (strcmp(buff,"\n") == 0) break;
        printf("\nClient Addr: %s| Port: %d\n",inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        printf("Message from client: %s",buff); //buff contain '\n'

        while(1){
            printf("\n\tEnter reply: ");
            fgets(buff, BUFF_SIZE, stdin);
            sendBytes = sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *) &cliaddr, len);
            if(strcmp(buff,"\n") == 0 || sendBytes == 0) break;
            if(sendBytes < 0){
                perror("Error: ");
                return 0;
            }
        }

        printf("Escaped from while(1)\n");
    }
}