/*
 * processes.cpp
 *
 *  Created on: Jun 24, 2016
 *      Author: Oscar Garcia-Telles
 */

#include <iostream>
using namespace std;


//
int main(int argc, char* argv[])
{
	bool verbose = true;

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

	// ***********	Simulating the following command line input:	****************
	// 						ps -A | grep [command] | wc -l
	// where:
	// child process executes 				wc -l
	//
	// grand child process executes 		grep [command]
	// great grand child process executes 	ps -A
}




