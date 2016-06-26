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

using namespace std;


// ***********		Preliminary notes:		***********
// Using execlp(...) from the notes
// execlp("/bin/ls", "ls", "-l", NULL);

// To be used with our buffers used with pipes
const int BUF_SIZE = 4096;

int main(int argc, char* argv[])
{
	bool verbose = true;			// To debug with cout statements
	enum {READ, WRITE};				// For pipes
	pid_t pidA, pidB, pidC, ptid;	// For fork()
	int childStatus;

	// pipeA will take ps -A's output and make it grep [cmd]'s input
	// pipeB will take grep [cmd]'s output and make it wc -l's input
	int pipeA[2], pipeB[2];	// Our pipes

	// Checking for correct number of arguments.
	// Should be 2: processes <command>
	if(argc != 2)
	{
		cerr << "Two Argument Usage:  " << argv[0] << "  [command]" << endl;
		return -1;
	}

	// Correct argument input at this point, continuing...
	string command = argv[1];
	if(verbose)
	{
		cout << "argv[1] == " << command << endl;
	}

	// Testing execlp(...)
	// execlp("/bin/ls", "ls", "-l", NULL);

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
				// Simulating ps -A
				cout << "This is the great-grandchild process for ps -A" << endl;
			}
			// Great-grandchild's parent, so global grandchild process
			else
			{
				wait(NULL);
				cout << "This is the grandchild process for grep [cmd]" << endl;
			}

		}
		// Grandchild's parent, so it's the global child process
		else
		{
			wait(NULL);
			cout << "This is the child process for wc -l" << endl;
		}

	}
	// Parent of child
	else
	{
		wait(NULL);
		cout << "This is the global parent process" << endl;
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




