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

//arg : IP, Port, loss rate
//Default Threshold = 16
//File size > 500KB
//Packet Size = 1KB
//Time out < 1 sec

int main(int argc, char **argv)
{
    struct sockaddr_in si_me, si_sender, si_receiver, si_other;
    int s, i, slen = sizeof(si_sender), recv_len = sizeof(si_receiver), o_len;
    char buf[2000], mes[2000], r_buf[2000];
    char IP[100];
    int port = atoi(argv[2]);
    float loss_rate = atof(argv[3]);
    strcpy(IP, argv[1]);
    srand(time(NULL));

    //create UDP
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == -1){
        printf("Socket!!!\n");
        return 0;
    }

    struct timeval time_out;
    time_out.tv_sec = 0;
    time_out.tv_usec = 1000;
    //setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out, sizeof(time_out));


    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = inet_addr(argv[1]);
    //si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    //int ch = inet_pton(AF_INET, SERVER_ADDRESS, &(si_me.sin_addr));

    //bind
    if((bind(s, (struct sockaddr*)&si_me, sizeof(si_me))) < 0){
        printf("bind!!!\n");
        return 0;
    }

    //check connection
    while(recvfrom(s, buf, 2000, 0, (struct sockaddr *)&si_sender, &slen) < 0)
        continue;
    if(strcmp(buf, "sender") != 0) {
        memcpy(&si_receiver, &si_sender, sizeof(si_sender));
        while(recvfrom(s, buf, 2000, 0, (struct sockaddr *)&si_sender, &recv_len) < 0)
            continue;
    }
    else{
        memset(buf, '\0', sizeof(buf));
        while(recvfrom(s, buf, 2000, 0, (struct sockaddr *)&si_receiver, &slen) < 0)
            continue;
    }
    //printf("%s", buf);
    sendto(s, "sender go!\n", 20, 0, (struct sockaddr *)&si_sender, slen);
    sendto(s, "receiver go!\n", 20, 0, (struct sockaddr *)&si_receiver, recv_len);

    int tran = 0;
    int loss = 0;

    while(1){
        memset(buf, '\0', sizeof(buf));
        //memset(r_buf, '\0', sizeof(r_buf));
        recvfrom(s, buf, 2000, 0, (struct sockaddr *)&si_other, &o_len);
        char who[10];
        sscanf(buf, "%s %*s", who);
        if(strcmp(who, "r") == 0) {
            int count;
            sscanf(buf, "%*s %d %*s", &count);
            char m[200];
            strcpy(m, buf);
            sendto(s, m, strlen(m)+1, 0, (struct sockaddr *)&si_sender, slen);
            if(count == -1) {
                printf("get finack\nfwd finack\n");
                break;
            }
            else {
                printf("get ack #%d\n", count);
                printf("fwd ack #%d\n", count);
            }
        }
        else if(strcmp(who, "s") == 0) {
        
            int count;
            sscanf(buf, "%*s %d %*s", &count);
            char m[2000];
            memset(m, '\0', sizeof(m));
            strcpy(m, buf);
            if(count == -1) {
                printf("get fin\n");
                printf("fwd fin\n");
                sendto(s, m, strlen(m)+1, 0, (struct sockaddr *)&si_receiver, recv_len);
                continue;
            }
            printf("get data #%d\n", count);
        
            if((rand() % 100) > (loss_rate * 100)) {
                sendto(s, m, strlen(m)+1, 0, (struct sockaddr *)&si_receiver, recv_len);
                tran += 1;
                float rate =(float)((float)loss / ((float)tran+(float)loss));
                printf("fwd data #%d, loss rate = %f\n", count, rate);
            }
            else {
                loss += 1 ;
                float rate = ((float)loss / ((float)tran+(float)loss));
                printf("drop data #%d, loss rate = %f\n", count, rate);
            }
        }
    }

    //close(s);

    return 0;
}

