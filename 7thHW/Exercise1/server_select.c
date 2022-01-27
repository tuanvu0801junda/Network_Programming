#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>     //socklen_t
#include <sys/select.h>     //FD_SETSIZE, fd_set
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#define BUFF_SIZE 1024
#define BACKLOG 20

int charToInt(char c);
int encode(int shift, char *message);
int decode(int shift, char *message);
int resolve(int sockfd, char *token, int shift, int ret, char* control);

int main(int argc, char*argv[]){
    if(argc != 2){
        printf("[o] Error! Need exactly 2 arguments\n");
        printf("Type ./server <Port_Number>\n");
        return -1;
    }
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client[FD_SETSIZE];
    ssize_t ret;
    fd_set readfds, allset;
    char recvBuff[BUFF_SIZE], *token;
    socklen_t cliLen;
    struct sockaddr_in cliAddr, servAddr;
    int shift = 0;

    // Construct connection
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (listenfd == -1){
        perror("[x] Socket set up failed!");
        return -1;
    }

    // Binding address
    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if(bind(listenfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1){
        perror("[x] Binding failed! \n");
        return -1;
    }

    // Listen & request 
    if(listen(listenfd, BACKLOG) == -1){
        perror("[x] Listening failed! \n");
        return -1;
    }

    maxfd = listenfd;   // initialize (max value of fd != FD_SETSIZE)
    maxi = -1;          // max index of client[] array
    for(i=0;i<FD_SETSIZE;i++) 
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd,&allset);

    printf("Server started processing ...\n\n");

    // Communicating with client(s)
    while(1){
        readfds = allset;
        nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
        if(nready < 0){
            perror("[x] Error: select < 0");
            return -1;
        }

        if(FD_ISSET(listenfd,&readfds)){
            // new client connection
            cliLen = sizeof(cliAddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliAddr, &cliLen);
            if(connfd < 0){
                perror("[x] Can't accept new connection!\n");
                return -1;
            } else {
                printf("New connection from %s[port: %d]\n",inet_ntoa(cliAddr.sin_addr),ntohs(cliAddr.sin_port));
                for(i=0;i<FD_SETSIZE;i++){
                    if (client[i] < 0){
                        client[i] = connfd; // A "blank (-1)" is filled 
                        break;
                    }
                }
                if(i == FD_SETSIZE){
                    printf("[x]Too many clients\n");
                    close(connfd);
                }

                FD_SET(connfd,&allset);     // add new fd to set
                if(connfd > maxfd) maxfd = connfd;
                if(i>maxi) maxi = i;
                if(--nready <= 0) continue;
            }
        }

        for(i=0;i<=maxi;i++){
            //check all clients 
            if ((sockfd = client[i]) < 0)
                continue;

            if(FD_ISSET(sockfd, &readfds)){
                // Step 0: receive shift (key)
                ret = recv(sockfd,recvBuff,BUFF_SIZE,0);
                recvBuff[ret] = '\0';
                if (strcmp(recvBuff,"END") == 0){
                    FD_CLR(sockfd, &allset);
                    close(sockfd);
                    client[i] = -1;
                }
                if(recvBuff[0] == '0' || recvBuff[0] == '1'){
                    printf("\n-------------------------------\n");
                    printf("[o] Receiving Key: DONE!\n");
                    token = strtok(recvBuff,"-");
                    token = strtok(NULL,"-");
                    token = strtok(NULL,"-");
                }
                if(recvBuff[0] == '0'){
                    //encode
                    resolve(sockfd,token,shift,ret,"Encoding");
                } else if (recvBuff[0] == '1'){
                    //decode
                    resolve(sockfd,token,shift,ret,"Decoding");
                }
            }
            if (--nready <= 0) break;		/* no more readable descriptors */
			
        }
    }
}

int charToInt(char c){
    return c - '0';
}

int encode(int shift, char *message){
    int i=0;
    int result = 1;
    if (shift < 0) result = -1;
    for(i=0;i<strlen(message)-1;i++){
        if(!isprint(message[i]) && message[i] != EOF && message[i] != '\n') result = 0;     // HAVING UN-PRINTABLE CHARACTER
        if (!isalpha(message[i])) continue;
        if(isupper(message[i])){
            // Encode UPPERCASE
            message[i] = (message[i] + shift - 65)%26 + 65;
        } else {
            message[i] = (message[i] + shift - 97)%26 + 97;
        }
    }
    return result;
}

int decode(int shift, char *message){
    int i=0, mark=0, result=1;
    if (shift < 0) result = -1;
    for(i=0;i<strlen(message)-1;i++){
        if(!isprint(message[i]) && message[i] != EOF && message[i] != '\n') result = 0;     // HAVING UN-PRINTABLE CHARACTER
        if (!isalpha(message[i])) continue;
        if(isupper(message[i])){
            // Decode UPPERCASE
            mark = message[i] - shift - 65;
            if(mark < 0) message[i] = (message[i] - shift - 65)%26 + 65 + 26; 
            else message[i] = (message[i] - shift - 65)%26 + 65;
        } else {
            mark = message[i] - shift - 97;
            if(mark < 0) message[i] = (message[i] - shift - 97)%26 + 97 + 26;
            else message[i] = (message[i] - shift - 97)%26 + 97;
        }
    }
    return result;
}

int resolve(int sockfd, char *token, int shift, int ret, char* control){
    FILE *file;
    int result;
    char sendBuff[BUFF_SIZE*2], recvBuff[BUFF_SIZE];

    // Step 1: Receive and process key
    shift = charToInt(token[0]);
    ret = send(sockfd,"KEY",4,0);
    
    // Step 2: receive content of file
    ret = recv(sockfd,recvBuff,BUFF_SIZE,0);
    recvBuff[ret] = '\0';
    printf("[o] Received file: DONE!\n\n");
    file = fopen("./serverFiles/temp.txt","w");
    if(file == NULL) {
        printf("[x] Cannot open 'temp.txt'\n");
        return -1;
    }
    token = strtok(recvBuff,"-");
    token = strtok(NULL,"-");
    token = strtok(NULL,"-");
    fprintf(file,"%s",token);
    fclose(file);

    ret = send(sockfd, "KEYOK", 6, 0);
    ret = recv(sockfd,recvBuff,BUFF_SIZE,0);
    recvBuff[ret] = '\0';
    if((recvBuff[0] == '2') & (recvBuff[2] == '0')){
        printf("[o] %s ...  \n",control);
    }

    // Step 3: Encoding & Decoding 
    if (strcmp(control,"Encoding") == 0){
        result = encode(shift,token);
    } 
    if (strcmp(control,"Decoding") == 0){
        result = decode(shift,token);
    }
    if (result == -1){
        sprintf(sendBuff,"%c-%d-%s-",'3',0,"");
        ret = send(sockfd,sendBuff,strlen(sendBuff),0);
        if(ret < 0){
            printf("[x] %s: FAILED! \n",control);
            return -1;
        }
        printf("[x] %s: FAILED! \n",control);
        printf("[x] Shift (Key) < 0 !\n");
        return -2;
    } else if (result == 0){
        printf("[x] File content may have UN-PRINTABLE character ... \n");
    }

    file = fopen("./serverFiles/result.txt","w");
    if(file == NULL) {
        printf("[x] Cannot open 'temp.txt'\n");
        return -1;
    }
    fprintf(file,"%s",token);
    fclose(file);

    // Step 4: Encapsulate & Send back
    sprintf(sendBuff,"%c-%ld-%s-",'2',strlen(token),token); 
    printf("[o] %s: DONE! \n\n",control);
    ret = send(sockfd,sendBuff,strlen(sendBuff),0);
    if(ret < 0){
        perror("[x] Cannot send file\n");
        return -1;
    }
    sendBuff[ret] = '\0';

    sprintf(sendBuff,"%c-%d-%s-",'2',0,""); 
    printf("[o] Preparing to send file back ...\n");
    ret = send(sockfd,sendBuff,strlen(sendBuff),0);
    if(ret < 0){
        perror("[x] Cannot send file\n");
        return -1;
    }

    sendBuff[ret] = '\0';
    printf("[o] Sending file back: DONE !\n\n");

    // Step 5: Delete temporary file
    if (remove("./serverFiles/temp.txt") != 0 || remove("./serverFiles/result.txt") != 0){
        perror("[x] Cannot delete file\n");
        return -1;                            
    }  
    return 1;
}