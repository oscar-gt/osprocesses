/*
 * processes.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Oscar Garcia-Telles
 */

// Assignment simulates the command line
// execution of
// ps -A | grep [command] | wc -l

#include <iostream>		// Input/output
#include <unistd.h>		// fork() function
#include <sys/wait.h>	// wait() function when forking
#include <stdio.h>		// perror()
#include <strings.h>	// bzero() function for data buffer

using namespace std;


// ***********		Preliminary notes:		***********
// Using execlp(...) from the notes
// execlp("/bin/ls", "ls", "-l", NULL);

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

	bool verbose = true;			// To debug with cout statements
	enum {READ, WRITE};				// For pipes
	pid_t pidA, pidB, pidC, ptid;	// For fork()
	int childStatus;

	int execute = 0;				// For execlp(...)

	char buf[BUF_SIZE];
	bzero(buf, BUF_SIZE);

	// pipeA will take ps -A's output and make it grep [cmd]'s input
	// pipeB will take grep [cmd]'s output and make it wc -l's input
	int pipeA[2], pipeB[2];	// Our pipes

	// Variable command at the command line.
	const char* command = argv[1];

	if(verbose)
	{
		cout << "argv[1] == " << command << endl;
	}

	// Testing execlp(...)
	// execlp("/bin/ls", "ls", "-l", NULL);

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

			// Great-grandchild process for ps -A
			if(pidC == 0)
			{
				if(verbose)
				{
					cout << "This is the great-grandchild process for ps -A" << endl;
				}
				// Simulating ps -A

				// Writing
				close(pipeA[READ]);	// Disabling reading

				// stdout is now great-grandchild's write pipe
				dup2(pipeA[WRITE], STDOUT_FILENO);
				close(pipeA[WRITE]);

				// ps -A call
				execute = execlp("/bin/ps", "ps", "-A", NULL);
				if(execute == -1)
				{
					cerr << "Failed to run execlp(\"/bin/ps\", \"ps\", \"-A\", NULL);" << endl;
					return -1;
				}

			}
			// Great-grandchild's parent, so global grandchild process
			else
			{
				wait(NULL);
				if(verbose)
				{
					cout << "This is the grandchild process for grep [cmd]" << endl;
				}

				// Closing write
				close(pipeA[WRITE]);

				// Redirecting stdin
				dup2(pipeA[READ], STDIN_FILENO);
				close(pipeA[READ]);


				// Reading into char buffer, not sure if necessary
//				char buf[BUF_SIZE];
//				int n = read(pipeA[READ], buf, BUF_SIZE);
//				buf[n] = '\0';
//				cout << buf;

				// Piping for writing
				close(pipeB[READ]);

				// stdout is now grandchild's write pipe
				dup2(pipeB[WRITE], STDOUT_FILENO);

				// Closing write
				close(pipeB[WRITE]);

				// grep [command] call
				execute = execlp("/bin/grep", "grep", command, NULL);
				if(execute == -1)
				{
					cerr << "Failed to run execlp(\"/bin/grep\", \"grep\", command, NULL);" << endl;
					return -1;
				}
			}

		}
		// Grandchild's parent, so it's the global child process
		else
		{
			wait(NULL);
			if(verbose)
			{
				cout << "This is the child process for wc -l" << endl;
			}

			// Redirecting stdin
			close(pipeB[WRITE]);
			dup2(pipeB[READ], STDIN_FILENO);
			close(pipeB[READ]);

			execute = execlp("/bin/wc", "wc", "-l", NULL);
			if(execute == -1)
			{
				cerr << "Failed to run execlp(\"/bin/wc\", \"wc\", \"-l\", NULL);" << endl;
				return -1;
			}

		}

	}
	// Parent of child
	else
	{
		wait(NULL);
		if(verbose)
		{
			cout << "This is the global parent process, done" << endl;
		}

	}

	return 0;




	// ***********	Simulating the following command line input:	****************
	// 						ps -A | grep [command] | wc -l
	// where:
	// child process executes 				wc -l
	//
	// grand child process executes 		grep [command]

	// great grand child process executes 	ps -A
}




