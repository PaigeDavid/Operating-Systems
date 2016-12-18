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

// get the next token starting from line.  Return length of token.
int get_next_token(char* line, char** token)
{
  int i;                 // for loop control variable
  int start;             // start of some non-white space block
  int inwhite = TRUE;    // TRUE if "in" white space

  // go through the string one character at a time
  for (i = 0; line[i] != '\0'; i++) {  // NOTE:  i incremented ONLY here!
    if (iswhite(line[i])) {  //   did we find a white space?
      if (!inwhite) {        //   if not already in white space
	break;               //       then we got a new token
      }
      inwhite = TRUE;        //   remember the state that we are in white space
    }
    else {                   // not a white space character (ie. an argument)
      if (inwhite) {         // remember where we transitioned from a white
	start = i;           //   to non-white region
      }
      inwhite = FALSE;       // we are now not in a white space state
    }
  }

  if (!inwhite) {            // if we didn't end with a white space
    *token = line + start;   //     then we got a new token
    return i - start;
  }

  *token = NULL;             // if we get here, no tokens available
  return 0;
}

int count_tokens(char* line)
{
  int num_tokens = 0;
  char *token;
  int length = get_next_token(line, &token);
  
  while (length > 0) {
    num_tokens++;
    line = token + length;
    length = get_next_token(line, &token);
  }
  
  return num_tokens;
}

void parse(char* line, command_t* p_cmd)
{
  int i;
  int length;
  char *token;
  
  // get number of command line arguments
  p_cmd->argc = count_tokens(line);

  if (p_cmd->argc > 0) {
    // allocate space for arguments array (+ 1 for NULL)
    p_cmd->argv = (char **) malloc((p_cmd->argc+1) * sizeof(char *));

    for (i = 0; i < p_cmd->argc; i++) {   // for each argument...
      length = get_next_token(line, &token);
      line = token + length;
      p_cmd->argv[i] = (char *) malloc(length + 1);
      my_strncpy(p_cmd->argv[i], token, length);
    }
    p_cmd->argv[i] = NULL;
  }
  else {   // blank line entered
    p_cmd->argv = (char **) malloc(2 * sizeof(char *));
    p_cmd->argv[0] = (char *) malloc(1);
    p_cmd->argv[0][0] = '\0';
    p_cmd->argv[1] = NULL;
  }
  // allocate space and copy argv[0] into name
  p_cmd->name = (char *) malloc(my_strlen(p_cmd->argv[0])+1);
  my_strncpy(p_cmd->name, p_cmd->argv[0], my_strlen(p_cmd->argv[0]));
}

int execute(command_t* p_cmd)
{
  int i,j;
  int found = FALSE;
  pid_t pid1, pid2;
  int status, child_process_status;
  char fullpath1[MAXSTRLEN];
  char fullpath2[MAXSTRLEN];
  int fds[2];
  //char** pargs = (char**) malloc (p_cmd->argc * sizeof(char*));
  //char** cargs = (char**) malloc (p_cmd->argc * sizeof(char*));
  char** pargs;
  //char* cargs[] = {"wc","-l",NULL};

  //Check to see if this is a piped input (input has the '|' character)
  if(isPiped(p_cmd))
  {
	  //Set parameter argument to the p_cmd argv
	  pargs = p_cmd->argv;
	  
	  //Find the '|' character, and reassign it to the '\0' character
	  for(i = 0; p_cmd->argv[i] != '\0' && found == FALSE; i++)
	  {
		for(j = 0; p_cmd->argv[i][j] != '\0'; j++)
		{
			if(isBar(p_cmd->argv[i][j]))
			{
				//printf("cargs: %s\n", cargs[i]);
				p_cmd->argv[i] = '\0';
				found = TRUE;
				break;
			}
		}
	  	
	  }
	  /*
	  for(p = 0; pargs[p] != '\0'; p++)
	  {
	 	 printf("pargs: %s\n", pargs[p]);
	  }
	  printf("cargs: %s\n", cargs[i]);
	  */
	  if(pipe(fds))
	  {
	    fprintf(stderr, "Pipe failed.\n");

	  }
	  
	  //Fork twice, first one using the pargs as execv, which is the beginning of p_cmd->argv,
	  //then the second fork uses pargs+i, where i is where the newline character was placed. 
	  if ((pid1 = fork()) == 0) {
	    find_fullpath(fullpath1, p_cmd);
	    //printf("%s\n", fullpath);
	    close(1);
	    dup(fds[1]);
	    close(fds[0]);
	    execv(fullpath1,pargs);
	    //execv(fullpath, p_cmd->argv);
	    perror("Execute terminated with an error condition!-1\n");
	    //exit(1);
	  }
	  if((pid2 = fork()) == 0) {
	    find_fullpathPiped(fullpath2, pargs+i);
	    //printf("%s\n", fullpath);
	    close(0);
	    dup(fds[0]);
	    close(fds[1]);
	    execv(fullpath2,pargs+i);
	    //execv(fullpath,p_cmd->argv);
	    perror("Execute terminated with an error condition!-2\n");
	  }
	  //printf("Made it here 1\n");
   	  close(fds[0]);
	  close(fds[1]);
 	  waitpid(pid1, &child_process_status, 0);
	  waitpid(pid2, &child_process_status, 0); 
	  
	  /*
	  for(j = 0; j < i-placement; j++)
	  {
		//printf("j: %d, i: %d, argc: %d\n", j,i,p_cmd->argc);
		free(cargs[j]);
		cargs[j] = NULL;
		//printf("Making it here 2\n");
	  }
	  */
  }
  else
  {
    //printf("not piped\n");
    if ((pid1 = fork()) == 0) {
      find_fullpath(fullpath1, p_cmd);
      execv(fullpath1, p_cmd->argv);
      perror("Execute terminated with an error condition!\n");
      exit(1);
    }
  }
  //printf("Make it here 3\n");
  //Issue also possibly stems from here as well, double free
  //printf("argv[%d] 1: %s\n", placement, p_cmd->argv[placement]);
  //free(cargs);
  //cargs = NULL;
  //printf("argv[%d] 2: %s\n", placement, p_cmd->argv[placement]);
  return wait(&status);
}

//Check to see if the argv is a piped line
int isPiped(command_t* p_cmd)
{
	int i,j;
	//Check if there is a bar symbol, if it is a piped statement
	for (i = 0; p_cmd->argv[i] != '\0'; i++) 
	{ 
		for(j = 0; p_cmd->argv[i][j] != '\0'; j++)
		{
			if (isBar(p_cmd->argv[i][j]))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

int find_fullpath(char* fullpath, command_t* p_cmd)
{
  int i;
  int start = 0;
  char dir[MAXSTRLEN];    // directory stripped from PATH
  char *mypathvar;        // copy of string returned by getenv("PATH")
  struct stat sb;         // for stat() call
  char *pathvar = getenv("PATH");
  int pathvarlen = my_strlen(pathvar);

  // allocate space for pathvar (+ appending a colon and null character)
  // copy pathvar into mypathvar with extra colon appended
  // this allows all processing to occur within the loop
  mypathvar = (char *) malloc(pathvarlen+2);
  my_strncpy(mypathvar, pathvar, pathvarlen);
  mypathvar[pathvarlen] = ':';  mypathvar[pathvarlen+1] = '\0';
  
  for (i = 0; mypathvar[i] != '\0'; i++) {
    if (mypathvar[i] == ':') {
      my_strncpy(dir, mypathvar+start, i - start);
      start = i+1;
      // build the fullpath string
      my_strncpy(fullpath, dir, MAXSTRLEN);
      my_strncat(fullpath, "/", MAXSTRLEN);
      my_strncat(fullpath, p_cmd->name, MAXSTRLEN);
      
      if (stat(fullpath, &sb) != -1 && S_ISREG(sb.st_mode)) {
	  return TRUE;
      }
    }
  }
  
  return FALSE;
}

//Used for piped fullpath check
int find_fullpathPiped(char* fullpath, char** cmd)
{
  int i;
  int start = 0;
  char dir[MAXSTRLEN];    // directory stripped from PATH
  char *mypathvar;        // copy of string returned by getenv("PATH")
  struct stat sb;         // for stat() call
  char *pathvar = getenv("PATH");
  int pathvarlen = my_strlen(pathvar);

  // allocate space for pathvar (+ appending a colon and null character)
  // copy pathvar into mypathvar with extra colon appended
  // this allows all processing to occur within the loop
  mypathvar = (char *) malloc(pathvarlen+2);
  my_strncpy(mypathvar, pathvar, pathvarlen);
  mypathvar[pathvarlen] = ':';  mypathvar[pathvarlen+1] = '\0';
  
  for (i = 0; mypathvar[i] != '\0'; i++) {
    if (mypathvar[i] == ':') {
      my_strncpy(dir, mypathvar+start, i - start);
      start = i+1;
      // build the fullpath string
      my_strncpy(fullpath, dir, MAXSTRLEN);
      my_strncat(fullpath, "/", MAXSTRLEN);
      my_strncat(fullpath, cmd[0], MAXSTRLEN);
      
      if (stat(fullpath, &sb) != -1 && S_ISREG(sb.st_mode)) {
	  return TRUE;
      }
    }
  }
  
  return FALSE;
}

int is_builtin(command_t* p_cmd)
{
  int i;

  for (i = 0; valid_builtin_commands[i] != NULL; i++) {
    if (my_strequal(p_cmd->name, valid_builtin_commands[i]))
      return TRUE;
  }
  return FALSE;
}

int do_builtin(command_t* p_cmd)
{
  if (my_strequal(p_cmd->name, valid_builtin_commands[0]))
    return chdir(p_cmd->argv[1]);

  // can add more commands ...
  
  return SUCCESSFUL;
}

void cleanup(command_t* p_cmd)
{
  int i;
  //printf("Make it here 1\n");
  free(p_cmd->name);
  //p_cmd->name = NULL;
  //printf("Make it here a\n");
  //Issue stems from here, double free
  for (i = 0; i < p_cmd->argc; i++) {
   //printf("Make it here: %d\n", i);
    free(p_cmd->argv[i]);
    p_cmd->argv[i] = NULL;
  }
  //printf("Make it here b\n");
  free(p_cmd->argv);
  p_cmd->argv = NULL;
  p_cmd->argc = 0;
  
}

// **** MY UTILITY FUNCTIONS

int my_strlen(const char* s)
{
  int count = 0;
  while (*s++ != '\0')
    count++;

  return count;
}

// assumes properly terminated strings
int my_strequal(const char *s, const char *t)
{
  while (*s != '\0' && *s == *t) {
    s++; t++;
  }

  return *s == '\0' && *t == '\0';
}

int my_strncpy(char *dest, const char *src, int n)   // copy dest to src
{
  while (*src != '\0' && n--) {
    *dest++ = *src++;
  }
  *dest = '\0';
  
  return 0;
}

int my_strncat(char *dest, const char *src, int n)   // append dest to src
{
  while (*dest != '\0')
    dest++;    // move to the end of dest
  
  while (*src != '\0' && n--) {
    *dest++ = *src++;
  }
  *dest = '\0';
  
  return 0;
}

int iswhite(char c)
{
  return c == ' ' || c == '\t';
}

int isBar(char c)
{
	return c == '|';
}
