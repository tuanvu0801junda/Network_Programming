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
#include <poll.h>
#include "BST.h"

#define LENGTH 256
#define BUFF_SIZE 512

void replyClient(int sock, int *ctrlSignal){
    char replyMsg[LENGTH], *tokenUserId, *tokenPw;
    int recvByte = 1, sendByte = 0;

    int socketId = sock;
    (*ctrlSignal) = 0;

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
    (*ctrlSignal) = 1; //recvByte == 0 | < 0 --> escaped
    close(socketId);
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Server-Input Format Error!!\n");
        printf("Re-input with format: ./server <Port_Number>\n");
    } else{
        int nready, i, maxi, listenfd, connfd, ret, sockfd;
        struct sockaddr_in serverAddr, cliaddr;

        socklen_t clilen;
        const int OPEN_MAX = sysconf(_SC_OPEN_MAX);  // maximum number of opened files 
        struct pollfd clients[OPEN_MAX];
        int INFTIM = -1;
        int ctrlSignal = 0;

        if (atoi(argv[1]) <= 0 || atoi(argv[1]) > 65535) {
            perror("[x] Invalid port number!");
            return -1;
        }

        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd <=0 ){
            perror("[x] serverSocket failed\n");
            return -1;
        }
        //memset(&serverAddr, '\0', sizeof(serverAddr));

        bzero(&serverAddr,sizeof(serverAddr)); 
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(atoi(argv[1]));
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 

        ret = bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if(ret < 0){
            perror("[x] Server Binding failed\n");
            return -1;
        }

        if(listen(listenfd, 10) < 0){
            perror("[x] Server cannot listen\n");
            return -1;
        }

        clients[0].fd = listenfd;
        clients[0].events = POLLRDNORM;

        for (i = 1; i < OPEN_MAX; i++) {
                clients[i].fd = -1;     // -1 indicates available entry
        }
        maxi = 0;       // max index into clients[] array

        printf("Server started processing ... \n\n");

        while(1){
            nready = poll(clients, maxi + 1, INFTIM);
            if (nready <= 0) continue;
            
            // Check new connection
            if (clients[0].revents & POLLRDNORM) {
                clilen = sizeof(cliaddr);
                if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0) {
                    perror("[x] Accept failed!\n");
                    return -1;
                }

                printf("Accept socket %d (%s : %hu)\n", connfd, 
                        inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

                // Save client socket into clients array
                for (i = 0; i < OPEN_MAX; i++) {
                    if (clients[i].fd < 0) {
                        clients[i].fd = connfd;
                        break;
                    }
                }

                // No enough space in clients array
                if (i == OPEN_MAX) {
                    perror("[x] Too many client(s)!\n");
                    close(connfd);
                    return -1;
                }

                clients[i].events = POLLRDNORM;

                if (i > maxi) maxi = i;

                // No more readable file descriptors
                if (--nready <= 0) continue;
            } 

            // Check all clients to read data
            for (i = 1; i <= maxi; i++) {
                if ((sockfd = clients[i].fd) < 0) continue;

                // If the client is readable or errors occur
                if (clients[i].revents & (POLLRDNORM | POLLERR)) {
                    replyClient(sockfd,&ctrlSignal);
                    if(ctrlSignal == 1){
                        // NO MORE CONVERSATION
                        printf("[o] Close socket %d\n", sockfd);
                        close(sockfd);
                        clients[i].fd = -1; 
                    }                        
                    if (--nready <= 0) break;
                }
            }
        }
        close(listenfd);
        printf("\n[o] Server stopped processing safely!\n");
    }
}
