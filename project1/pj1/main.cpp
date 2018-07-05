#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

using namespace std;

int main()
{
    FILE *fp;
    fp = fopen("config", "r");
    char buf[100];
    char *tmp;
    char *des;
    char *pass;
    fgets(buf, 100, fp);
    tmp = strtok(buf, "'");
    des = strtok(NULL, "'");
    //printf("%s\n", des);
    char msg[1000] = "JOIN ";
    //printf("%s\n", msg);
    strcat(msg, des);
    strcat(msg, " ");
    fgets(buf, 100, fp);
    tmp = strtok(buf, "'");
    pass = strtok(NULL, "'");
    //printf("%s\n", pass);
    strcat(msg, pass);
    //printf("%s\n", msg);
    strcat(msg, "\r\n");
    
    struct addrinfo hints;
    struct addrinfo *servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo("irc.freenode.net", "6667", &hints, &servinfo) != 0)
        printf("ohhhh\n");
    int sockfd;
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        printf("goddamn\n");

    char buffer[10000];
    char pass_msg[1000];
    /*recv(sockfd, buffer, sizeof(buffer), 0);
    printf("\n%s", buffer);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("\n%s", buffer);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("\n%s", buffer);*/
    strcpy(pass_msg, "PASS afdjafklsjf\r\n");
    send(sockfd, pass_msg, sizeof(pass_msg), 0);
    cout << pass_msg;
    strcpy(pass_msg, "NICK qaz741987\r\n");
    send(sockfd, pass_msg, sizeof(pass_msg), 0);
    cout << pass_msg;
    strcpy(pass_msg, "USER qqq qqq qqq qqq\r\n");
    send(sockfd, pass_msg, sizeof(pass_msg), 0);
    cout << pass_msg;
    
    strcpy(pass_msg, "JOIN #channel\r\n");
    send(sockfd, pass_msg, sizeof(pass_msg), 0);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("\n%s", buffer);

    
    while(1){
        scanf("%s\r\n", pass_msg);
        cout << pass_msg;
        send(sockfd, pass_msg, sizeof(pass_msg), 0);
    }
    
    /*char *nick = "Frank";
    char *t = "zzzzzzz";
    char *p = "secret";
    sprintf(pass_msg, "PASS %s\r\n", p);
    printf("%s", pass_msg);
    if(send(sockfd, pass_msg, sizeof(pass_msg), 0) == -1)
        printf("woo\n");
    sprintf(pass_msg, "NICK %s\r\n", nick);
    printf("%s", pass_msg);
    if(send(sockfd, pass_msg, sizeof(pass_msg), 0) == -1)
        printf("woola\n");
    sprintf(pass_msg, "USER %s %s %s %s\r\n", t, t, t, t);
    printf("%s", pass_msg);
    if(send(sockfd, pass_msg, sizeof(pass_msg), 0) == -1)
        printf("woo\n");*/

    /*strcpy(pass_msg, "PASS secret\r\n");
    printf("%s", pass_msg);
    send(sockfd, pass_msg, sizeof(pass_msg), 0);
    strcpy(pass_msg, "NICK Frank\r\n");
    printf("%s", pass_msg);
    send(sockfd, pass_msg, sizeof(pass_msg), 0);
    strcpy(pass_msg, "USER x x x x\r\n");
    printf("%s", pass_msg);
    send(sockfd, pass_msg, sizeof(pass_msg), 0);*/
    

    while(1) { 
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
    printf("%s", msg);}
    //send(sockfd, msg, sizeof(msg), 0);
    /*while(1) {
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
    }*/

        
    /*char buffer[100];
    int sockfd;
    socklen_t length;
    struct sockaddr_in serverAddress;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6667);
    struct hostent *h =gethostbyname("irc.freenode.net");
    struct in_addr **addr = (struct in_addr **)h->h_addr_list;
    char ip[1000];
    for(int i = 0; addr[i] != NULL; i++)
        strcpy(ip, inet_ntoa(*addr[i]));
    printf("%s\n", ip);
    serverAddress.sin_addr.s_addr = ip;
    connect(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    bzero(buffer, sizeof(buffer));
    char a[10] = "/connect";
    send(sockfd, a, sizeof(a), 0);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s", buffer);
    close(sockfd);*/
        
    




    return 0;
}

