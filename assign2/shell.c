/*
Shell.c
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

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist, however in future, more could 
// be added to the builtin command array.
// --------------------------------------------
const char* valid_builtin_commands[] = {"cd", "exit", NULL};

// --------------------------------------------
// Implementation for each prototype below
// --------------------------------------------
void parse(char* line, command_t* p_cmd)
{
	int i = 0, d = 0; 		//Counter for while loop and index for array
	char* space_ptr = line;
	int cntr = 1;				//Counter for keeping track of variables
	int space = FALSE;
	

	if(line[0] == '\0')
	{
		p_cmd->name = line;
		p_cmd->argc = 0;
		p_cmd->argv[0] = line;
	}
	else
	{
		while(line[i] != '\0')
		{
			if(line[i] == ' ')
			{
				space_ptr += i - d;
				*space_ptr = '\0';
				p_cmd->argv[cntr] = space_ptr+1;
				cntr++;
				d = i;
				space = TRUE;
			}//END if(line[i] ==...
			i++;
		}//END while(line[i] !=...
		p_cmd->name = line;
		if(space == TRUE)
		{
			p_cmd->argc = cntr-1;
			p_cmd->argv[0] = line;
			p_cmd->argv[cntr] = '\0';
		}
		else
		{
			p_cmd->argc = cntr;
			p_cmd->argv[0] = line;
			p_cmd->argv[cntr] = '\0';
		}
	}
	
		
	//printf("name[2] = %c, valid[2] = %s", (*p_cmd).name[2], valid_builtin_commands[2]);
	/*if(cntr > 0)
	{
		printf("Name: \"%s\"\nargc = %d\nargv = {\"%s\", \"%s\", %s}\n", (*p_cmd).name, (*p_cmd).argc, p_cmd->argv[0], p_cmd->argv[1], p_cmd->argv[2]);
	}
	else
	{
		printf("Name: \"%s\"\nargc = %d\nargv = {\"%s\"}\n", (*p_cmd).name, (*p_cmd).argc, p_cmd->argv[0]);
	}
	*/
}//END parse

int execute(command_t* p_cmd)
{
	int fnd = FALSE;
	char* fullpath;
	fullpath = (char*) malloc (100*sizeof(char));
	int status;

	//printf("execute\n");
	fnd = find_fullpath(fullpath, p_cmd);
	//printf("full path after execute's call: %s\n", fullpath);
	//printf("fnd: %d\n", fnd);

	if (fnd) 
	{
		if (fork() == 0) 
		{
			//This is the child
			//Execute in same environment as parent
			execv(fullpath, p_cmd->argv);
			perror("Execute terminated with an error condition!\n");
			exit( 1 );
		}
		wait(&status);
	} else 
	{
		// display to user cmd not found
		printf("Command \"%s\" not found.\n", p_cmd->argv[0]);
	}

	free(fullpath);
	return status;	
}//END execute

int find_fullpath(char* fullpath, command_t* p_cmd)
{
	char* path_env_variable;
	char* my_path;
	int i = 0;
        char* my_path_save;
	char* colon_ptr;
	char* newString;
	struct stat buffer;
	int exists;
	//String that represents the fully qualified
	//path of a file or director on the file system
	char* file_or_dir;
	int is_dir_or_file = FALSE;
	
	path_env_variable = getenv("PATH");
	//printf("PATH = %s\n", path_env_variable);
	for(i = 0; path_env_variable[i] != '\0'; i++) {}

	my_path = (char*) malloc (i*sizeof(char)+1);
	my_path_save = my_path;

	for(i = 0; path_env_variable[i] != '\0'; i++)	
	{
		my_path[i] = path_env_variable[i];
	}
	my_path[i] = '\0';


	//printf("%s\n", my_path);
	i = 0;
	colon_ptr = my_path;
	while((my_path[i] != '\0') && is_dir_or_file == FALSE)
	{
		if(my_path[i] == ':')
		{
			colon_ptr += i;
			*colon_ptr = '\0';
			//file_or_dir = path_env_variable;
			newString = (char*) malloc (100* sizeof(char*));
			file_or_dir = str_append(my_path, p_cmd->argv[0], newString);
			colon_ptr += 1;
			my_path = colon_ptr;
			i = 0;

			exists = stat( file_or_dir, &buffer);
			//printf("File/Directory: %s\n", file_or_dir);
	
			if( exists == 0 && ( S_IFREG & buffer.st_mode) )
			{
				//File exists
				//printf("File exists\n");
				for(i = 0; file_or_dir[i] != '\0'; i++)
				{
					fullpath[i] = file_or_dir[i];
				}
				fullpath[i] = '\0';
				//printf("Path = %s\n", fullpath);
				is_dir_or_file = TRUE;
			}
			else
			{
				//Not a valid file
				//printf("%s is not a valid file\n", file_or_dir);
			}
			
			free(newString);
			newString = NULL;

		}
		i++;
	}
	free(my_path_save);
	my_path_save = NULL;
	return is_dir_or_file;

}//END find_fullpath

char* str_append(char* str1, char* str2, char* newString)
{
	int i,j, c = 0;
	for(i = 0; str1[i] != '\0'; i++) 
	{
		newString[c] = str1[i];
		c++;
	}
	newString[c] = '/';
	c++;
	for(j = 0; str2[j] != '\0'; j++) 
	{
		newString[c] = str2[j];
		c++;
	}
	newString[c] = '\0';
	
	return newString;
}//END str_append

int is_builtin(command_t* p_cmd)
{
	int i = 0, p = 0; 	//Counters for loops
	int done = FALSE;	//invalid until proven valid
	while((valid_builtin_commands[i] != NULL) && (done == FALSE))	//Ends loop when we are placed into the final index
	{
		//printf("loop1: %d\n", i);
		while(((*p_cmd).name[p] != '\0' || valid_builtin_commands[i][p] != '\0') && (done == FALSE))
		{
			//printf("loop2: %d\n", p);
			if((*p_cmd).name[p] == valid_builtin_commands[i][p])
			{
				
				if((*p_cmd).name[p+1] == '\0' && valid_builtin_commands[i][p+1] == '\0')
				{
					done = TRUE;
				}
			}
			p++;
		}
		p = 0;
		i++;
	}
	return done;
}//END is_builtin

int do_builtin(command_t* p_cmd)
{
	struct stat buffer;
	int exists;
	int continue_on;

	char* file_or_dir = p_cmd->argv[1];		//Check for every argument? Or just the first?
	exists = stat(file_or_dir, &buffer);

	if(exists == 0 && (S_IFDIR & buffer.st_mode) )
	{
		//Directory exists
		//printf("Directory exists\n");
		continue_on = 1;
	}
	else if(exists == 0 && (S_IFREG & buffer.st_mode) )
	{
		//File exists
		//printf("File exists\n");
		continue_on = 1;
	}
	else
	{
		//Not a valid file or directory
		//printf("Not a valid file or directory\n");
		continue_on = 0;	
	}
	if(continue_on == 1)
	{
		if(chdir(p_cmd->argv[1]) == SUCCESSFUL)
		{
			//printf("Do builtin successful\n");
			return SUCCESSFUL;
		}
		else
		{	
			return ERROR;
		}	
	}
	return ERROR;
	
}//END do_builtin

void cleanup(command_t* p_cmd)
{
	int i = 0;
	while(p_cmd->argv[i] != NULL)
	{
		p_cmd->argv[i] = NULL;
		free(p_cmd->argv[i]);
		i++;
	}
	p_cmd->name = NULL;
	free(p_cmd->name);
}//END cleanup
