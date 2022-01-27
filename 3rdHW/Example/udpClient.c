#include <stdio.h>
#include <string.h>
#include <strings.h>    //bzero()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

#define BUFF_SIZE 255
#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 1225

int main(){
    int sockfd, rcvBytes, sendBytes;
    socklen_t len;
    char buff[BUFF_SIZE+1], rcvMesg[BUFF_SIZE];
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
    servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
    servaddr.sin_port = htons(SERV_PORT);

    //Step 3: Communicate with server
    while(1){
        printf("\n\nSend to server: ");
        fgets(buff,BUFF_SIZE,stdin);
        if(strcmp(buff,"\n") == 0) break;
        len = sizeof(servaddr);
        sendBytes = sendto(sockfd, buff, strlen(buff), 0,(struct sockaddr *) &servaddr, len);
        if(sendBytes < 0){
            perror("Error: ");
            return 0;
        }

        while(rcvBytes != 0){
            rcvBytes = recvfrom(sockfd, rcvMesg, BUFF_SIZE, 0, (struct sockaddr *) &servaddr, &len);
            if(rcvBytes < 0){
                perror("Error: ");
                return 0;
            }        
            rcvMesg[rcvBytes] = '\0';
            if(strcmp(rcvMesg,"\n") == 0 || rcvBytes == 0) {
                printf("\n\nSend to server: ");
                fgets(buff,BUFF_SIZE,stdin);
                sendBytes = sendto(sockfd, buff, strlen(buff), 0,(struct sockaddr *) &servaddr, len);
                if(sendBytes < 0){
                    perror("Error: ");
                    return 0;
                }
                break;
            }
            printf("Received Bytes = %d\n",rcvBytes);
            printf("Reply from server: %s\n", rcvMesg);
        }
    }
}
