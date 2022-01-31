#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BUFLEN 1024 //Max length of buffer
#define PORT 8881   //The port on which to listen for incoming data

static const int MAXPENDING=10;

void die(char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

char* fileRead(char* num)
{
    char* ch;
    FILE *fptr;
    ch = (char*)malloc(BUFLEN);
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int max=0;
     memset(ch, 0, BUFLEN);


    //open file
    fptr = fopen("poems.txt", "r");
    if (fptr == NULL)
        die("fopen() failed");
    //Find max number of poems
    while ((read = getline(&line, &len, fptr)) != -1)
    {
        if((atoi(line))>max)
            max=atoi(line);
    }
       rewind(fptr);
    int val = atoi(num);
    if (val>max)
    {

        strcpy(ch,"Maximum number of poems exceed \n");
        fclose(fptr);
        return(ch);
    }
    if(val==0)
        val = rand() % (max+1);


    //find start number and end number=start+1
    //if start zero randomize

    char start=val+'0';
    val=val+1;
    char end=val+'0';

    int c;
    int j=0;

    //move over file to find start and take it to buffer till end
    while (c != EOF)
        if ((c = getc(fptr)) == start)
            while ((c = getc(fptr)) != end && c != EOF)
            {
                ch[j]=c;
                j++;
            }


    fclose(fptr);

    return(ch);
}

int main(void)
{

    struct sockaddr_in si_udp, si_tcp,si_remote;
    int sockudp,socktcp,sock_remote,activity;
    int recv_len;
    socklen_t slen= sizeof(si_remote);
    char buf[BUFLEN];

    //set of socket descriptors
    fd_set readfds;

    if ( (sockudp=socket(AF_INET,SOCK_DGRAM, IPPROTO_UDP)) == -1 || (socktcp=socket(AF_INET,SOCK_STREAM, IPPROTO_TCP)) == -1)
        die("socket");

    // zero out the structure
    memset((char *) &si_udp, 0, sizeof(struct sockaddr_in));
    memset((char *) &si_tcp, 0, sizeof(struct sockaddr_in));
    memset((char *) &si_remote, 0, sizeof(struct sockaddr_in));
    memset(buf, 0, sizeof buf);

    si_udp.sin_family = AF_INET;
    si_udp.sin_port = htons(PORT);
    si_udp.sin_addr.s_addr = htonl(INADDR_ANY);

    si_tcp.sin_family = AF_INET;
    si_tcp.sin_port = htons(PORT);
    si_tcp.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(sockudp, (struct sockaddr *) &si_udp, sizeof(si_udp) ) == -1 )
        die("bind UDP");

    if( bind(socktcp, (struct sockaddr *) &si_tcp, sizeof(si_tcp))==-1)
        die("bind TCP");

    //listen tcp port
    if (listen(socktcp,MAXPENDING) < 0)
        die("listen() failed");

    printf("-=- Welcome to Orhan Veli Poems Server -=-\n");
    printf("OVP Server running on TCP/UDP\nPort: %d\nListening...\n",PORT);

    while(1)
    {
        fflush(stdout);
//clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(sockudp, &readfds);
        FD_SET(socktcp, &readfds);

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( sizeof(readfds)*8, &readfds, NULL, NULL, NULL);
        if (activity == -1||activity==0)
            die("select error");

        //Handle TCP connection
        if (FD_ISSET(socktcp, &readfds))
        {

            if ((sock_remote = accept(socktcp, (struct sockaddr *) &si_remote, &slen )) == -1)
                die("accept() failed");

            if ((recv_len = recv(sock_remote, buf, BUFLEN, 0)) == -1)
                die("TCP receive() failed");

            //print details of the client/peer and the data received
            printf("Received packet from TCP %s:%d\n", inet_ntoa(si_remote.sin_addr), ntohs(si_remote.sin_port));
            printf("Data: %s\n", buf);


            if (send(sock_remote,fileRead(buf), BUFLEN,0) == -1)
                die("TCP send() failed");

        }

        // Wait for a client to connect UDP
        if (FD_ISSET( sockudp, &readfds))
        {
            //try to receive some data, this is a blocking call UDP
            if ((recv_len = recvfrom(sockudp, buf, BUFLEN, 0, (struct sockaddr *) &si_remote, &slen)) == -1)
                die("recvfrom()");


            //print details of the client/peer and the data received
            printf("Received packet from UDP %s:%d\n", inet_ntoa(si_remote.sin_addr), ntohs(si_remote.sin_port));
            printf("Data: %s\n", buf);

            //reply
            if (sendto(sockudp, fileRead(buf), BUFLEN, 0, (struct sockaddr*) &si_remote, slen) == -1)
                die("UDP sendto() failed");


        }
        printf("Answer sent! \n");

    }

    close(sockudp);
    close(socktcp);

    return EXIT_SUCCESS;
}
