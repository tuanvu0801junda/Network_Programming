#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "server.h"
#define BUFF_SIZE 255

void sendWithPrefixLengthCheck(int connfd, char *origin, char *final){
    char containLen[10];
    sprintf(containLen,"%ld",strlen(origin));
    strcpy(final,containLen);
    strcat(final,"-");
    strcat(final,origin);
    strcat(final,"-");
    int sendBytes = send(connfd, final, strlen(final) , 0);
    if(sendBytes < 0){
        perror("Error: ");
        return;
    }
}

int main(int argc, char *argv[]){
    if (argc != 2) printf("Need 2 arguments: ./server <port_number>!\n");
    else{
        int listenfd, connfd, countDigit=0, countLetter=0;
        struct sockaddr_in servAddr,clientAddr; 
        char buff[BUFF_SIZE+1], digit[BUFF_SIZE+1], letter[BUFF_SIZE+1],sendMsg[BUFF_SIZE*2];
        int rcvBytes, sendBytes, clientAddrLen = sizeof(clientAddr);
        char *token; int tokenLenCheck=0;

        // Step 1: Construct socket
        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) { 
            perror("Socket failed"); 
            return 0; 
        }      

        // Step 2: Bind address to socket
        bzero(&servAddr,sizeof(servAddr)); 
        servAddr.sin_family = AF_INET; 
        servAddr.sin_addr.s_addr = INADDR_ANY; 
        servAddr.sin_port = htons(atoi(argv[1]));  //gan cong la 8080   

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

        if ((connfd = accept(listenfd, (struct sockaddr *)&clientAddr,(socklen_t*)&clientAddrLen))<0) { 
            perror("Accept Exception"); 
            return 0; 
        } 
        while(1){    
            countDigit=0; countLetter=0;
            // if ((connfd = accept(listenfd, (struct sockaddr *)&clientAddr,(socklen_t*)&clientAddrLen))<0) { 
            //     perror("Accept Exception"); 
            //     return 0; 
            // } 

            //accept in loop will cause error! (it wait until A TOTAL NEW CLIENT send message)
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

                token = strtok(buff,"-");
                tokenLenCheck = atoi(token);
                token = strtok(NULL,"-");
                if (tokenLenCheck != strlen(token)) {
                    printf("Received string is different from the origin string\n");
                    printf("Checked with prefix-length\n");
                } else strcpy(buff,token);
                //printf("\nFrom Client: %s, port: %d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
                //printf("Message from client: %s",buff);
                //printf("Your reply: ");
                //fgets(MsgToClient,BUFF_SIZE+1,stdin);
                //buff[rcvBytes] = '\0';
                int buffLen = strlen(buff), err = 0, i=0;
                if(buff[buffLen-1] == '\n') buff[buffLen-1] = '\0';
                for(i=0;i<buffLen-1;i++){
                    if(isdigit(buff[i])) digit[countDigit++] = buff[i];
                    else if(isalpha(buff[i])) letter[countLetter++] = buff[i];
                    else {
                        err = 1; break;
                    }
                }
                digit[countDigit] = '\0';
                letter[countLetter] = '\0';

                if(err == 1){
                    sendWithPrefixLengthCheck(connfd,"Error",sendMsg);
                    // sendBytes = send(connfd, "Error", strlen("Error") , 0);
                    // if(sendBytes < 0){
                    //     perror("Error: ");
                    //     return 0;
                    // }
                } else if (countDigit == 0){
                    sendWithPrefixLengthCheck(connfd,letter,sendMsg);
                    // sendBytes = send(connfd, letter, strlen(letter), 0);
                    // if(sendBytes < 0){
                    //     perror("Error: ");
                    //     return 0;
                    // }
                } else if (countLetter == 0){
                    sendWithPrefixLengthCheck(connfd,digit,sendMsg);
                    // sendBytes = send(connfd, digit, strlen(digit), 0);
                    // if(sendBytes < 0){
                    //     perror("Error: ");
                    //     return 0;
                    // }
                } else{
                    strcat(digit, "\n");
                    strcat(digit, letter);
                    sendWithPrefixLengthCheck(connfd,digit,sendMsg);
                    // sendBytes = send(connfd, digit, strlen(digit), 0);
                    // if(sendBytes < 0){
                    //     perror("Error: ");
                    //     return 0;
                    // }
                }
            }
        }
        close(connfd); //close connect
    }

}