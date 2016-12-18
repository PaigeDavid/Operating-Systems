#include <stdio.h>         // for printf()
#include <pthread.h>       // for pthread_xxx() routines
#include <limits.h>

#include "mcsim.h" // monte carlo simulation function prototypes

double gcount;    // global counter -- a double to handle large sums (billions+)
long numits;      // global variable for number of iterations (see step 3 below)

int main(int argc, char** argv)
{
  pthread_attr_t attr;
  gcount = 0;

  /*

    ---------------------------------------------------------------------
    Step-by-Step procedure for Monte carlo simulator to estimate PI
    ---------------------------------------------------------------------
    1. Create the following variables:
       - pointer to an array of pthread structures
       - counting variables for loops
       - thread id
       - exit status of joined thread
       - number of threads to run
       - maximum number of threads (hint: rlimit)
       - nanosecond start and stop timespec structs
    2. Get number of threads input by user from argv[1]
    3. Get number of iterations input by user from argv[2]
    4. Get maximum number of threads the OS can create (man getrlimit function)
    5. If number of threads > maximum OS threads then goto 14, else goto step 6
    6. Allocate array of pthread structures using number of threads from step 2
    7. Get start time of simulation and store in timespec struct
    8. Create a pthread for each position in pthread array created in step 6
       - if an error occurs during thread creation exit simulation immediately
    9. Join each pthread created in the pthread array
       - if an error occurs during thread join exit simulation immediately
    10. Get stop time of simulation and store in timespec struct
    11. Print out:
       - value of PI using 6 decimal places
       - value stored in global variable gcount using 0 decimal places
       - time in seconds to run the simulation using 4 decimal places (hint: use mydifftime function)
    12. Free memory allocated for pthread array
    13. Call pthread terminate function (hint: pthread_exit function)
    14. exit simulation program

    ---------------------------------------------------------------------
    Example input and output of Monte carlo simulator to estimate PI
    ---------------------------------------------------------------------

    # ./hw5 300 100  // i.e. run simulatior using 300 threads and 100 iterations

    monte carlo value of PI = 3.141200
    value of count = 23559
    time in seconds = 0.0761
  */

  pthread_t *arrayOfThreads;
  int i = 0;
  long threadID = 0;
  long exitStatus = 0;
  int numOfThreads;
  int error_n;
  struct rlimit limit;
  struct timespec start;
  struct timespec stop;
  double totalTime;
 
  if(argc < 3)
  {
    printf("Please enter the number of threads and iterations in the arguments. Exiting...");
    exit(-1);
  }
  
  numOfThreads = atoi(argv[1]);
  numits = strtol(argv[2], NULL, 10);
  getrlimit(RLIMIT_FSIZE, &limit);

  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

  if(numOfThreads > limit.rlim_cur)
  {
    printf("Number of threads is more than maximum allowed. Exiting Simultation");
    exit(-1);
  }
  else
  {

    arrayOfThreads = (pthread_t *) malloc(sizeof(pthread_t) * numOfThreads);
    clock_gettime(CLOCK_REALTIME, &start);
    for(i = 0; i < numOfThreads; i++)
    {
      if((error_n = (pthread_create(&(arrayOfThreads[i]), &attr, &th_routine, (void *) threadID++))) != 0)
      {
        printf("The thread at %d caused the program to exit. \n", i);
        exit(error_n);
      }
    }
    for ( i = 0; i < numOfThreads; i++ ) 
    {
      if ((error_n = (pthread_join(arrayOfThreads[i], NULL))) != 0 ) 
      {
        printf("The thread at %d caused the program to exit. \n", i);
        exit(error_n);
      }
    }

    clock_gettime(CLOCK_REALTIME, &stop);
    totalTime = mydifftime(&start, &stop);

    printf("monte carlo value of PI = %.6f\n", ((4*gcount)/(numits*numOfThreads)));
    printf("value of count = %.0f\n", gcount);
    printf("time in seconds: %.4f\n", (totalTime/1000000000));
  }
   
  free(arrayOfThreads);
  pthread_exit((void*) exitStatus);

  return 0;

} // end main function
