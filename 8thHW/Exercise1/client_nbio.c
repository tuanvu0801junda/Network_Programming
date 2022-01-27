#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/uio.h>

#define BUFF_SIZE 512

int isValid(char *ip);

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("[x] Need 3 arguments!\n");
        printf("[x] Retry with './client <server_address> <port_number>'\n");
        return -1;
    }

    if (isValid(argv[1]) == 0){
        printf("[x] %s isn't a valid ip address\n", argv[1]);
        return -1;
    }

    int client_sock, result;
    struct sockaddr_in server_addr; 
    char buff[BUFF_SIZE], digit[BUFF_SIZE], letter[BUFF_SIZE];
    char len1[BUFF_SIZE], len2[BUFF_SIZE];
    ssize_t bytes_sent, bytes_received;
    struct iovec sendMsg[1], recvMsg[2];

    // Step 1: Construct socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Step 2.5: Set client socket to NON-BLOCKING
    fcntl(client_sock, F_SETFL, O_NONBLOCK); 
    fcntl(client_sock, F_SETFL, O_ASYNC);

    // Step 3: Request to connect server
    result = connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (result < 0){
        printf("[x] Connection failed!\n");
        return -1;
    }

    // Step 4: Communicate with server
    while (1){
        memset(buff, '\0', (strlen(buff) + 1));
        printf("\n[o] Press \"Enter\" to escape\n");
        printf("[o] Input a string (digit & letter): ");
        fgets(buff, BUFF_SIZE, stdin);
        // if (strcmp(buff, "\n") == 0){
        //     printf("\n[o] Client stopped !\n\n");
        //     break;
        // }

        sendMsg[0].iov_base = buff;
        sendMsg[0].iov_len = strlen(buff);

        memset(len1, '\0', (strlen(len1) + 1));
        memset(len2, '\0', (strlen(len2) + 1));

        bytes_sent = writev(client_sock, sendMsg, 1);
        if (bytes_sent <=0){
            printf("[x] WriteV: FAILED\n");
            return -1;
        }

        recvMsg[0].iov_base = &digit;
        recv(client_sock,len1,BUFF_SIZE,0);
        recvMsg[0].iov_len = atoi(len1);

        recvMsg[1].iov_base = &letter;
        recv(client_sock,len2,BUFF_SIZE,0);
        recvMsg[1].iov_len = atoi(len2);

        bytes_received = readv(client_sock, recvMsg, 2);
        if (bytes_received < 0){
            printf("[x] Received nothing from server!\n");
            return -1;
        }        
        if (strcmp(buff, "\n") == 0){
            printf("\n[o] Client stopped !\n\n");
            send(client_sock, "STOPC", strlen("STOPC"), 0);
            sleep(1 / 2);
            break;
        } else {
            digit[atoi(len1)] = '\0';
            letter[atoi(len2)] = '\0';
            printf("[o] Reply from server: \n");
            printf(" %s\n %s\n", digit, letter);
            send(client_sock, "NO", strlen("NO"), 0);
            sleep(1 / 2);
        }
        recv(client_sock, buff, BUFF_SIZE - 1, 0);
    }

    // Step 4: Close socket
    close(client_sock);
}

int isValid(char *ip){
    struct sockaddr_in sample;
    int result = inet_pton(AF_INET, ip, &(sample.sin_addr));
    if (result != 0) return 1;
    else return 0;
}