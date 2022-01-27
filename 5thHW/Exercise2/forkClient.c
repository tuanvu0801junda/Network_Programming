#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <ctype.h>
#include <sys/stat.h>  //stat (file exist)
#include <stdlib.h>
#include <unistd.h>

#define PORT 5500
#define CONNECT_ADDR "127.0.0.1"
#define LENGTH 1024
#define PATH_LEN 512

int main(){
    int clientSocket, ret, countByte=0, sendByte=0, recvByte=0;
    struct sockaddr_in serverAddr;
    char fileContent[LENGTH], path[PATH_LEN], *fileName, newPath[PATH_LEN];

    struct stat fileStatus;
    FILE *f;

    clientSocket = socket(AF_INET,SOCK_STREAM,0);
    if(clientSocket < 0){
        perror("[x] clientSocket failed!\n");
        return -1;
    }

    //memset(&serverAddr, '\0', sizeof(serverAddr));
    bzero(&serverAddr,sizeof(serverAddr)); 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(CONNECT_ADDR);

    ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if(ret < 0){
        perror("[x] connect Failed!\n");
        return -1;
    }

    while(1){
        printf("\nPlease select a file in folder \"clientStorage\"\n");
        printf("Entered file path must be like \"clientStorage/Laravel.txt\"\n");
        printf("Enter file path (quit using \"q\" and \"Q\"): ");
        fgets(path, LENGTH, stdin);

        if(strcmp(path,"q\n") != 0 && strcmp(path,"Q\n") != 0){
            if(path[strlen(path)-1] == '\n') path[strlen(path)-1] = '\0';
            if(stat(path,&fileStatus)!=0){
                perror("[x] Error: File not found in \"clientStorage\"\n");
                continue;
            } else{
                strcat(path,"/");
                sendByte = send(clientSocket,path,strlen(path),0);
                if(sendByte <= 0){
                    perror("[x] Cannot send filepath\n");
                    return -1;
                }
                countByte += sendByte;
                if(path[strlen(path)-1] == '/') path[strlen(path)-1] = '\0';
                f = fopen(path,"rb");
                if (f == NULL) {
                    perror("[x] Cannot open file\n");
                    continue;
                } else{
                    fread(fileContent,LENGTH,1,f);
                    sendByte = send(clientSocket,fileContent,strlen(fileContent),0);
                    if(sendByte <= 0){
                        perror("[x] Cannot send file content to server\n");
                        return -1;
                    }
                    countByte += sendByte;
                }
                fclose(f);        
            }
        } else{
            sendByte = send(clientSocket,"q\n",strlen("q\n"),0);
            if(sendByte <= 0){
                perror("[x] Cannot send quit message to server\n");
                return -1;
            }
            countByte += sendByte;
            printf("[o] Connection stop safely. \n");
            printf("[o] Total transferred byte(s): %d\n",countByte);
            break;
        }
        
        recvByte = recv(clientSocket,fileContent,LENGTH,0);
        if (recvByte <= 0){
            perror("[x] Cannot receive file from server\n");
            return -1;
        }
        fileContent[recvByte] = '\0';
        fileName = strtok(path,"/");
        fileName = strtok(NULL,"/");
        fileName = strtok(fileName,".");
        strcat(fileName,"_upper.txt");
        strcpy(newPath,fileName);
        strcpy(path,"clientStorage/");
        strcat(path,newPath);

        f = fopen(path,"wb");
        if(f == NULL){
            perror("[x] Cannot open file received from server\n");
            return -1;
        }
        ret = fputs(fileContent,f);       
        if(ret != EOF){
            printf("[o] Received uppercase file successfully!\n");
            printf("[o] A new file with \"_upper.txt\" created in \"clientStorage\"\n");
        } else {
            printf("[x] Cannot receive uppercase file\n");
        }
        fclose(f);
    }
    close(clientSocket);
}