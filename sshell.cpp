# include <stdio.h>
# include <unistd.h>
# include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

# define MAXLINE 80 /* The maximum length command  */

int main(void)
{
  char buf[MAXLINE/2 + 1]; /*command line arguments */
  char *args[MAXLINE/2 + 1];
  int shouldrun = 1; /* flag to determine when to exit program */
  int status = 0;
  char *token;
  const char *quit = "quit";
  const char *connect = "&";
  
  // The program would keep prompting user as long as shouldrun == 1
  while(shouldrun == 1)
  {
    bool hasConnect = false; //check if there is & in the command
    int i = 0;
    pid_t pid;
    memset(buf, 0, sizeof buf); //clear the buf
    usleep(150000); // after running each command, wait for 0.15 seconds
    printf("osh> ");
    fflush(stdout);

    // get input from the users
    fgets(buf, MAXLINE, stdin);

    // https://stackoverflow.com/questions/41112659/c-while-loop-exit-condition-using-fgets-for-input
    // Check the length of the input
    int len = strlen(buf);

    if (len > 1)
    {
      if (buf[len - 1] == '\n')
      {
        buf[len - 1] = '\0';
        // If user input quit, the program would quit
        if (strcmp(buf, quit) == 0)
        {
          printf("Goodbye\n");
          break;
        }
      }
      else
      {
        exit(EXIT_FAILURE);
      }
    }
    
    // deleted the \n ot \0 at the end of the buffer
    buf[len - 1] = ' ';

    /* get the first token */
    // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    token = strtok(buf, " ");

    /* walk through other tokens */
    while (token != NULL)
    {
      // check if the input is quit
      if (strcmp(token, connect) == 0)
        hasConnect = true;
      // putting user inputs to a char array
      args[i++] = token;
      token = strtok(NULL, " ");
    }
    
    // set the last element in the array to be NULL
    args[i] = args[i - 1];
    args[i] = NULL;

    // fork a child process using fork()
    pid = fork();
    if(pid < 0)
    {
      printf("Fork error: %d (%s)\n", errno, strerror(errno));
      return -1;
      shouldrun = 0;
    }
    else if(pid == 0)
    { 
      /* This is the child process */
      status = execvp(args[0], args);
      if (status == -1)
      {
        perror("error in execvp");
        return 0;
      }
      printf("child process.\n");
      return 0;
      shouldrun = 0;
    }
    else
    { 
      /* This is the parent process */
      if(hasConnect)
      {
        cout << "has an &" << endl;
        int stat_val;
        pid_t child_pid;

        child_pid = wait(&stat_val);

        printf("Child has finished: PID = %d \n", child_pid);
        if (WIFEXITED(stat_val))
        {
          printf("Child exited with code %d\n", WEXITSTATUS(stat_val));
        }

        printf("Parent has finished \n");
      }
    }

    /**
     * After reading user input, the steps are:
     * (1) fork a child process using fork()
     * (2) the child process will invoke execvp()
     * (3) if command included &, parent will incoke wait()
     */
  }

  return 0;
}