#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 4096 /*max text line length*/
//#define SERV_PORT 3000 /*port*/

void getFileName(char* filepath, char* filename){
    int i = strlen(filepath)-1;
    while(filepath[i] != '/') i--;
    int j = 0;
    i++;
    for(; i < strlen(filepath);)
        filename[j++] = filepath[i++];
    filename[j] = 0;
}

int main(int argc, char **argv) 
{
    int sockfd, n;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    //basic check of the arguments
    //additional checks can be inserted
    if (argc != 3) {
        perror("Usage: ./client <ServerIP> <Port>"); 
        exit(1);
    }

    int serv_port = atoi(argv[2]);
    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    servaddr.sin_port =  htons(serv_port); //convert to big-endian order

    //Connection of the client to the socket 
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
        perror("Problem in connecting to the server");
        exit(3);
    }

    char filepath[256];
    char filename[50];
    FILE* fp;
        
    // get file path from user
    printf("Enter filepath: ");
    fgets(filepath, 256, stdin);
    filepath[strlen(filepath)-1] = 0; // strip newline

    if(strlen(filepath) == 0){
        printf("Terminate signal detected. Terminate process.\n");
        send(sockfd, "0", 1, 0);
        exit(1);
    }

    fp = fopen(filepath, "rb");
    if(fp == NULL){
        printf("Error: <%s> no such file exist.\n", filepath);
        exit(1);
    }
    // resolve to get filename
    getFileName(filepath, filename);
    // send filename to server
    send(sockfd, filename, strlen(filename), 0);
    // wait for reply
    if((n = recv(sockfd, recvline, MAXLINE, 0)) == 0){
        printf("Server refuse connection.\n");
        exit(4);
    }
    if(recvline[0] == '0'){
        printf("Error: File already exist on server.\n");
        fclose(fp);
        exit(1);
    }
    // get file length
    fseek(fp, 0, SEEK_END);
    long filelen = ftell(fp);
    rewind(fp);
    // transfer file in 4096bytes-long blocks.
    char buffer[MAXLINE];
    while(filelen > 0){
        fread(buffer, MAXLINE, 1, fp);
        send(sockfd, buffer, MAXLINE, 0);
        filelen -= MAXLINE;
    }
    // send done transfering signal
    send(sockfd, "1", 1, 0);
    printf("Done transfering\n");
    fclose(fp);

    exit(0);
}
