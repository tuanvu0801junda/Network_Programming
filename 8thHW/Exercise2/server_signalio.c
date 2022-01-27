#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#define BUFF_SIZE 1024

int sock;
char buff[BUFF_SIZE];
int i;
struct hostent *host;
struct in_addr ipv4addr;

void signalio_process(int); 
int isValid(char *ip);
char *convertIPToDomainName(char *ip);
char *convertDomainToIP(char *address);
char *getConvertedResult(char *buff);

int main(int argc, char *argv[]){
    struct sockaddr_in server;
    socklen_t size = sizeof(server);

    if (argc != 2){
        printf("[x] Need 2 arguments!\n");
        printf("[x] Retry with './server <port_number>'\n");
        return -1;
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0){
        perror("[x] Construct socket: FAILED\n");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0){
        perror("[x] Binding: FAILED\n");
        return -1;
    }

    if (getsockname(sock, (struct sockaddr *)&server, &size) < 0){
        perror("[x] GetSockName: FAILED\n");
        return -1;
    }

    printf("[o] Server started processing ... \n\n");
    signal(SIGIO, signalio_process);

    if (fcntl(sock, __F_SETOWN, getpid()) < 0){
        perror("[x] fcntl F_SETOWN: ERROR\n");
        return -1;
    }

    if (fcntl(sock, F_SETFL, FASYNC) < 0) {
        perror("[x] fcntl F_SETFL, FASYNC: ERROR\n");
        return -1;
    }

    while(1){}
}

void signalio_process(int signal){
    int recvByte, sendByte;               
    struct sockaddr_in serverAddr; 
    socklen_t addr_len = sizeof(serverAddr);

    recvByte = recvfrom(sock, buff, BUFF_SIZE, 0,
                    (struct sockaddr *)&serverAddr, &addr_len);
    if (recvByte < 0){
        printf("[x] Cannot receive from server");
        return;
    }
    buff[recvByte - 1] = '\0'; //remove "\n"

    printf("[o] Received a new request from %s[port: %d]\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
    char *reply = getConvertedResult(buff);

    sendByte = sendto(sock, reply, strlen(reply), 0, 
                    (struct sockaddr *)&serverAddr, addr_len);
    if (sendByte < 0){
        printf("[x] Cannot send to server");
        return;
    }
    printf("[o] Process: DONE!\n\n");
}

int isValid(char *ip){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    if (result != 0) return 1;
    else return 0;
}

char *convertIPToDomainName(char *ip){
    char *message = "Official name: ";
    char reply[BUFF_SIZE];
    inet_pton(AF_INET, ip, &ipv4addr);
    if ((host = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET)) == NULL){
        herror("[x] Cannot get IP Address\n");
        return "Not found information\n";
    }
    else{
        strcpy(reply, message);
        message = strcat(reply, host->h_name);
        strcat(message, "\nAlias name:\n");
        for (i = 0; host->h_aliases[i] != NULL; i++){
            strcpy(reply, message);
            message = strcat(reply, host->h_aliases[i]);
            strcat(message, "\n");
        }
        return message;
    }
}

char *convertDomainToIP(char *address){
    char *message = "Official IP: \n";
    char reply[BUFF_SIZE];
    char contain[BUFF_SIZE];
    if ((host = gethostbyname(address)) == NULL){
        herror("[x] Cannot get domain name");
        return "IP Address is invalid\n";
    }
    else{
        for (i = 0; host->h_addr_list[i] != NULL; i++){
            strcpy(reply, message);
            message = strcat(reply, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
            strcat(message, "\n");
        }
        strcat(message, "\nAlias IP address: \n");

        for (i = 0; host->h_aliases[i] != NULL; i++){
            strcpy(contain, message);
            message = strcat(contain, inet_ntoa(*(struct in_addr *)host->h_aliases[i]));
            strcat(message, "\n");
        }
        return message;
    }
}

char *getConvertedResult(char *buff){
    char *result;
    if (strcmp(buff, "") == 0) result = "STOP";
    else if (isValid(buff)) result = convertIPToDomainName(buff);
    else result = convertDomainToIP(buff);
    return result;
}