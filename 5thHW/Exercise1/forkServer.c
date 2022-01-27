#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>

#define PORT 5500
#define CONNECT_ADDR "127.0.0.1"
#define LENGTH 1024

void sig_chld(int signo){
    pid_t pid;
    int stat;
    pid = waitpid(-1, &stat, WNOHANG );
    printf("[o] Child %d terminated\n\n", pid);
}

void replyClient(int socketId){
    char replyMsg[LENGTH];
    int recvByte = 0, sendByte = 0;

    recvByte = recv(socketId,replyMsg,LENGTH,0);
    while(1){
        if(strcmp(replyMsg,"q\n") == 0 || strcmp(replyMsg,"Q\n") == 0){
            printf("[o] Close connection safely!\n");
            close(socketId);
            return;
        }
        replyMsg[recvByte] = '\0';
        for(int i=0;i<recvByte;i++) replyMsg[i] = toupper(replyMsg[i]);

        sendByte = send(socketId, replyMsg, recvByte, 0);
        if(sendByte < 0){
            perror("[x] Can't send message\n");
            return;
        }
        recvByte = recv(socketId,replyMsg,LENGTH,0);
    }
    if(recvByte < 0){
        perror("[x] Received nothing\n");
        return;
    }
}

int main(){
    int sockfd, ret;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;
    pid_t childpid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd <=0 ){
        perror("[x] serverSocket failed\n");
        return -1;
    }
    //memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero(&serverAddr,sizeof(serverAddr)); 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 

    ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0){
        perror("[x] Server Binding failed\n");
        return -1;
    }

    if(listen(sockfd, 10) < 0){
        perror("[x] Server cannot listen\n");
        return -1;
    }
    printf("Server started processing ... \n\n");
    signal(SIGCHLD,sig_chld);
    
    while(1){
        newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
        if (newSocket < 0){
            if (errno == EINTR) continue;
            else{
                perror("[x] newSocket failed!\n");
                return -1;
            }
        }
    
        childpid = fork();
        if(childpid == 0){
            close(sockfd); //close parents socket
            printf("Connection accepted from %s:(port %d)\n\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
            //currentClient += 1;
            //printf("Number of client(s) now: %d\n",currentClient);
            replyClient(newSocket);
            return 1;
        }
        close(newSocket);
    }
    //close(sockfd);
    printf("\n[o] Server stopped processing safely!\n");
}
