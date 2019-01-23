/* A simple client program to communicate with the corresponding
   server executable.
   The IP and TCP port are passed as arguments.

   Rensselaer Polytechnic Institute (RPI)
*/
////CLient gia na stelnei minimata mono
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

struct timespec start;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    char* answer=malloc(32 * sizeof(char));

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char* buffer=malloc(32 * sizeof(char));
    char* timer=malloc(32 * sizeof(char));
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


///steile to onoma sou(dld pios stelnei minima)

    do
    {


        printf("Please enter your name: ");
        bzero(buffer,32);
        fgets(buffer,31,stdin);
        //strcpy(buffer,"aris");
        n = write(sockfd,buffer,strlen(buffer));///grapse apo ton buffer sto sockfd-server to onoma sou
        if (n < 0)
            error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(sockfd,buffer,31);///diabase apantisi apo server(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
        //printf("%s\n",buffer); //print i got your message


///steile to onoma toy client(dld se pion stelneis minima)

        printf("Please enter the name of the client you want to send the message to: ");
        bzero(buffer,32);
        fgets(buffer,31,stdin);
        //strcpy(buffer,"kostas");
        n = write(sockfd,buffer,strlen(buffer));//grapse apo ton buffer sto sockfd-server to onoma tou client
        if (n < 0)
            error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(sockfd,buffer,31);//diabase apantisi apo server(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
// printf("%s\n",buffer); //print i got your message

///Upload speed
        clock_gettime(CLOCK_MONOTONIC,&start);
        bzero(timer,32);
        sprintf(timer, "%f", (double)(start.tv_sec + start.tv_nsec/1000000000));
        n = write(sockfd, timer, 31);
        if (n < 0)
            error("ERROR writing on socket.");

///steile to minima pou 8es
        printf("Please enter the message: ");
        bzero(buffer,32);
        fgets(buffer,31,stdin);
        //strcpy(buffer,"hi");
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(sockfd,buffer,31);//diabase apantisi apo server(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
        //printf("%s\n",buffer);

        printf("Do you want so send another message?(yes,no)\n");
        bzero(answer,32);
        fgets(answer,31,stdin);
        //strcpy(answer,"no");
        answer[strcspn(answer, "\n")] = 0;// gia na bgaloume to \n apo to telos

        if(strcmp(answer,"yes")==0 || strcmp(answer,"no")==0)
        {
            n = write(sockfd,answer,strlen(answer));
            if (n < 0)
                error("ERROR writing to socket");
            bzero(buffer,32);
            n = read(sockfd,buffer,31);//diabase apantisi apo server(Got your message)
            if (n < 0)
                error("ERROR reading from socket");
            // printf("%s\n",buffer); //print i got your message
        }
        else
        {
            printf("wrong input\n");
            break;
        }

    }
    while(strcmp(answer,"yes")==0);


    free(answer);
    free(timer);
    free(buffer);
    close(sockfd);


    return 0;
}
