#include <stdio.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#define max_users 13000

///prosoxi sto ip analoga an xrisimopioumai zsun h qemu. Episis na mpoyn se sxolia ta fgets kai na bgoyn apo sxolia ta strcpy sta programmata twn client.

int main(){

int i;


	for(i=0;i<max_users;i++){
		system("./client 192.168.1.1 2223");
		printf("%d\n",i);
	}



return 0;
}
