// Edited By: Paige Peck
// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included
#include <stdio.h>	// IO functions
#include <stdlib.h> 	// atof function
#include <math.h>	// math functions
#include "utils.h"	// your own functions

// ----------------------------------------------
// Define constants
#define TOTAL_VECS 10	// constant that defines the total number 
			// of vectors, i.e. magnitude,direction 
			// pairs, in the vectors text file.

// -----------------------------------
// Main function 
// Arguments:	argc = number of arguments suppled by user
//		argv = array of argument values
//
//
int main( int argc, char** argv ) {
	
	// --------------------------------
	// Steps to be performed
	//	1. Define variables (including arrays)
	//	2. Using read() function in utils.h, read vectors defined in text file 
	//		- the location of the text file (i.e. vectors.txt) must be defined in argv[1]
	//  	3. Loop through array of vector structs and print to console:
	//		- vector magnitude (using only 2 decimal places)
	//		- vector direction (in radians using only 2 decimal places)
	//		- vector direction (in degrees using only 2 decimal places)
	//		- Using compute_x() function in utils.h, x component value (using only 2 decimal places)
	//		- Using compute_y() function in utils.h, y component value (using only 2 decimal places)
	//	    in the following format
	//		r = <value>, theta = <value> degrees, <value> radians, x_comp = <value>, y_comp = <value>
	//	    For example:
	//		r = 10.00, theta = 180.00 degrees, theta = 3.14 radians, x_comp = -10.00, y_comp = 0.00 
	//
	//char* fileName = "vectors.txt";
	v_struct* p;			//Pointer to v_struct
	int totalVec;			//Variable for storing total vectors grabbed from file
	double x_com, y_com, radians;	//Variables for computing
	int i = 0;			//Counter
	p = (v_struct*)malloc(10*sizeof(v_struct));
	
	if(argc < 2) //If user doesn't input file name, end program after printing they need more input
	{
		printf("Not enough input, must include a text file.\n");
	}
	else
	{
		totalVec = read(argv[1], p);

		while(i < totalVec)
		{
			x_com = x_component(p+i);
			y_com = y_component(p+i);
			radians = p[i].theta * M_PI / 180;
			
			printf("r = %.2f, theta = %.2f degrees, theta = %.2f radians, x_comp = %.2f, y_comp = %.2f\n", p[i].r, p[i].theta, radians, x_com, y_com);
			i++;
		}

		//printf("\nTotal vectors: %d", totalVec);
		//printf("\nx: %.2f", x_com);
		//printf("\ny: %.2f", y_com);
	}

	printf(" Homework Assignment 1 - Have fun!\n" );

	free(p);
	
	return 0;

} // end main() function
