#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "BST.h"

#define LENGTH 100

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Client-Input Format Error !!\n");
        printf("Re-input with: ./client <IP_Addr> <Port_Number>\n");
        return -1;
    }
    int clientSocket, ret, sendByte=0, recvByte=0, countWrong=0;
    struct sockaddr_in serverAddr;
    char msg[LENGTH*2], userId[LENGTH], password[LENGTH],*tokenNumber,*tokenMsg;
    node root = NULL;

    clientSocket = socket(AF_INET,SOCK_STREAM,0);
    if(clientSocket < 0){
        perror("[x] clientSocket failed!\n");
        return -1;
    }

    //memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero(&serverAddr,sizeof(serverAddr)); 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0){
        perror("[x] connect Failed!\n");
        return -1;
    }

    while(1){
        printf("Enter userId: ");
        fgets(userId,LENGTH,stdin);
        printf("Enter password: ");
        fgets(password,LENGTH,stdin);

        strcpy(msg,"");
        if(userId[strlen(userId)-1] == '\n') userId[strlen(userId)-1] = '\0';
        strcpy(msg,userId);
        strcat(msg,"-");
        if(password[strlen(password)-1] == '\n') password[strlen(password)-1] = '\0';
        strcat(msg,password);
        strcat(msg,"-");

        sendByte = send(clientSocket,msg,strlen(msg),0);
        if(sendByte <= 0){
            perror("[x] Cannot send message to server\n");
            return -1;
        }

        recvByte = recv(clientSocket,msg,LENGTH,0);
        if (recvByte <= 0){
            perror("[x] Cannot receive message from server\n");
            return -1;
        }
        msg[recvByte] = '\0';

        tokenNumber = strtok(msg,"-");
        tokenMsg = strtok(NULL,"-");
        
        if(atoi(tokenNumber) == 1) {
            //Log in successfully
            if(root == NULL ){
                root = create_node(userId,password,1);
            } else {
                printf("[x] Each client window CAN ONLY LOG IN ONCE with 1 account!");
                continue;
            }
            printf("[o] Annouce from server: %s\n",tokenMsg);
            printf("[o] Want to quit, input \"q\" or \"Q\" here: ");
            fgets(msg,LENGTH,stdin);

            if(strcmp(msg,"q\n") == 0 || strcmp(msg,"Q\n") == 0){
                printf("[o] Connection stop safely. \n");
                sendByte = send(clientSocket,msg,strlen(msg),0);
                if(sendByte <= 0){
                    perror("[x] Cannot send message to server\n");
                    return -1;
                }
                break;
            }
        } else {
            if(countWrong == 4){
                strcpy(msg,"");
                strcat(msg,userId);
                strcat(msg,"-");
                strcat(msg,"LOCK-");
                sendByte = send(clientSocket,msg,strlen(msg),0);
                if(sendByte <= 0){
                    perror("[x] Cannot send message to server\n");
                    return -1;
                }
                printf("\n[x] You've inputted password wrong 5 times\n");
                printf("[x] Your account will be LOCKED! \n");
                printf("[o] Want to quit, input \"q\" or \"Q\" here: ");
                fgets(msg,LEN,stdin);

                if(strcmp(msg,"q\n") == 0 || strcmp(msg,"Q\n") == 0){
                    printf("[o] Connection stop safely. \n");
                    sendByte = send(clientSocket,msg,strlen(msg),0);
                    if(sendByte <= 0){
                        perror("[x] Cannot send message to server\n");
                        return -1;
                    }
                    break;
                }
            }
            printf("\n[x] Password or userId incorrect!\n");
            countWrong += 1;
        }
    }
    close(clientSocket);
    free_tree(root);
}