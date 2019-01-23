/* A simple client program to communicate with the corresponding
   server executable.
   The IP and TCP port are passed as arguments.

   Rensselaer Polytechnic Institute (RPI)
*/
////CLient gia na kanei receive minimata mono
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <signal.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    //signal(SIGALRM, sighandler);
    int sockfd, portno, n;
    double download;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct timespec end;


    char* buffer=malloc(32 * sizeof(char));
    char* time=malloc(32 * sizeof(char));
    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port \n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

///steile pios eisai
    printf("Please enter your name: ");
    bzero(buffer,32);
    fgets(buffer,31,stdin);
    //strcpy(buffer,"kostas");
    n = write(sockfd,buffer,strlen(buffer));///grapse apo ton buffer sto sockfd-server to onoma sou
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer,32);
    n = read(sockfd,buffer,31);///diabase apantisi apo server(Got your message)
    if (n < 0)
        error("ERROR reading from socket");
    //printf("%s\n",buffer); //print i got your message

//perimene na deis an sou exoun steilei minimata
    printf("\nChecking if you got mail... \n");
    bzero(buffer,32);
    strcpy(buffer,"receiver");
    n = write(sockfd,buffer,strlen(buffer));///grapse apo ton buffer sto sockfd oti eisai receiver
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer,32);
    n = read(sockfd,buffer,31);///diabase apantisi apo server sto buffer(Got your message) alla min tin ektiposeis
    if (n < 0)
        error("ERROR reading from socket");

    bzero(time, 32);
    n = read(sockfd,time,31);
    clock_gettime(CLOCK_MONOTONIC,&end);
    download = atol(time);//string to int
    if (n < 0)
        error("ERROR writing on socket.");
    printf("Download speed: %f ns\n", (double)(fabs(end.tv_sec +end.tv_nsec/1000000000 - download))); ///download speed

    do
    {

        bzero(buffer,32);
        n = read(sockfd,buffer,31);///diabase apantisi apo server sto buffer pou 8a einai to minima pou sou steilan
        if (n < 0)
            error("ERROR reading from socket");
        n = write(sockfd,"I got your message",18);
        if (n < 0) error("ERROR writing to socket");


        if (strcmp(buffer,"done")!=0)
        {
            printf("\nMessage: %s ",buffer);
        }



        if(strcmp(buffer,"You have no messages")==0)  ///an den exeis minimata
        {
            break;
        }
        else if(strcmp(buffer,"done")==0) ///an peires minimata alla den exeis alla
        {
            break;
        }
        else
        {
            bzero(buffer,32);
            n = read(sockfd,buffer,31);///diabase to onoma autou pou sou esteile minima
            if (n < 0)
                error("ERROR reading from socket");
            n = write(sockfd,"I got your message",18);
            if (n < 0) error("ERROR writing to socket");
            printf("from: %s\n",buffer);
        }


    }
    while(1);

    free(buffer);
    free(time);
    close(sockfd);
    return 0;
}
