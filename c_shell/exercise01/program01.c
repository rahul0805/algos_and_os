#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signal_number) {
    // Write your code here.
	char choice=0;
	if(signal_number==SIGINT || signal_number==SIGTERM ){
	printf("\nInterrupt SIGINT received. \n");
	printf("Do you really want to quit (y|n)? \n");
	choice = getchar();
	if(choice=='y'){
		exit(0);
	}
	else if(choice=='n'){
		getchar();
		return;
	}
	else{
	}
	
	}
	
}

int main() {
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	printf("%d\n", getpid());
	while (1){
		// printf("you are here\n");
	}
}
