#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <netdb.h>
#include <ctype.h>
#include <stdlib.h>
#include "util.h"
#define MAX_LEN 255

int main(int argc, char *argv[]){
    if (argc != 2) printf("Need 2 arguments: ./server <port_number>!\n");
    else{
        int sockfd, rcvBytes, sendBytes;
        socklen_t len;
        char buff[MAX_LEN+1], message[MAX_LEN+1];
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
            len = sizeof(cliaddr);
            rcvBytes = recvfrom(sockfd, buff, MAX_LEN, 0, (struct sockaddr *) &cliaddr, &len);
            if(rcvBytes < 0){
                perror("1. Error: ");
                return 0;
            }

            buff[rcvBytes] = '\0';
            if (buff[strlen(buff)-1] == '\n') buff[strlen(buff) -1] = '\0';
            if (strcmp(buff,"") == 0 || strcmp(buff,"\n") == 0) {
                printf("\nStopped processing.\n");
                break;
            }
            
            int i=0,flag=0;
            struct hostent *hostInfo;
            struct in_addr **addr_IP_list, **addr_name_list, addr;
            if(validateIP(buff) == TRUE){
                inet_aton(buff, &addr);
                hostInfo = gethostbyaddr(&addr, sizeof(addr), AF_INET);
                if (hostInfo != NULL){
                    flag = 1;
                    strcpy(message,"Official name: ");
                    strcat(message,hostInfo->h_name);
                    strcat(message,"\nAlias name: \n");
                    
                    sendBytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &cliaddr, len);
                    if(sendBytes < 0){
                        perror("2. Error: ");
                        return 0;
                    }
                    addr_name_list = (struct in_addr **)hostInfo->h_aliases;
                    for(i = 0; addr_name_list[i] != NULL; i++) {
                        if(i != 0) {
                            strcpy(message,inet_ntoa(*addr_name_list[i]));
                            strcat(message," \n");
                            sendBytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &cliaddr, len);
                            if(sendBytes < 0){
                                perror("3. Error: "); return 0;
                            }
                        }
                    }
                } else {
                    strcpy(message,"Not found information\n");
                    sendBytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &cliaddr, len);
                    if(sendBytes < 0){
                        perror("4. Error: "); return 0;
                    }   
                }
            }
        
            if (flag == 0){
                long bufflen = strlen(buff);
                if(isdigit(buff[bufflen-1])) {
                    strcpy(message,"IP Address is invalid\n");
                    sendBytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &cliaddr, len);
                    if(sendBytes < 0){
                        perror("4. Error: "); return 0;
                    }   
                } else{
                    hostInfo = gethostbyname(buff);
                    if (hostInfo != NULL){
                        flag = 1;
                        strcpy(message,"Official IP: ");
                        strcat(message,inet_ntoa(*(struct in_addr*)hostInfo->h_addr));
                        strcat(message,"\nAlias IP: \n");  
                       
                        addr_IP_list = (struct in_addr **)hostInfo->h_addr_list;
                        for(i = 0; addr_IP_list[i] != NULL; i++) {
                            if(i != 0) {
                                strcat(message,inet_ntoa(*addr_IP_list[i]));
                                strcat(message," \n");
                            }
                        }
                        // printf("%s\n",message);
                        // printf("len = %ld\n",strlen(message));
                        sendBytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &cliaddr, len);
                        if(sendBytes < 0){
                            perror("5. Error: ");
                            return 0;
                        }
                    }
                }
            }

            if (flag == 0) {
                strcpy(message,"Not found information\n");
                sendBytes = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *) &cliaddr, len);
                if(sendBytes < 0){
                    perror("7. Error: "); return 0;
                }
            }

        }
    }

}