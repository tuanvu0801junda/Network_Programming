#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h>
#include <ctype.h>
#define PORT 8080 

void ToUp( char *p ) { 
	while( *p ) { 
		*p=toupper( *p ); 
		p++; 
	} 
} 

int main(int argc, char const *argv[]) { 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1, addrlen = sizeof(address); 
    char mess_from_client[300], buffer[1024], hello[300];

    //tao socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }      

    //gan dia chi cho socket, port = 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT);  //gan cong la 8080   

    // bind 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }

    //listen, chi dinh socket lang nghe ket noi
    if (listen(server_fd, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 

    //accept, chap nhan ket noi
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 

    while(1){    
	    char str_cli_ip[INET_ADDRSTRLEN];
	    struct sockaddr_in* ip_client = (struct sockaddr_in*)&address;
	    inet_ntop(AF_INET, &ip_client->sin_addr, str_cli_ip, INET_ADDRSTRLEN);
	    printf("ipclient: %s\n", str_cli_ip );
	    char str_cli_port[INET_ADDRSTRLEN];
	    printf("port: %d\n", ntohs(ip_client->sin_port));

	    printf("\nMessage from client: ");
	    //read, doc du lieu gan vao bien valread tra ve so byte ma no doc duoc
	    valread = read( new_socket, buffer, 1024);
        printf("%s\n",buffer); 

	    // //viet hoa
	    // ToUp(buffer); 
	    // //gan bien hello tra ve cho client la buffer da viet hoa
        // strcpy(hello,buffer);
	    // printf("%s\n",buffer ); 

        printf("Your message to client: ");
        gets(hello);
	    send(new_socket, hello, strlen(hello), 0 ); 
    }
    close(new_socket);
    return 0; 
} 
