/***********************************************************************
name:
	assignment4 -- acts as a pipe using ":" to seperate programs.
description:	
	See CS 360 Processes and Exec/Pipes lecture for helpful tips.
***********************************************************************/

/* Includes and definitions */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

/**********************************************************************
./assignment4 <arg1> : <arg2>

    Where: <arg1> and <arg2> are optional parameters that specify the programs
    to be run. If <arg1> is specified but <arg2> is not, then <arg1> should be
    run as though there was not a colon. Same for if <arg2> is specified but
    <arg1> is not.
**********************************************************************/

int main(int argc, char *argv[]){
	int fd[2];
	int l = 0;
	int r = 0;
	int n = 1;
	char *left_args[argc];
	char *right_args[argc];
	int pid;
	pipe(fd); //The grand pipe

	//Check if argument is passed
	if(argc < 2) {
		printf("%s\n", strerror(errno));
		exit(0);
	}

	//Count # of arguments for left side.
	while((strcmp(argv[n], ":")) != 0) {
		n++;
		if(n >= argc) { break; }
	}

	int countLeft = n;
	int countRight = n + 1;

	//Copy left-side args into left_args(array)
	n = 1;
	while(n < countLeft) {
		left_args[l] = argv[n];
		n++;
		l++;
	}
	left_args[l] = NULL;

	//Copy right-side args into right_args(array)
	n = countRight;
	while(n < argc) {
		right_args[r] = argv[n];
		n++;
		r++;
	}
	right_args[r] = NULL;

	/* 
	 * Error checking here:
	 * Checking if args on left and right are present
	 * If none, then execute the opposite side.
	 */
	if(right_args[0] == NULL) { 
		execvp(left_args[0], left_args);
		printf("%s\n", strerror(errno));
		exit(1);
	}
	else if (left_args[0] == NULL) { 
		execvp(right_args[0], right_args);
		printf("%s\n", strerror(errno));
		exit(1);
	}

	//Create process via mitosis(fork)
	if((pid = fork()) == 0) { //Exec parent
		close(fd[0]); //Close parent 

		//Check if there's an error in creating a copy
		if(dup2(fd[1], 1) == -1) { 
			printf("%s\n", strerror(errno));
			exit(1);
		}
		close(fd[1]); //Close child
		execvp(left_args[0], left_args);
		printf("%s\n", strerror(errno));
		exit(1);
	}
	else { //Exec child 
		close(fd[1]); //Close child
		if(dup2(fd[0],0)) {
			printf("%s\n", strerror(errno));
			exit(1);
		}
		close(fd[0]);
		execvp(right_args[0], right_args);
		printf("%s\n", strerror(errno));
		exit(1);
	}
	return 0;
}
