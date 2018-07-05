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
//Time out < 1 sec => 0.3sec

int main(int argc, char **argv)
{
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    char buf[2000], mes[4000];
    char IP[100];
    int port = atoi(argv[2]);
    strcpy(IP, argv[1]);
    time_t timer;

    //create UDP
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 300000;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    if(s == -1){
        printf("Socket!!!\n");
        return 0;
    }

    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = inet_addr(argv[1]);

    strcpy(mes, "sender");
    //listen
    sendto(s, mes, strlen(mes), 0, (struct sockaddr *)&si_me, slen);
    char star[20];
    while(recvfrom(s, star, 20, 0, (struct sockaddr *)&si_me, &slen) < 0)
        continue;
    printf("%s", star);

    FILE *fp = fopen(argv[3], "r");
    int winSize = 1;
    int count = 1, threshold = 16;
    int had = 0;

    //if(fread(buf, 1, sizeof(buf), fp) == -1)
    //    printf("!!!!!!!!!!!");

    while(1){
        int start = count, end = count + winSize;
        //printf("s : %d e : %d c : %d\n", start, end, count);
        int c = 0;
        for(int i = 0; i < winSize; i++) {
            int bytes;
            memset(mes, '\0', sizeof(mes));
            memset(buf, '\0', sizeof(buf));
            if(fread(buf, 1, 1024, fp) == 0)
                break;
            c++;
            sprintf(mes, "s %d %s", start+i, buf);
            //strcat(mes, buf);
            sendto(s, mes, strlen(mes), 0, (struct sockaddr *)&si_me, slen);
            if(start+i > had){
                printf("send data #%d, winSize = %d\n", start+i, winSize);
                had++;
            }
            else {
                printf("rsnd data #%d, winSize = %d\n", start+i, winSize);
            }
            //start++;
        }

        int sum = 0, timeout = 0;
        int *check = malloc(sizeof(int)*c);
        for(int k = 0; k < c; k++)
            check[k] = 0;

        for(int i = 0; i < c; i++) {
            if(recvfrom(s, mes, 2000, 0, (struct sockaddr *)&si_me, &slen) < 0){
                //printf("!!!%d\n", time(NULL));
                timeout = 1;
                break;
            }
            int ack;
            sscanf(mes, "%*s %d %*s", &ack);
            check[ack-start] = 1;
            printf("recv ack #%d\n", ack);
            sum++;
        }
        int stop = 0;
        if(timeout == 1) {
            threshold = winSize / 2;
            if(threshold < 1)
                threshold = 1;
            winSize = 1;
            //count = start + sum;
            printf("time out, threshold = %d\n", threshold);
            for(stop = 0; stop < c; stop++) {
                if(check[stop] == 0)
                    break;
            }
            fseek(fp, -1024*(c-stop), SEEK_CUR);
            count = start + stop;
            continue;
        }
        else if(feof(fp))
            break;
        else if(winSize > threshold) 
            winSize ++;
        else
            winSize *= 2;
        count = start + sum;
    }
    
    sendto(s, "s -1 fin", 10, 0, (struct sockaddr *)&si_me, slen);
    printf("send fin\n");
    while(recvfrom(s, buf, 200, 0, (struct sockaddr *)&si_me, &slen) < 0)
        continue;
    //printf("%s ", buf);
    printf("recv finack\n");
    

    fclose(fp);

    return 0;
}

