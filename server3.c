/* A simple server in the internet domain numofthreadssing TCP
   The port number is passed as an argument

   Rensselaer Polytechnic Institute (RPI)
*/

#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#define max_users 4



///global metalbites wste na fainontai apo ola ta threads
pthread_t threads[max_users];
pthread_mutex_t mutex;
pthread_cond_t cond;

// A linked list node
struct Node
{

    char Senders[32];
    char Receivers[32];
    char Messages[32];

    struct Node *next;
};

void push(struct Node** head_ref, char* new_sender,char* new_receiver,char* new_message)
{
    /* 1. allocate node */
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));

    /* 2. put in the data  */
    strcpy(new_node->Senders,new_sender);
    strcpy(new_node->Receivers,new_receiver);
    strcpy(new_node->Messages,new_message);

    /* 3. Make next of new node as head */
    new_node->next = (*head_ref);

    /* 4. move the head to point to the new node */
    (*head_ref)    = new_node;
}


void error(char *msg)
{
    perror(msg);
    exit(1);
}


void deleteKey(struct Node **head_ref, char* key,int newsockfd)
{

    char buffer[32];

    int flag2=0; ///simaia gia to an kapios xristis exei adiabasta minimata( 0 == den exei minimata)
    int n;
    // Store head node
    struct Node* temp = *head_ref, *prev;

    // If head node itself holds the key to be deleted
    while (temp != NULL && (strcmp(temp->Receivers,key)==0))
    {

        n = write(newsockfd,temp->Messages,strlen(temp->Messages));
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(newsockfd,buffer,31);///diabase apantisi apo client(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s\n",buffer); //print i got your message

        n = write(newsockfd,temp->Senders,strlen(temp->Senders));
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(newsockfd,buffer,31);///diabase apantisi apo client(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s\n",buffer); //print i got your message

        flag2=1;
        *head_ref = temp->next;   // Changed head

        free(temp);               // free old head
        temp= *head_ref;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while(temp != NULL)
    {
        while (temp != NULL && strcmp(temp->Receivers,key)!=0)
        {
            prev = temp;
            temp = temp->next;
        }

        // If key was not present in linked list
        if (temp == NULL)
        {


            ///an den eixe ka8olou minimata
            if(flag2==0)
            {
                n = write(newsockfd,"You have no messages",20);
                if (n < 0) error("ERROR writing to socket");
                bzero(buffer,32);
                n = read(newsockfd,buffer,31);///diabase apantisi apo client(Got your message)
                if (n < 0)
                    error("ERROR reading from socket");


            }
            else  ///an pire kapoia minimata steile done oti teliosan
            {

                n = write(newsockfd,"done",4);
                if (n < 0) error("ERROR writing to socket");
                bzero(buffer,32);
                n = read(newsockfd,buffer,255);///diabase apantisi apo client(Got your message)
                if (n < 0)
                    error("ERROR reading from socket");

            }

            return;
        }

        n = write(newsockfd,temp->Messages,strlen(temp->Messages));
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(newsockfd,buffer,31);///diabase apantisi apo client(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s\n",buffer); //print i got your message

        n = write(newsockfd,temp->Senders,strlen(temp->Senders));
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(newsockfd,buffer,31);///diabase apantisi apo client(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s\n",buffer); ///print i got your message

        flag2=1;


        // Unlink the node from linked list
        prev->next = temp->next;

        free(temp);  // Free memory

        temp=prev->next;
    }

    if(flag2==0) //edw den 8a mpei pote
    {
        n = write(newsockfd,"You have no messages",20);
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(newsockfd,buffer,31);//diabase apantisi apo client(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
    }
    else  ///an pire kapoia minimata steile done oti teliosan
    {

        n = write(newsockfd,"done",4);
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,32);
        n = read(newsockfd,buffer,31);//diabase apantisi apo client(Got your message)
        if (n < 0)
            error("ERROR reading from socket");
    }
}

int  flag=0;///simaia gia to an egine sindesi me kapion client

// This function prints contents of linked list starting from
// the given node
void printList(struct Node *node)
{
    while (node != NULL)
    {
        printf("\n %s ", node->Senders);
        printf("\n %s ", node->Receivers);
        printf("\n %s ", node->Messages);
        node = node->next;
    }
}


void* newclient(void *ptr);
/* Start with the empty list */
struct Node* head = NULL;



int main(int argc, char *argv[])
{

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond, NULL);

    int sockfd, newsockfd, portno, clilen;
    int pid,i;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");



//	for (;;){



    for(i=0; i<max_users; i++)
    {
        pthread_create(&threads[i],NULL,newclient,(void*)sockfd);
        pthread_mutex_lock (&mutex);
        while(flag==0)
            pthread_cond_wait(&cond,&mutex);
        flag=0;
        pthread_mutex_unlock (&mutex);

    }

    for (i=0; i<max_users; i++)
    {
        pthread_join(threads[i],NULL);
    }

    printList(head);




    //}//telos for

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

void* newclient(void* sockfdptr)
{

    int sockfd=(int)sockfdptr;
    int newsockfd,clilen;
    char* answer=malloc(32 * sizeof(char));
    strcpy(answer,"no");
    char* buffer=malloc(32 * sizeof(char));
    char* time=malloc(32 * sizeof(char));
    char* timer=malloc(32 * sizeof(char));
    char* buffersender=malloc(32 * sizeof(char));
    char* bufferreceiver=malloc(32 * sizeof(char));
    char* buffermessage=malloc(32 * sizeof(char));
    struct sockaddr_in cli_addr;
    struct timespec end,start;
    int n;
    double upload;

    listen(sockfd,1);


    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    pthread_mutex_lock (&mutex);
    flag=1; ///global metabliti poy mas deinxei oti pragmatopii8ike sindesi.
    pthread_mutex_unlock (&mutex);
    pthread_cond_signal(&cond);

    do
    {



///diabazoume to onoma tou xrhsth poy stelnei minima h poy 8elei na labei minimata
        bzero(buffersender,32);
        n = read(newsockfd,buffersender,31);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffersender);
        n = write(newsockfd,"I got your message",18);
        if (n < 0) error("ERROR writing to socket");
///diabazoume to onoma tou xrhsth ston opion 8eloyme na steiloume minima h to string "receiver" me to opoio katalabainoume oti o parapanw xristis 8elei na labei minima(stelnetai automata apo to programma)
        bzero(bufferreceiver,32);
        n = read(newsockfd,bufferreceiver,31);
        if (n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n",bufferreceiver);
        n = write(newsockfd,"I got your message",18);
        if (n < 0) error("ERROR writing to socket");


        ///elenxos an to string einai "receiver" ara eimaste se receive mode
        if(strcmp(bufferreceiver,"receiver")==0) //receive mode
        {

            ///Download speed
            clock_gettime(CLOCK_MONOTONIC,&start);
            bzero(timer,32);
            sprintf(timer, "%f", (double)(start.tv_sec + start.tv_nsec/1000000000));//ns
            n = write(newsockfd, timer, 31);
            if (n < 0)
                error("ERROR writing on socket.");

            pthread_mutex_lock (&mutex);///mutex giati den einai thread-safe i sindedemeni lista
            deleteKey(&head,buffersender,newsockfd);
            pthread_mutex_unlock (&mutex);

            break;
        }
        else  ///send mode
        {

///Upload speed
            bzero(time, 32);
            n = read(newsockfd,time,31);
            clock_gettime(CLOCK_MONOTONIC,&end);
            upload = atol(time);//string to int
            if (n < 0)
                error("ERROR writing on socket.");

            printf("Upload speed: %.2f ns\n", (double)(fabs(end.tv_sec+ end.tv_nsec/1000000000 - upload))); //upload speed


            ///diabazoyme to minima pou 8elei na steilei o client efoson eimaste se send mode
            bzero(buffermessage,32);
            n = read(newsockfd,buffermessage,31);
            if (n < 0) error("ERROR reading from socket");
            printf("Here is the message: %s\n",buffermessage);
            n = write(newsockfd,"I got your message",18);
            if (n < 0) error("ERROR writing to socket");


///dimiourgoume enan kainourio kombo kai apo8ikeuoume oles tis parapanw plirofories
            pthread_mutex_lock (&mutex);
            push(&head,buffersender,bufferreceiver,buffermessage);
            pthread_mutex_unlock (&mutex);

            ///8elei na steilei kai allo minima?
            bzero(answer,32);
            n = read(newsockfd,answer,31);
            if (n < 0) error("ERROR reading from socket");
            n = write(newsockfd,"I got your message",18);
            if (n < 0) error("ERROR writing to socket");
        }
        printf("Client wants to send another message? : %s\n",answer);


    }
    while(strcmp(answer,"yes")==0);



    free(answer);
    free(timer);
    free(time);
    free(buffer);
    free(buffersender);
    free(bufferreceiver);
    free(buffermessage);
    close(newsockfd);

    pthread_exit(NULL);
}
