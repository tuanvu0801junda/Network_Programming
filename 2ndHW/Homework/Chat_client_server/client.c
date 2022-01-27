#include <stdio.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#define PORT 8080 
   
int main(int argc, char const *argv[]) { 
    struct sockaddr_in address, serv_addr; 
    int sock = 0, valread; 
    char buffer[1024], add[255], hello[512]; 
    int continu = 1;
    printf("Input server address: ");
    gets(add);

    //tao socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 

    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, add, &serv_addr.sin_addr) <= 0)  { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 

    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    while(1){
        char mess_from_client[225];
        printf("\nMessage to server: ");
        gets(mess_from_client);
        fflush(stdin);
        strcpy(hello,mess_from_client);

        printf("Message from server: ");
        send(sock , hello , strlen(hello) , 0); 
        valread = read( sock , buffer, 1024); 
        printf("%s\n",buffer );
        
        if (strcmp(mess_from_client, "bye") == 0) break;
        fflush(stdin);
    }
    close(sock);
    return 0; 
} 