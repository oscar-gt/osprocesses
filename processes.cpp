/*
 * processes.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Oscar Garcia-Telles
 */

// Assignment simulates the unix command line
// execution of
//
// ps -A | grep ttys | wc -l
//
// where:
// Main process forks and creates a child.
// ----Main process waits for child.
//
// Child process forks and creates grandchild.
// ---- Child process waits for grandchild.
//
// Grandchild forks and creates great grandchild
// ---- Grandchild process waits for grandchild.
//
// great grand child process executes 	ps -A
//
// grand child process executes 		grep ttys
//
// child process executes 				wc -l
//

#include <iostream>		// Input/output
#include <unistd.h>		// fork() function
#include <sys/wait.h>	// wait() function when forking
#include <stdio.h>		// perror()
#include <strings.h>	// bzero() function for data buffer
#include <stdlib.h>  	// exit()

using namespace std;

// To be used with our buffers used with pipes
const int BUF_SIZE = 4096;

int main(int argc, char* argv[])
{
	// Checking for correct number of arguments.
	// Should be 2: processes <command>
	if(argc != 2)
	{
		cerr << "Two Argument Usage:  " << argv[0] << "  [command]" << endl;
		return -1;
	}

	// Correct argument input at this point, continuing...

	bool verbose = false;			// To debug with cout statements
	enum {READ, WRITE};				// For pipes
	pid_t pidA, pidB, pidC;			// For fork()

	int dr1, dr2, dr3, dr4;			// For dup2() calls

	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);

	// pipeA will take ps -A's output and make it grep [cmd]'s input
	// pipeB will take grep [cmd]'s output and make it wc -l's input
	int pipeA[2], pipeB[2];	// Our pipes

	// Variable command at the command line.
	const char* command = argv[1];

	// Setting up pipes
	if(pipe(pipeA) < 0)
	{
		perror("Error in creating pipeA");
		exit(EXIT_FAILURE);
	}
	if(pipe(pipeB) <0)
	{
		perror("Error in creating pipeB");
		exit(EXIT_FAILURE);
	}

	// Forking for the first time
	pidA = fork();
	if(pidA < 0)
	{
		perror ("Error during child fork.");
		exit(EXIT_FAILURE);
	}

	// Child process for wc -l
	if(pidA == 0)
	{
		// Forking for grandchild process
		pidB = fork();
		if(pidB < 0)
		{
			perror("Error during grandchild fork.");
			exit(EXIT_FAILURE);
		}

		// grandchild process for grep [command]
		if(pidB == 0)
		{
			// Forking for great-grandchild process
			pidC = fork();
			if(pidC < 0)
			{
				perror("Error during great-grandchild fork.");
				exit(EXIT_FAILURE);
			}

			// Great-grandchild process that executes ps -A.
			if(pidC == 0)
			{
				if(verbose)
				{
					cout << "This is the great-grandchild process for ps -A" << endl;
				}

				// Simulating ps -A

				// Redirecting output to WRITE end of pipe
				close(pipeA[READ]); 						// Closing unused ends
				close(pipeB[READ]);
				close(pipeB[WRITE]);
				dr1 = dup2(pipeA[WRITE], STDOUT_FILENO);	// Redirecting
				close(pipeA[WRITE]);						// Closing starting side
				if(dr1 == -1)
				{
					perror("dup2 in great grandchild failed");
					exit(EXIT_FAILURE);
				}

				// ps -A call
				if(execlp("/bin/ps", "ps", "-A", NULL) == -1)
				{
					perror("Failed to run execlp(\"/bin/ps\", \"ps\", \"-A\", NULL);");
					exit(EXIT_FAILURE);
				}
			}
			// Great-grandchild's parent, so global grandchild process
			// Grand child process executes grep ttys
			else
			{
				if(verbose)
				{
					cout << "This is the grandchild process for grep [cmd]" << endl;
				}

				// Redirecting stdin to READ end of pipeA
				close(pipeA[WRITE]);	// Closing unused ends
				close(pipeB[READ]);
				// Waiting for great grand child
				wait(NULL);
				dr2 = dup2(pipeA[READ], STDIN_FILENO);	// Redirecting
				if(dr2 == -1)
				{
					perror("dup2 for READ in grandchild failed");
					exit(EXIT_FAILURE);
				}

				// Redirecting output
				dr3 = dup2(pipeB[WRITE], STDOUT_FILENO);
				if(dr3 == -1)
				{
					perror("dup2 for WRITE grandchild failed");
					exit(EXIT_FAILURE);
				}

				// grep [command] call
				int x2 = 0;

				x2 = execlp("/bin/grep", "grep", command, NULL);
				cerr << "Failed to run execlp(\"/bin/grep\", \"grep\", command, NULL);" << endl;
				if(x2 == -1)
				{
					perror("Failed to run execlp(\"/bin/grep\", \"grep\", command, NULL);");
					exit(EXIT_FAILURE);
				}
			}

		}
		// Grandchild's parent, so it's the global child process that executes wc -l
		else
		{

			if(verbose)
			{
				cout << "This is the child process for wc -l" << endl;
			}


			// Redirecting stdin
			close(pipeB[WRITE]);	// Closing unused ends
			close(pipeA[READ]);
			close(pipeA[WRITE]);
			// Waiting for grandchild to finish
			wait(NULL);
			dr4 = dup2(pipeB[READ], STDIN_FILENO);	// Redirecting
			if(dr4 == -1)
			{
				perror("dup2 in child failed");
				exit(EXIT_FAILURE);
			}

			// Executing wc -l
			execlp("/usr/bin/wc", "wc", "-l", NULL);

			perror("Failed to run execlp(\"/bin/wc\", \"wc\", \"-l\", NULL);");
			exit(EXIT_FAILURE);

		}

	}
	// Parent of child
	else
	{
		close(pipeA[READ]);
		close(pipeA[WRITE]);
		close(pipeB[READ]);
		close(pipeB[WRITE]);


		// Waiting for child
		wait(NULL);
		//waitpid(pidA, &returnStatus, 0);


		if(verbose)
		{
			cout << "This is the global parent process, done" << endl;
		}

	}

	return 0;

}




