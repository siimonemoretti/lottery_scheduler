/*
 * This program changes sched policy and then launches two child processes.
 * Childs will execute the dummy_program process.
 */

 #include <sched.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 #include <unistd.h>
 
 #ifndef SCHED_LOTTERY
 #include "linux/sched.h"
 #endif
 
 void start_dummy_with_lottery() {
	pid_t pid = fork();
	if (pid == 0) {
	  // In child: run dummy_program
	  execl("./dummy", "dummy", NULL);
	  perror("execl failed");
	  exit(EXIT_FAILURE);
	} else if (pid < 0) {
	  perror("fork failed");
	}
 }
 
 int main(int argc, char *argv[]) {
	if (argc != 2) {
	  fprintf(stderr, "Usage: %s <number_of_children>\n", argv[0]);
	  exit(EXIT_FAILURE);
	}
	int num_children = atoi(argv[1]);
	if (num_children <= 0) {
	  fprintf(stderr, "Number of children must be a positive integer.\n");
	  exit(EXIT_FAILURE);
	}
 
	struct sched_param param;
	int policy;
 
	// Display current scheduling policy before changing
	policy = sched_getscheduler(0);
 
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
 
	// Fork N child processes, based on argv[1]
	for (int i = 0; i < num_children; i++) {
	  start_dummy_with_lottery();
	}
 
	// Wait for child processes to finish
	// wait(NULL);
	// wait(NULL);
 
	return 0;
 }