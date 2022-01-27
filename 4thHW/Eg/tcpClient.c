#include <stdio.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#define PORT 8080 
#define BUFF_SIZE 255
#define LOCAL_HOST "127.0.0.1"
   
int main(int argc, char const *argv[]) { 
    int clientfd;
    char buff[BUFF_SIZE+1];
    struct sockaddr_in servAddr;

    // Create socket
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("Socket creation error \n"); 
        return -1; 
    } 
    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = inet_aton(SERV_ADDR, &servaddr.sin_addr);
    // This 27th make the whole doesn't work!!!!
    servAddr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    servAddr.sin_port = htons(PORT);
    
    // Connect
    if (connect(clientfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    int rcvBytes, sendBytes;
    while(1){
        printf("Send to server: ");
        fgets(buff,BUFF_SIZE+1,stdin);

        sendBytes = send(clientfd,buff,strlen(buff),0);
        if(sendBytes < 0){
            perror("Error: sendBytes < 0");
            return -1;
        }

        // check buff if buff is "\n" <stop communicating>
        if(strcmp(buff,"\n")==0) {
            printf("\nNo message to server! Stop here\n");
            break;
        }

        rcvBytes = recv(clientfd,buff,BUFF_SIZE,0);
        if(rcvBytes < 0){
            perror("Error: rcvBytes < 0");
            return -1;
        }
        buff[rcvBytes] = '\0';
        printf("Message from server: %s\n",buff);
    }
    close(clientfd);
    return 0; 
} 