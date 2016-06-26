/*
 * processes.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Oscar Garcia-Telles
 */

// Assignment simulates the command line
// execution of
// ps -A | grep [command] | wc -l

#include <iostream>		// For input/output
#include <unistd.h>

using namespace std;


// ***********		Preliminary notes:		***********
// Using execlp(...) from the notes
// execlp("/bin/ls", "ls", "-l", NULL);
//
int main(int argc, char* argv[])
{
	bool verbose = true;	// To debug with cout statements
	enum {READ, WRITE};		// For pipes
	pid_t pid;				// For fork()

	// pipeA
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

	// Testing the use of execlp(...)
	execlp("/bin/ls", "ls", "-l", NULL);

	// ***********	Simulating the following command line input:	****************
	// 						ps -A | grep [command] | wc -l
	// where:
	// child process executes 				wc -l
	//
	// grand child process executes 		grep [command]
	// great grand child process executes 	ps -A
}




