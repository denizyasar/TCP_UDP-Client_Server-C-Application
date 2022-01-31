#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock.h>

#define SRV_IP "127.0.0.1"
#define BUFLEN 1024
#define NPACK 10
#define PORT 8881

void die(char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

int main( int argc, char *argv[] )
{

    struct sockaddr_in si_other;
    int s,slen = sizeof(si_other);
    char buf[BUFLEN];

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    err = WSAStartup(wVersionRequested, &wsaData);

    memset((char *) &si_other, 0, sizeof(si_other));
    memset(buf, 0, sizeof buf);

    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr(SRV_IP);


    switch (argc)
    {
    case 3:
        if (strcmp(argv[2],"-u")==0) //UDP connection
        {
            if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
                die("UDP socket failed");

            printf("Using protocol UDP\n");
            sprintf(buf, argv[1]);

            if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, slen) == -1)
                die("UDP sendto() failed");

            if (recvfrom(s, buf, BUFLEN, 0, NULL,NULL) == -1)
                die("UDP recvfrom() failed");

            printf(buf);
        }
        break;
    case 2: //TCP Connection
        printf("Using default protocol TCP. -u to use UDP \n");

        if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
            die("TCP socket failed");

        if (connect(s, (struct sockaddr *) &si_other,sizeof(si_other)) < 0)
            die("TCP connect() failed");

        printf("Sending packet using TCP\n");
        sprintf(buf, argv[1]);

        if (send(s, buf, BUFLEN, 0) == -1)
            die("TCP send failed");

        if (recv(s, buf, BUFLEN, 0) == -1)
            die("TCP recv failed");

        printf(buf);
        break;
    case 1:
        printf("Use an integer to choose a poem.\n0 for random poem.\n-u to use UDP. Default is TCP.\n");
        break;
    }

    close(s);
    WSACleanup();

    return EXIT_SUCCESS;
}
