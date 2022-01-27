#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 5500
#define CONNECT_ADDR "127.0.0.1"
#define LENGTH 1024

int main(){
    int clientSocket, ret, countByte=0, sendByte=0, recvByte=0;
    struct sockaddr_in serverAddr;
    char msg[LENGTH];

    clientSocket = socket(AF_INET,SOCK_STREAM,0);
    if(clientSocket < 0){
        perror("[x] clientSocket failed!\n");
        return -1;
    }

    //memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero(&serverAddr,sizeof(serverAddr)); 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(CONNECT_ADDR);

    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0){
        perror("[x] connect Failed!\n");
        return -1;
    }

    while(1){
        printf("Client's message: ");
        fgets(msg, LENGTH, stdin);

        sendByte = send(clientSocket,msg,strlen(msg),0);
        if(sendByte <= 0){
            perror("[x] Cannot send message to server\n");
            return -1;
        }
        countByte += sendByte;
        if(strcmp(msg,"q\n") == 0 || strcmp(msg,"Q\n") == 0){
            printf("[o] Connection stop safely. \n");
            printf("[o] Total transferred byte(s): %d\n",countByte);
            break;
        } else{
            recvByte = recv(clientSocket,msg,LENGTH,0);
            if (recvByte <= 0){
                perror("[x] Cannot receive message from server\n");
                return -1;
            }
            msg[recvByte] = '\0';
            printf("Reply from server: %s\n",msg);
        }
    }
    close(clientSocket);
}