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
    if (argc != 3) printf("Need 3 arguments: ./client <server_address> <port_number>!\n");
    else{
         int sockfd, rcvBytes, sendBytes;
        socklen_t len;
        char buff[MAX_LEN+1], rcvMesg[MAX_LEN+1];
        struct sockaddr_in servaddr;

        //Step 1: Construct socket
        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
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

        //Step 3: Communicate with server
        while(1){
            strcpy(buff,""); strcpy(rcvMesg,"");
            printf("\n\nEnter domain name or IP address: ");
            fgets(buff,MAX_LEN,stdin);
            if(strlen(buff) == 0) break;
            len = sizeof(servaddr);
            sendBytes = sendto(sockfd, buff, strlen(buff), 0,(struct sockaddr *) &servaddr, len);
            if(sendBytes < 0){
                perror("1. Error: ");
                return 0;
            }
            if(strcmp(buff,"\n") == 0) {
                printf("No message to server. Stopped\n");
                break;
            }
            rcvBytes = recvfrom(sockfd, rcvMesg, MAX_LEN, 0, (struct sockaddr *) &servaddr, &len);
            if(rcvBytes < 0){
                perror("2. Error: ");
                return 0;
            }
         
            rcvMesg[rcvBytes] = '\0';
            //printf("rcvBytes = %d\n",rcvBytes);
            printf("Reply from server: \n%s", rcvMesg);
            //printf("Received all\n");
        }
    }
}