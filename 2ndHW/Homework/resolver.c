#include <stdio.h>
#include <netinet/in.h> // struct in_addr 
#include <arpa/inet.h>  // inet_ntoa
#include <netdb.h>      // struct hostent
#include <string.h>     // strlen
#include <ctype.h>
#include "util.h"


int main(int argc, char* argv[]){
    if (argc != 2) printf("argc = %d. Need exactly 2 arguments\n",argc);
    else{
        int i=0,flag=0;
        struct hostent *hostInfo;
        struct in_addr **addr_IP_list, **addr_name_list, addr;
        if(validateIP(argv[1]) == TRUE){
            inet_aton(argv[1], &addr);
            hostInfo = gethostbyaddr(&addr, sizeof(addr), AF_INET);
            if (hostInfo != NULL){
                flag = 1;
                printf("Official name: %s\n", hostInfo->h_name);
                printf("Alias name: \n");
                addr_name_list = (struct in_addr **)hostInfo->h_aliases;
                for(i = 0; addr_name_list[i] != NULL; i++) {
                    if(i != 0) printf("%s \n",inet_ntoa(*addr_name_list[i]));
                }
            } else {printf("Not found information\n"); return 0;}
        }
        
        if (flag == 0){
            long len = strlen(argv[1]);
            if(isdigit(argv[1][len-1])) {printf("Invalid address\n");return -1;}
            else{
                hostInfo = gethostbyname(argv[1]);
                if (hostInfo != NULL){
                    flag = 1;
                    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*)hostInfo->h_addr));
                    printf("Alias IP: \n");
                    addr_IP_list = (struct in_addr **)hostInfo->h_addr_list;
                    for(i = 0; addr_IP_list[i] != NULL; i++) {
                        if(i != 0) printf("%s \n",inet_ntoa(*addr_IP_list[i]));
                    }   
                }
            }
        }

        if (flag == 0) printf("Not found information\n");

    }
}