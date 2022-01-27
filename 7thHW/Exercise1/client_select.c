#include <stdio.h>
#include <netinet/in.h>     // struct sockaddr_in, htons(serverPort)
#include <arpa/inet.h>      // inet_addr()
#include <sys/socket.h>     // socket()
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFF_SIZE 1024
#define PAYLOAD_SIZE 1014
#define PATH_LEN 100
#define FILE_LEN 512

void preProcess(int client_sock, char opcode);

int main(int argc, char* argv[]){
    if (argc != 3){
        printf("[o] Error! Need exactly 3 arguments\n");
        printf("Type ./client <IP_address> <Port_Number>\n");
        return -2;
    }
    int client_sock;
    struct sockaddr_in serverAdd;
    int recvBytes, choice=0;
    char buff[BUFF_SIZE+1];
    char *token;
    FILE *f;

    // Construct socket
    client_sock = socket(AF_INET,SOCK_STREAM,0);
    if(client_sock <0){
        perror("[x] Cannot creat socket!");
        return -1;
    }

    // Specify server address
    serverAdd.sin_family = AF_INET;
    serverAdd.sin_port = htons(atoi(argv[2]));
    serverAdd.sin_addr.s_addr = inet_addr(argv[1]);

    // Request to connect server
    if(connect(client_sock,(struct sockaddr*)&serverAdd, sizeof(struct sockaddr)) <0){
        perror("[x] Error! Cannot connect to server!");
        return -1;
    }
    
    while(1){
        printf("\n----------CAESAR ENCODE & DECODE----------\n");
        printf("1. Encode a message\n");
        printf("2. Decode a message\n");
        printf("3. Exit\n");
        printf("Your choice: ");
        scanf("%d%*c",&choice);
        switch(choice){
            case 1:
                printf("1. Encode a message\n");
                preProcess(client_sock,'0');
                recvBytes = recv(client_sock,buff,BUFF_SIZE,0);
                if(recvBytes < 0){
                    printf("[x] Cannot receive from server\n");
                    return -1;
                }
                if (buff[0] == '3'){
                    printf("[x] From server: ENCODING FAILED!\n");
                } else if (buff[0] == '2'){
                    f = fopen("./clientFiles/result.txt","w");
                    if (f == NULL){
                        printf("[x] Cannot open result.txt\n");
                        return -1;
                    }
                    printf("[o] Receiving result file ... \n");
                    token = strtok(buff,"-");
                    token = strtok(NULL,"-");
                    token = strtok(NULL,"-");
                    fprintf(f,"%s",token);
                    fclose(f);
                    // sendBytes = send(client_sock,"GOTIT",6,0);
                    // if(sendBytes < 0){
                    //     printf("[x] Error: Cannot send GOTIT\n");
                    //     return -1;
                    // }
                    recvBytes = recv(client_sock,buff,BUFF_SIZE,0);
                    if (buff[0] == '2'){
                        printf("\n[o] From server: ENCODING complete! \n");
                        printf("[o] Please check file 'result.txt' in folder 'clientFiles'.\n");
                    }
                }
                break;
            case 2:
                printf("2. Decode a message\n");
                printf("[o] [Optional]: Try key = 5 with './clientFiles/file2.txt' \n");
                printf("[o] Or you can create your own encoded-file with your own key\n");
                preProcess(client_sock,'1');
                recvBytes = recv(client_sock,buff,BUFF_SIZE,0);
                if(recvBytes < 0){
                    printf("[x] Cannot receive from server\n");
                    return -1;
                }
                if (buff[0] == '3'){
                    printf("[x] From server: DECODING FAILED!\n");
                } else if (buff[0] == '2'){
                    f = fopen("./clientFiles/result.txt","w");
                    if (f == NULL){
                        printf("[x] Cannot open result.txt\n");
                        return -1;
                    }
                    printf("[o] Receiving result file ... \n");
                    token = strtok(buff,"-");
                    token = strtok(NULL,"-");
                    token = strtok(NULL,"-");
                    fprintf(f,"%s",token);
                    fclose(f);
                    // sendBytes = send(client_sock,"GOTIT",6,0);
                    // if(sendBytes < 0){
                    //     printf("[x] Error: Cannot send GOTIT\n");
                    //     return -1;
                    // }
                    recvBytes = recv(client_sock,buff,BUFF_SIZE,0);
                    if (buff[0] == '2'){
                        printf("\n[o] From server: DECODING complete! \n");
                        printf("[o] Please check file 'result.txt' in folder 'clientFiles'.\n");
                    }
                }
                break;
            case 3: 
                printf("3. Exit\n");
                break;
            default:
                printf("[x] Your choice doesn't match!\n");
                break;
        }
        if(choice == 3) {
            send(client_sock,"END",4,0);
            break;
        }
    }
    close(client_sock);
}

void preProcess(int client_sock, char opcode){
    char buff[BUFF_SIZE+1], path[PATH_LEN];
    char payload[PAYLOAD_SIZE];
    short length;   // 2 byte (opcode: 1 byte)
    int sendBytes, recvBytes, shift;
    FILE *file;

    // 1. Step 1: Send key with opcode '1' or '0' to server
    do {
        printf("\n[o] Example: Key '3' means 'shift 3 places' (A -> D, B -> E) \n");
        printf("[o] Encode (key should be any positive number)\n");
        printf("Enter your key (a positive number): ");
        scanf("%d%*c",&shift);
    }
    while(shift < 0);
    sprintf(buff,"%c-%d-%d-",opcode,1,shift);
    sendBytes = send(client_sock,buff,strlen(buff),0);
    if (sendBytes < 0){
        perror("[x] Error: Cannot send\n");
        return;
    }
    
    recvBytes = recv(client_sock,buff,BUFF_SIZE,0);
    if(strcmp(buff,"KEY") != 0){
        printf("[x] Wrong key!\n");
        return;
    }
    // 2. Step 2: Send file to server with opcode '2' (length > 0)
    printf("\nNote: Client's files are stored in folder 'clientFiles'\n");
    if(opcode == '0'){
        printf("Encode Example File: './clientFiles/file1.txt'\n");
        printf("Enter file path: ");
        fgets(path,PATH_LEN,stdin);
    } else if (opcode == '1'){
        printf("Decode Example File: './clientFiles/file2.txt'\n");
        printf("Enter file path: ");
        fgets(path,PATH_LEN,stdin);
    }
    length = strlen(path);
    if(path[length-1] == '\n') path[length-1] = '\0';
    file = fopen(path, "r");
    if (file == NULL){
        perror("[x] Error: File not found!");
        return;
    }

    opcode = '2';
    fread(payload,PAYLOAD_SIZE,1,file);
    fclose(file);
    length = strlen(payload);
    sprintf(buff,"%c-%d-%s-",opcode,length,payload);
    sendBytes = send(client_sock,buff,strlen(buff),0);
    //printf("\n[o] Message Content: \"%s\"\n",buff);
    if(sendBytes <= 0){
        perror("[x] Error: Cannot send file\n");
        return;
    }

    recvBytes = recv(client_sock,buff,strlen(buff),0);
    buff[recvBytes] = '\0';
    if(strcmp(buff,"KEYOK") != 0){    
        //KEY OK
        perror("[x] Error: Wrong message\n");
        return;
    }

    // 3. Step 3: Finish sending, opcode '2' (length == 0), payload ""
    strcpy(buff,"");
    sprintf(buff,"%c-%d-%s-",opcode,0,"");
    sendBytes = send(client_sock,buff,strlen(buff),0);
    if(sendBytes < 0){
        perror("[x] Error: Cannot send file\n");
        return;
    }
    printf("\n[o] Sending File to server: DONE\n");
}