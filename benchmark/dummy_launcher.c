/*
 * This program changes sched policy and then launches two child processes.
 * Childs will execute the dummy_program process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
 
#ifndef SCHED_LOTTERY
#include "linux/sched.h"
#endif
 

void start_dummy_with_lottery() {
   pid_t pid = fork();
   if (pid == 0) {
	   // In child: run dummy_program
      printf("%d 's policy: %d\n", getpid(), sched_getscheduler(0));
	   execl("./dummy_program", "dummy_program", NULL);
	   perror("execl failed");
	   exit(EXIT_FAILURE);
   } else if (pid < 0) {
	   perror("fork failed");
   }
}

int main(void){
	struct sched_param param;
	int policy;
	
	#ifdef SCHED_LOTTERY
	printf("SCHED_LOTTERY is defined\n");
	#else
	printf("SCHED_LOTTERY is not defined\n");
	#endif
	
	// Display current scheduling policy before changing
	policy = sched_getscheduler(0);
	printf("Initial scheduling policy: %d\n", policy);
	
	// Set the scheduling priority
	param.sched_priority = 0;
	
	// Set the scheduling policy to LOTTERY
	int r = sched_setscheduler(0, SCHED_LOTTERY, &param);
	if (r == -1) {
	   perror("sched_setscheduler");
	   exit(EXIT_FAILURE);
	}
	
	// Get and print the new scheduling policy
	policy = sched_getscheduler(0);
	printf("New scheduling policy: %d\n", policy);

   // Fork two child processes
   start_dummy_with_lottery();
   start_dummy_with_lottery();
	// Wait for child processes to finish
   wait(NULL);
   wait(NULL);
   
   return 0;
}