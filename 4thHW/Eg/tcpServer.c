#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h>
#include <ctype.h>
#define BUFF_SIZE 255
#define PORT 8080 

int main(int argc, char const *argv[]) { 
    int listenfd, connfd;
    struct sockaddr_in servAddr,clientAddr; 
    char buff[BUFF_SIZE+1], MsgToClient[BUFF_SIZE+1];
    int rcvBytes, sendBytes, clientAddrLen = sizeof(clientAddr);

    // Step 1: Construct socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) { 
        perror("Socket failed"); 
        return 0; 
    }      

    // Step 2: Bind address to socket
    bzero(&servAddr,sizeof(servAddr)); 
    servAddr.sin_family = AF_INET; 
    servAddr.sin_addr.s_addr = INADDR_ANY; 
    servAddr.sin_port = htons(PORT);  //gan cong la 8080   

    // Bind 
    if (bind(listenfd, (struct sockaddr *)&servAddr, sizeof(servAddr))<0) { 
        perror("Binding process failed"); 
        return 0; 
    }

    // Listen request from client 
    if (listen(listenfd, 10) < 0) { 
        perror("Listening Exception"); 
        return 0; 
    } 

    printf("Server started processing ...\n");
    printf("Waiting connection from client(s) ... \n");

    while(1){    
        if ((connfd = accept(listenfd, (struct sockaddr *)&clientAddr,(socklen_t*)&clientAddrLen))<0) { 
            perror("Accept Exception"); 
            return 0; 
        } 
        rcvBytes = recv(connfd,buff,BUFF_SIZE,0);
        if (rcvBytes <0){
            perror("Error: rcvBytes < 0");
            break;
        } else{
            buff[rcvBytes] = '\0';
            if(strcmp(buff,"\n") == 0) {
                printf("\nServer stopped processing!\n");
                break;
            }
            printf("\nFrom Client: %s, port: %d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
            printf("Message from client: %s",buff);
            printf("Your reply: ");
            fgets(MsgToClient,BUFF_SIZE+1,stdin);

            sendBytes = send(connfd, MsgToClient, strlen(MsgToClient),0);
            if(sendBytes < 0){
                perror("Error: sendBytes < 0");
            }
        }
    }
    close(connfd);
    return 0; 
} 