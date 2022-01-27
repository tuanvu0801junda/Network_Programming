#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <ctype.h>
#include <stdlib.h>
#define MAX_LEN 255

int main(int argc, char *argv[]){
    if (argc != 2) printf("Need 2 arguments: ./server <port_number>!\n");
    else{
        int sockfd, rcvBytes, sendBytes, countLetter=0, countDigit=0;
        socklen_t len;
        char buff[MAX_LEN+1], letter[MAX_LEN+1], digit[MAX_LEN+1];
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
        servaddr.sin_port = htons(atoi(argv[1]));
        if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) <0 ){
            perror("Error: ");
            return 0;
        }

        printf("Server started processing ... \n");
        //Step 3: Communicate with client
        for ( ; ; ) {
            countDigit = 0; countLetter = 0;
            len = sizeof(cliaddr);
            rcvBytes = recvfrom(sockfd, buff, MAX_LEN, 0, (struct sockaddr *) &cliaddr, &len);
            if(rcvBytes < 0){
                perror("Error: ");
                return 0;
            }

            buff[rcvBytes] = '\0';
            if (strcmp(buff,"") == 0 || strcmp(buff,"\n") == 0) {
                printf("\nStopped processing.\n");
            
               break;
            }
            int buffLen = strlen(buff), err=0;
            if (buffLen == 0) break;
            if (buff[buffLen-1] == '\n') {buff[buffLen-1] = '\0';}
            for(int i=0;i<buffLen-1;i++){
                if(isdigit(buff[i])) digit[countDigit++] = buff[i];
                else if(isalpha(buff[i])) letter[countLetter++] = buff[i];
                else {
                    err = 1; break;
                }
            }
            digit[countDigit] = '\0';
            letter[countLetter] = '\0';

            if(err == 1){
                sendBytes = sendto(sockfd, "Error", strlen("Error") , 0, (struct sockaddr *) &cliaddr, len);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
            } else if (countDigit == 0){
                sendBytes = sendto(sockfd, letter, strlen(letter), 0, (struct sockaddr *) &cliaddr, len);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
            } else if (countLetter == 0){
                sendBytes = sendto(sockfd, digit, strlen(digit), 0, (struct sockaddr *) &cliaddr, len);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
            } else{
                strcat(digit, "\n");
                strcat(digit, letter);
                sendBytes = sendto(sockfd, digit, strlen(digit), 0, (struct sockaddr *) &cliaddr, len);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
            }
        }
    }

}