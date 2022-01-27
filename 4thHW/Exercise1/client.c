#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#define MAX_LEN 255

int main(int argc, char *argv[]){
    if (argc != 3) printf("Need 3 arguments: ./client <server_address> <port_number>!\n");
    else{
        int sockfd, rcvBytes, sendBytes;
        char buff[MAX_LEN+1], rcvMesg[MAX_LEN+1], completeBuff[MAX_LEN*2];
        char *token; int tokenLenCheck=0;
        struct sockaddr_in servaddr;

        //Step 1: Construct socket
        if((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0){
            perror("Error: ");
            return 0;
        }   

        //Step 2: Define the address of the server
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        //servaddr.sin_addr.s_addr = inet_aton(SERV_ADDR, &servaddr.sin_addr);
        // This 27th make the whole doesn't work!!!!
        servaddr.sin_addr.s_addr = inet_addr(argv[1]);
        servaddr.sin_port = htons(atoi(argv[2]));

        // Connect
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { 
            printf("\nConnection Failed \n"); 
            return -1; 
        } 
        //Step 3: Communicate with server
        while(1){
            strcpy(buff,""); strcpy(rcvMesg,"");
            printf("\n\nSend to server: ");
            fgets(buff,MAX_LEN,stdin);
            if(strlen(buff) == 0) break;
            
            if(strcmp(buff,"\n") != 0){
                // put length of string before string (VD: "5-hello") with sprintf
                sprintf(completeBuff,"%ld",strlen(buff));
                strcat(completeBuff,"-");
                strcat(completeBuff,buff);
                strcat(completeBuff,"-");
                sendBytes = send(sockfd, completeBuff, strlen(completeBuff), 0);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
            } else{
                sendBytes = send(sockfd, "\n", strlen("\n") , 0);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
                printf("No message to server. Stopped\n");
                break;
            }
            
            rcvBytes = recv(sockfd, rcvMesg, MAX_LEN, 0);
            if(rcvBytes < 0){
                perror("Error: ");
                return 0;
            }
            rcvMesg[rcvBytes] = '\0';
            token = strtok(rcvMesg,"-");
            tokenLenCheck = atoi(token);

            token = strtok(NULL,"-");
            if (tokenLenCheck != strlen(token)) {
                printf("Received string is different from the origin string\n");
                printf("Checked with prefix-length\n");
            } else printf("Reply from server: \n%s", token);
        }
        close(sockfd);
    }
}