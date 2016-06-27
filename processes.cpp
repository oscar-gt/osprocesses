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

using namespace std;

// To be used with our buffers used with pipes
const int BUF_SIZE = 4096;

int main(int argc, char* argv[])
{

	//test();
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

	int execute = 0;				// For execlp(...)

	int dr1, dr2, dr3, dr4;			// For dup2() calls

	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);

	// pipeA will take ps -A's output and make it grep [cmd]'s input
	// pipeB will take grep [cmd]'s output and make it wc -l's input
	int pipeA[2], pipeB[2];	// Our pipes

	// Variable command at the command line.
	const char* command = argv[1];

	if(verbose)
	{
		cout << "argvv[1] == " << command << endl;
	}


	// Setting up pipes
	if(pipe(pipeA) < 0)
	{
		perror("Error in creating pipeA");
		return -1;
	}
	if(pipe(pipeB) <0)
	{
		perror("Error in creating pipeB");
		return -1;
	}

	int returnStatus;	// To wait for offspring processes.

	// Forking for the first time
	pidA = fork();
	if(pidA < 0)
	{
		cerr << "Error during child fork." << endl;
		return -1;
	}

	// Child process for wc -l
	if(pidA == 0)
	{


		// Forking for grandchild process
		pidB = fork();
		if(pidB < 0)
		{
			cerr << "Error during grandchild fork." << endl;
			return -1;
		}

		// grandchild process for grep [command]
		if(pidB == 0)
		{


			// Forking for great-grandchild process
			pidC = fork();
			if(pidC < 0)
			{
				cerr << "Error during great-grandchild fork." << endl;
				return -1;
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
				close(pipeA[READ]); 						// Closing unused end
				dr1 = dup2(pipeA[WRITE], STDOUT_FILENO);	// Redirecting
				close(pipeA[WRITE]);						// Closing starting side
				if(dr1 == -1)
				{
					cerr << "dup2 in great grandchild failed" << endl;
				}

				cerr << "great grand child before execlp(\"/bin/ps\", \"ps\", \"-A\", NULL);" << endl;
				// ps -A call
				if(execlp("/bin/ps", "ps", "-A", NULL) == -1)
				{
					cerr << "Failed to run execlp(\"/bin/ps\", \"ps\", \"-A\", NULL);" << endl;
					return -1;
				}
			}
			// Great-grandchild's parent, so global grandchild process
			// Grand child process executes grep ttys
			// ************ NOTE: this grand child process never finishes,
			// ************		  since it's probably hung on the execlp(grep, grep, tty, null) call.
			else
			{
				//wait(NULL);
				//waitpid(pidC, &returnStatus, 0);
				if(verbose)
				{
					cout << "This is the grandchild process for grep [cmd]" << endl;
				}

				// Redirecting stdin to READ end of pipeA
				close(pipeA[WRITE]);					// Closing unused end
				dr2 = dup2(pipeA[READ], STDIN_FILENO);	// Redirecting
				close(pipeA[READ]);						// Closing starting side
				if(dr2 == -1)
				{
					cerr << "dup2 for READ in grandchild failed" << endl;
				}
				// close(pipeA[READ]);

				// Redirecting output
				close(pipeB[READ]);							// Closing unused end
				dr3 = dup2(pipeB[WRITE], STDOUT_FILENO);	// Redirecting
				close(pipeB[WRITE]);						// Closing starting end
				if(dr3 == -1)
				{
					cerr << "dup2 for WRITE grandchild failed" << endl;
				}

				// grep [command] call
				//cerr << "grand child" << endl;
				int x2 = 100;
				// Waiting for great grand child
				//waitpid(pidC, &returnStatus, 0);

				cerr << "grand child before 	 execlp(\"/bin/ls\", \"ls\", \"-l\", NULL);" << endl;

				// NOTE: call to execlp should be
				// execlp("/bin/grep", "grep", "ttys", NULL);,
				// but I'm trying simpler calls like ls -l

				x2 = execlp("/bin/grep", "grep", command, NULL);
				cerr << "Failed to run execlp(\"/bin/grep\", \"grep\", command, NULL);" << endl;
				if(x2 == -1)
				{
					cerr << "Failed to run execlp(\"/bin/grep\", \"grep\", command, NULL);" << endl;
					return -1;
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
			close(pipeB[WRITE]);					// Closing unused end
			dr4 = dup2(pipeB[READ], STDIN_FILENO);	// Redirecting
			close(pipeB[READ]);						// Closing starting side
			if(dr4 == -1)
			{
				cerr << "dup2 in child failed" << endl;
			}
			close(pipeB[READ]);
			//close(pipeB[WRITE]);

			cerr << "dup2(pipeB[READ], STDIN_FILENO) == " << dr4 << endl;

			cerr << "child before waitpid()" << endl;
			//Waiting for grandchild to finish
			//waitpid(pidB, &returnStatus, 0);

			cerr << "child before 	         execlp(\"/bin/wc\", \"wc\", \"-l\", NULL);" << endl;
			if(execlp("/bin/wc", "wc", "-l", NULL) == -1)
			{
				cerr << "Failed to run execlp(\"/bin/wc\", \"wc\", \"-l\", NULL);" << endl;
				return -1;
			}
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
		waitpid(pidA, &returnStatus, 0);

		cerr << "Hi from parent process" << endl;
		if(verbose)
		{
			cout << "This is the global parent process, done" << endl;
		}

	}

	return 0;

}




