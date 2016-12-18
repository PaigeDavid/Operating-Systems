/*
Homework 2
CSCI 340
Created By: Paige Peck
Date: 2-1-16
*/


// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "shell.h"

// -----------------------------------
// Main function 
// Arguments:	argc = number of arguments suppled by user
//				argv = array of argument values
//
//
int main(int argc, char** argv) {

	// ----------------------------------
	// Pseudocode
	// ----------------------------------
	// while (!done)
	//		print the prompt (i.e. >, #, or $ ... your choice)
	//		read command line
	//		parse the command line
        //      if (parsed command is "exit")
        //      	done = true
	//  	else if (is _builtin(...))
	//			do_builtin(...)
	//		else if (find_fullpath(...))
	//  		execute(...)
	//		else
	//			error()
	// 		cleanup(...)
	//
	char input[100];
	char buffer[100];
	int done = FALSE;
	int j;			//Counter for for loop
	//char newInput[100];
	//int compare;
	command_t* t;
	t = (command_t*)malloc(sizeof(command_t));	
	//for loop to determine length of p_cmd.name
	
	 	
	while(!done)
	{
		printf(">> ");
		fgets( input, 100, stdin);
		for(j=0; input[j] != '\0'; j++) {}
		t->name = (char*) malloc(j+1*sizeof(char*));
		t->argv = (char**) malloc(3*sizeof(char*));
		if(input[j-1] == '\n')
		{
			input[j-1] = '\0';
		}
		parse(input, t);
		if((*t).name[0] == 'e' && (*t).name[1] == 'x' && (*t).name[2] == 'i' && (*t).name[3] == 't')
		{
			done = TRUE;
		}
		else if(is_builtin(t))
		{
			do_builtin(t);
			//printf("Is Builtin successful\n");
		}
		else if(find_fullpath(buffer, t))
		{
			execute(t);
			//printf("Find fullpath successful\n");
		}
		else
		{
			printf("An error occurred\n");
		}
		cleanup(t);
	}

	free(t);

	printf("CSCI 340 Homework Assignment 2 - Have Fun!\n");

	return 0;

} // end main function
