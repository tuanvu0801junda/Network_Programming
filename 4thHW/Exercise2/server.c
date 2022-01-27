#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <unistd.h>
#include <sys/stat.h>
#define BUFF_SIZE 1024

int main(int argc, char *argv[]){
    if (argc != 2) printf("Need 2 arguments: ./server <port_number>!\n");
    else{
        int listenfd, connfd, result;
        struct sockaddr_in servAddr,clientAddr; 
        char buff[BUFF_SIZE+1],sendMsg[BUFF_SIZE*2],path[BUFF_SIZE];
        int rcvBytes, sendBytes, clientAddrLen = sizeof(clientAddr);
        char *token, fileContent[BUFF_SIZE];
        //struct stat fileStatus;
        FILE* file;

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
        strcpy(path,"");
        printf("\nServer started processing ...\n\n");
        printf("**************************************\n");
        printf("File tranferred successfully will be stored at folder \"serverStorage\".\n");
        printf("Client: test/abc.txt --> Server: serverStorage/abc.txt.\n");
        printf("Please create a folder called \"serverStorage\" if you dont have any.\n");
        printf("Here in my work, i've already created one called \"serverStorage\".\n");
        printf("**************************************\n");
        printf("\nWaiting connection from client(s) ... \n");

        if ((connfd = accept(listenfd, (struct sockaddr *)&clientAddr,(socklen_t*)&clientAddrLen))<0) { 
            perror("Accept Exception"); 
            return 0; 
        } 
        while(1){  
            strcpy(fileContent,"");  
            //printf("fileContent = %s\n",fileContent);
            
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
                //printf("\nfilepath: %s\n",buff);
                token = strtok(buff,"/");
                token = strtok(NULL,"/"); //token = filename

                //printf("token = %s\n",token);
                strcpy(path,"serverStorage/");
                strcat(path,token); //buff = path to file
                //printf("path = %s\n",path);

                rcvBytes = recv(connfd,fileContent,BUFF_SIZE,0);
                if (rcvBytes <0){
                    perror("Error: received Bytes < 0");
                    break;
                }
                file = fopen(path,"rb");
                if(file != NULL) {
                    strcpy(sendMsg,"Error: File is existed on server\n");
                    sendBytes = send(connfd,sendMsg,strlen(sendMsg),0);
                    if (sendBytes < 0){
                        perror("Error: sendBytes < 0");
                        break;
                    }
                } else{
                    file = fopen(path,"wb");
                    result = fputs(fileContent,file);
                    if (result == EOF){
                        strcpy(sendMsg,"Error: File tranfering is interupted\n");
                        sendBytes = send(connfd,sendMsg,strlen(sendMsg),0);
                        if (sendBytes < 0){
                            perror("Error: sendBytes < 0");
                            break;
                        }
                    } else {
                        strcpy(sendMsg,"Successful transfering\n");
                        sendBytes = send(connfd,sendMsg,strlen(sendMsg),0);
                        if (sendBytes < 0){
                            perror("Error: sendBytes < 0");
                            break;
                        }
                    }
                }
                fclose(file); 
            }
        } //while (1)
        close(connfd); //close connect
    } //else 3 arguments

} //int main