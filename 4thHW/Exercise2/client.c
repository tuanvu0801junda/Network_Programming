#include <stdio.h>
#include <strings.h>    //bzero()
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h> //socklen_t 
#include <netinet/in.h> //struct sockaddr, htonl
#include <arpa/inet.h>  //inet_ntoa, inet_aton
#include <sys/stat.h>  //stat (file exist)
#include <unistd.h>
#define MAX_LEN 1024

int checkFileExist (char *filePath){
  struct stat buffer;   
  return (stat(filePath, &buffer) == 0);
}

int main(int argc, char *argv[]){
    if (argc != 3) printf("Need 3 arguments: ./client <server_address> <port_number>!\n");
    else{
        int sockfd, rcvBytes, sendBytes, buffLen;
        char buff[MAX_LEN+1], rcvMesg[MAX_LEN+1];
        struct sockaddr_in servaddr;
        struct stat fileStatus;
        FILE *f;
        char fileContent[MAX_LEN*2];

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
            strcpy(fileContent,"");
            printf("\n\nClient's folder: \"test\" --> Valid file path example: \"test/abc.txt\"\n");
            printf("**************************************\n");
            printf("You can also try with 2 files in \"test\" folder.\n");
            printf("Those are \"test/If.mp3\" and \"test/ship.jpg\"\n");
            printf("**************************************\n");
            printf("Enter file path to server: ");
            fgets(buff,MAX_LEN,stdin);
            if(strlen(buff) == 0) break;
            
            if(strcmp(buff,"\n") != 0){
                buffLen = strlen(buff);
                if(buff[buffLen-1] == '\n') buff[buffLen-1] = '\0';
                if (stat(buff,&fileStatus) != 0){
                    printf("Error: File not found\n");
                    continue;
                } else{
                    //if(buff[buffLen-1] == '\n') buff[buffLen-1] = '/';
                    strcat(buff,"/");
                    sendBytes = send(sockfd,buff,strlen(buff),0);
                    if(sendBytes < 0){
                        perror("Error: ");
                        return 0;
                    }

                    if(buff[buffLen - 1] == '/') buff[buffLen-1] = '\0';
                    //printf("Filepath (buff) = %s\n",buff);
                    f = fopen(buff,"rb");
                    if (f == NULL) {
                        printf("File not found!\n");
                        continue;
                    } else{
                        fread(fileContent,MAX_LEN,1,f);
                        //printf("fileContent = %s\n",fileContent);
                        sendBytes = send(sockfd,fileContent,strlen(fileContent),0);
                    }
                    fclose(f);
                }
            } else{
                sendBytes = send(sockfd, "\n", strlen("\n") , 0);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
                printf("\nNo message to server. Stopped\n");
                break;
            }
            
            rcvBytes = recv(sockfd, rcvMesg, MAX_LEN, 0);
            if(rcvBytes < 0){
                perror("Error: ");
                return 0;
            }
            rcvMesg[rcvBytes] = '\0';
            printf("Reply from server: \n%s",rcvMesg);
        }
        close(sockfd);
    }
}