#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>

//Send file by UDP
//Reliable transmission : data / timeout & retran / seq num
//Congestion control : slow start 1 2 4 8...16 17
//packet loss or loss => set threshold = ?, con win = 1, retran

//Show : send, recv, data, ack, fin, finack, seq num, timeout, retran, winSizem, threshold

//arg : IP, Port, filename
//Default Threshold = 16
//File size > 500KB
//Packet Size = 1KB
//Time out < 1 sec

int main(int argc, char **argv)
{
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    char buf[2000], mes[2000];
    char IP[100];
    int port = atoi(argv[2]);
    strcpy(IP, argv[1]);
    time_t timer;

    //create UDP
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct timeval tv;
    /*tv.tv_sec = 0;
    tv.tv_usec = 300000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));*/

    if(s == -1){
        printf("Socket!!!\n");
        return 0;
    }


    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = inet_addr(argv[1]);
    //int ch = inet_pton(AF_INET, SERVER_ADDRESS, &(si_me.sin_addr));

    strcpy(mes, "receiver");
    //listen
    sendto(s, mes, strlen(mes), 0, (struct sockaddr *)&si_me, slen);
    memset(mes, '\0', sizeof(mes)); 
    recvfrom(s, mes, 20, 0, (struct sockaddr *)&si_me, &slen);
    printf("%s", mes);
    FILE *fp = fopen(argv[3], "w");
    
    char word[32][2000];
    int count = 0; 
    int base = 1;
    for(int i = 0; i < 32; i++) 
        memset(word[i], '\0', sizeof(word[i]));

    char tmp[2000];
    while(1) {
        memset(tmp, '\0', sizeof(tmp));
        recvfrom(s, tmp, 2000, 0, (struct sockaddr *)&si_me, &slen);
        int num;
        //char m_in[2000];
        sscanf(tmp, "%*s %d", &num);
        //printf("!!!!data # %s\n", tmp);
        int fin = 0;
        if(num == -1) {  //fin
            printf("recv fin\n");
            fin = 1;
            sendto(s, "r -1 finack", 20, 0, (struct sockaddr *)&si_me, slen);
            printf("send finack\n");
        }
        else if(num < base) {  //ignor
            printf("ignr data #%d\n", num);
            printf("send ack #%d\n", num);
            char num_t[200];
            sprintf(num_t, "r %d ack", num);
            sendto(s, num_t, strlen(num_t)+1, 0, (struct sockaddr *)&si_me, slen);
        }
        else if(num < base + 32){  //recv
            if(word[num-base][0] != '\0') {   //ignor
                printf("ignr data #%d\n", num);
            }
            else {
                count++;
                char in[2000], a[100], b[100];
                strcpy(in, tmp);
                sscanf(tmp, "%s %s\n", a, b);
                for(int k = 0; k < strlen(a)+1+strlen(b)+1; k++) {
                    memmove(in, in+1, strlen(in));
                    in[strlen(in)] = '\0';
                }
                strcpy(word[num-base], in);
                printf("recv data #%d\n", num);
                //printf("%s", in);
            }
            printf("send ack #%d\n", num);
            char num_t[200];
            sprintf(num_t, "r %d ack", num);
            sendto(s, num_t, strlen(num_t)+1, 0, (struct sockaddr *)&si_me, slen);
        }
        else if(num-base > 32) {  //drop
            printf("drop data #%d\n", num); 
        }

        int check = 1;
        for(int i = 0; i < 32; i++) {
            if(word[i][0] == '\0'){
                check = 0;
                break;
            }
        }

        if(check == 1 || fin == 1) {  //flush!
            printf("flush\n");
            for(int i = 0; i < count; i++) {
                fwrite(word[i], 1, strlen(word[i])*sizeof(char), fp);
                memset(word[i], '\0', sizeof(word[i]));
            }
            base += 32;
            count = 0;
            if(fin == 1)
                break;
        }
    }

    fclose(fp);

    return 0;
}

