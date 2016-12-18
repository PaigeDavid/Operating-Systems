// -----------------------------------
// CSCI 340 - Operating Systems I 
// Spring 2015 (Tony Leclerc, Brent Munsell)
// utils.c implementation file
// Homework Assignment 1
// Edited By: Paige Peck
// -----------------------------------

// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included
#include <stdio.h>	// IO functions
#include <stdlib.h> 	// atof function
#include <math.h>	// math functions
#include "utils.h"	// your own functions

// Now you put your implementation of the function prototypes here...
// Arguments: 	char* file_name = file name of vector text file
//	      	p_vec_array = pointer to an array of v_structs
// Return:      Number (int data type) of vectors read-in by function
int read(char* file_name,v_struct* p )
{
	FILE* fhnd;		//a pointer to a FILE structure
	char line[50];		//used for reading line of data from a file
	int totalVectors = 0;	//total vectors grabbed from file
	int i = 0;		//counter
	int c = 0;		//counter
	char* comma_ptr;	//pointer for the middle of line, where the comma is
	
	//Open the file
	fhnd = fopen(file_name, "r");

	if (fhnd != NULL)
	{
		printf("File opened successfully\n");
		while (fgets(line, sizeof(line), fhnd) != NULL)
		{
			comma_ptr = line; //initialize the pointer to point to line
			while(line[i] != '\0') //search until end of line
			{
				if(line[i] == ',') //find first comma in line string
				{
					//printf("This is a comma. %d\n", i);
					//Once comma found, point to it, replace it with null character
					if(line[i+1] == ' ')//if there is a space after comma, skip it
					{
						comma_ptr+=(i+1);
						*comma_ptr = '\0';
					}
					else
					{
						comma_ptr+=i;
						*comma_ptr = '\0';
					}//END if(line[i=1]...
					//printf("%s\n", comma_ptr + 1);
					//printf("%s\n", line);
				}//END if(line[i]...
				i++;
			}//END while(line[i]...
			//printf("%s\n", line);
			//printf("%s\n", comma_ptr + 1);
			
			//Convert strings to doubles and store them into the respective struct fields
			p[c].r = atof(line);
			p[c].theta = atof(comma_ptr+1);
			if(p[c].theta > 360)
				p[c].theta -= 360;
			else if(p[c].theta < -360)
				p[c].theta += 360;
			//END if(p[c].theta...
			c++;
			i = 0;
			totalVectors++; //count amount of vectors grabbed from file
		}//END while(fgets...
	}
	else
	{
		printf("Error opening file %s", file_name);
	}//END if(fhnd)...
	/*
	c = 0;
	while(c < 10)
	{
		printf("r = %.2f, ", p[c].r);
		printf("theta = %.2f\n", p[c].theta);
		c++;
	}
	*/
	//printf("%.2f", p[9].theta);

	fclose(fhnd); //close file

	return totalVectors;
}//END read(2)

// Arguments: 	p_vector = point to v_struct 
// Return: 	double that represents x component value
double x_component( v_struct* p_vector )
{
	double temp = 0.0;		//Used for storing temporary values
	temp = (*p_vector).theta * M_PI / 180; //converting theta from degrees to radians
	temp = (*p_vector).r * cos(temp);
	return temp;
}//END x_component(1)

// Arguments: 	p_vector = point to v_struct 
// Return: 	double that represents y component value
double y_component( v_struct* p_vector )
{
	double temp = 0.0;		//Used for storing temporary values
	temp = (*p_vector).theta * M_PI / 180; //converting theta from degrees to radians
	temp = (*p_vector).r * sin(temp);
	return temp;
} //END y_component(1)
