#include <stdio.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <ctype.h>
#include <sys/uio.h>

#define BACKLOG 5 
#define BUFF_SIZE 512

void processMessage(int sockfd, char *buffer, int len){
    struct iovec filter[2];
    int count_digit = 0, count_letter = 0, errFlag = 0;
    char digit[BUFF_SIZE], letter[BUFF_SIZE];
    char length_1[BUFF_SIZE], length_2[BUFF_SIZE];
    memset(length_1, '\0', (strlen(length_1) + 1));
    memset(length_2, '\0', (strlen(length_2) + 1));

    if (strcmp(buffer, "\n") != 0) {
        for (int i = 0; i < len - 1; i++){
            if (isdigit(buffer[i])) digit[count_digit++] = buffer[i];
            else if (isalpha(buffer[i])) letter[count_letter++] = buffer[i];
            else errFlag = 1;
        }
    }
    digit[count_digit] = '\0';
    letter[count_letter] = '\0';
    
    if (errFlag == 1){
        filter[0].iov_base = "Error";
        filter[0].iov_len = sizeof("Error\n");
        sprintf(length_1, "%ld", filter[0].iov_len);
        send(sockfd, length_1, strlen(length_1), 0);
        sleep(1/2);
        filter[1].iov_base = "";
        filter[1].iov_len = strlen(filter[1].iov_base);
        sprintf(length_2, "%ld\n", filter[1].iov_len);
        send(sockfd, length_2, strlen(length_2), 0);
        sleep(1/2);
    }
    else{
        filter[0].iov_base = digit;
        filter[0].iov_len = count_digit;
        sprintf(length_1, "%d", count_digit);
        send(sockfd, length_1, strlen(length_1), 0);
        sleep(1/2);

        filter[1].iov_base = letter;
        filter[1].iov_len = count_letter;
        sprintf(length_2, "%d\n", count_letter);
        send(sockfd, length_2, strlen(length_2), 0);
        sleep(1/2);
    }
    writev(sockfd, filter, 2);
    sleep(1 / 2);
    //printf("digit: %s, letter: %s\n", (char*)filter[0].iov_base, (char*)filter[1].iov_base);
}

int main(int argc, char *argv[]){
    if (argc != 2){
        printf("[x] Need 2 arguments!\n");
        printf("[x] Retry with './server <port_number>'\n");
        return -1;
    }
    int i, maxi, max_fd, listen_fd, conn_fd, sockfd;
    int n_ready, client[FD_SETSIZE], recvBytes;
    fd_set readfds, allset;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct iovec iov_1[1];
    char recv_data[BUFF_SIZE], ctrlMessage[BUFF_SIZE];

    // Step 1: Construct a TCP socket to listen connection request
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){ 
        perror("[x] Construct socket: FAILED\n");
        return -1;
    }

    // Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1){
        perror("[x] Binding: FAILED\n");
        return -1;
    }
    fcntl(listen_fd, F_SETFL, O_NONBLOCK); 
    fcntl(listen_fd, F_SETFL, O_ASYNC);

    // Step 3: Listen request from client
    if (listen(listen_fd, BACKLOG) == -1){ 
        perror("[x] Listening: FAILED\n");
        return -1;
    }

    max_fd = listen_fd; /* initialize */
    maxi = -1;          
    for (i = 0; i < FD_SETSIZE; i++)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listen_fd, &allset);

    printf("[o] Server started processing ... \n\n");
    // Step 4: Communicate with clients
    while (1){

        // 1. Construct 'select'
        readfds = allset; 
        n_ready = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (n_ready < 0){
            perror("[x] Error: Negative n_ready");
            return 0;
        }

        // 2. Get new connection 
        if (FD_ISSET(listen_fd, &readfds)){ 
            clilen = sizeof(cliaddr);
            if ((conn_fd = accept(listen_fd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
                perror("[x] Accept: FAILED\n");
            else{
                printf("You got a connection from %s[port: %d]\n", 
                        inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port)); 
                for (i = 0; i < FD_SETSIZE; i++)
                    if (client[i] < 0){
                        client[i] = conn_fd; 
                        break;
                    }
                if (i == FD_SETSIZE){
                    printf("\nToo many clients");
                    close(conn_fd);
                }

                FD_SET(conn_fd, &allset); 
                if (conn_fd > max_fd) max_fd = conn_fd; 
                if (i > maxi) maxi = i; 
                if (--n_ready <= 0) continue; 
            }
        }

        // 3. For loop & send reply back
        for (i = 0; i <= maxi; i++){ 
            if ((sockfd = client[i]) < 0) continue;
            if (FD_ISSET(sockfd, &readfds)) {
                memset(recv_data, '\0', (strlen(recv_data) + 1));
                
                // iov_1[0].iov_base = recv_data;
                // iov_1[0].iov_len = readv(sockfd, iov_1, 1);
                iov_1[0].iov_base = recv_data;
                iov_1[0].iov_len = BUFF_SIZE;
                ssize_t recv_byte = readv(sockfd, iov_1, 1);
                iov_1[0].iov_len = recv_byte;
                strcpy(recv_data,iov_1[0].iov_base);
                recv_data[recv_byte] = '\0';
                iov_1[0].iov_base = recv_data;

                processMessage(sockfd, iov_1[0].iov_base, iov_1[0].iov_len);

                recvBytes = recv(sockfd, ctrlMessage, BUFF_SIZE - 1, 0);
                if (recvBytes < 0){
                    printf("[x] Received nothing from client!\n");
                    break;
                }
                ctrlMessage[recvBytes] = '\0';
                if (strcmp(ctrlMessage, "STOPC") == 0){
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                    client[i] = -1;
                }
                send(sockfd, "FINISH", strlen("FINISH"), 0);
                if (--n_ready <= 0) break; 
            }
        }
    }
    return 0;
}
