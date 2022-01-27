#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include "BST.h"

#define LENGTH 256

void *replyClient(void *arg){
    char replyMsg[LENGTH], *tokenUserId, *tokenPw;
    int recvByte = 1, sendByte = 0;

    //pthread_detach(pthread_self());
    int socketId = *(int*) arg;

    node root = NULL, searchResult = NULL;
    FILE* file = fopen("account.txt","r");
    if(file == NULL){
        perror("[x] Error: Can't open \"account.txt\" file to read\n");
    } else {
        char reader[LENGTH];
        char userId[LEN], pw[LEN];
        int status;
        fscanf(file, "%[^\n]%*c", reader); //ignore header
        while(1){
            fscanf(file,"%s\t%s\t%d\n",userId,pw,&status);
            root = insert_node(root,userId,pw,status);
            if(feof(file)) break;
        }
    }
    fclose(file);

    while(recvByte > 0){
        recvByte = recv(socketId,replyMsg,LENGTH,0);
        if(recvByte <= 0){
            perror("[x] Received nothing\n");
            break;
        }
        replyMsg[recvByte] = '\0';

        if(strcmp(replyMsg,"q\n") == 0 || strcmp(replyMsg,"Q\n") == 0){
            printf("[o] Close connection safely!\n");
            break;
        }

        tokenUserId = strtok(replyMsg,"-"); //userId
        tokenPw = strtok(NULL,"-"); //password

        if(strcmp(tokenPw,"LOCK") == 0){
            searchResult = look_for_node(root,tokenUserId);
            searchResult->status = 0;
            FILE *f = fopen("account.txt","w");
            if(f == NULL){
                perror("Cannot open \"account.txt\" to write\n");
                break;
            }
            fprintf(f,"%s\t%s\t%s\n","UserId","Password","Status");
            print_file(root,f);
            fclose(f);
        } else{
            searchResult = look_for_node(root,tokenUserId);
            if(searchResult != NULL && strcmp(searchResult->password,tokenPw) == 0){
                sendByte = send(socketId, "1-Login successfully!\n-", 25, 0);
                searchResult->status = 1;
                FILE *f = fopen("account.txt","w");
                if(f == NULL){
                    perror("Cannot open \"account.txt\" to write\n");
                    break;
                }
                fprintf(f,"%s\t%s\t%s\n","UserId","Password","Status");
                print_file(root,f);
                fclose(f);
            } else if (searchResult == NULL){
                sendByte = send(socketId, "0-UserId not existed!\n-", 23, 0);
            } else sendByte = send(socketId, "0-Password incorrect!\n-", 25, 0);        
            if(sendByte < 0){
                perror("[x] Can't send message\n");
                break;
            }
        }
    }
    free_tree(root);
    close(socketId);
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Server-Input Format Error!!\n");
        printf("Re-input with format: ./server <Port_Number>\n");
    } else{
        int sockfd, ret;
        struct sockaddr_in serverAddr;

        int newSocket;
        struct sockaddr_in newAddr;

        socklen_t addr_size;
        pthread_t tid;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd <=0 ){
            perror("[x] serverSocket failed\n");
            return -1;
        }
        //memset(&serverAddr, '\0', sizeof(serverAddr));
        bzero(&serverAddr,sizeof(serverAddr)); 
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(atoi(argv[1]));
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

        while(1){
            //newSocker = connfd; sockfd = listenfd
            newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
            if (newSocket < 0){
                if (errno == EINTR) continue;
                else{
                    perror("[x] newSocket failed!\n");
                    return -1;
                }
            } else{
                printf("New connection from: %s (port: %d)\n\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
            }
            pthread_create(&tid,NULL,&replyClient,(void*)&newSocket);
            
            //close(newSocket);
        }
        close(sockfd);
        printf("\n[o] Server stopped processing safely!\n");
    }
}