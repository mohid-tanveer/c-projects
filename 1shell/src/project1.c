#include "cwd.h" // for cwd()
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #include <tchar.h>
  #include <Windows.h>
#else
  #include <unistd.h>
  #include <limits.h>
  #include <sys/wait.h>
#endif

#define CMD_SIZE 512
#define MAX_ARGS 512
#define BUF_LEN 512

// initializes the variable pathReturned that will be given by getenv("PATH").
// initializes the circular buffer history and a count variable for how many
// commands are in history.
char *pathReturned = NULL;
char history[BUF_LEN][CMD_SIZE];
int cnt = 0;

// put(char *item) places a new command into the history buffer and if about to
// be filled resets the count to zero. Initially the buffer will be highest
// number represents the most recently executed command until the buffer is
// filled and it starts placing new commands from index 0 on again replacing the
// oldest commands..
void put(char *item) {
  strncpy(history[cnt], item, CMD_SIZE);
  cnt++;
  cnt %= BUF_LEN;
}

// get(int index) simply returns the command at the index of the history buffer
char *get(int index) {
  char *item = history[index];
  return item;
}

// print_Path() copys the PATH given by $PATH and seperates each singular path
// whenever a semi-colon appears
void print_Path() {
  char paths[CMD_SIZE];
  pathReturned = getenv("PATH");
  strncpy(paths, pathReturned, CMD_SIZE);
  printf("Welcome to MyShell!\nPATH:");
  char *curr = strtok(paths, ":");
  while (curr != NULL) {
    printf("\t%s\n", curr);
    curr = strtok(NULL, ":");
  }
}

// commandLoop(int argc, char **argv) runs the commandLoop until user types exit
void commandLoop(int argc, char **argv) {
  char command[512];
  char currDir[512];
  // runs loop while checking if the user input is exit. if so exits.
  while (strncmp(command, "exit", 4) != 0) {
    printf("$ ");
    fgets(command, CMD_SIZE, stdin);
    // initialized integer index which will represent where the non-white space
    // characters of the command ends.
    int index = 0;
    // if no command given prints and continues loop.
    if (strncmp(command, "\n", 1) == 0) {
      printf("no command specified\n");
      continue;
    }
    // if there is white space at beginning of command tells user and continues
    // loop.
    if (isspace(command[0])) {
      printf("please remove white space from beginning of command\n");
      continue;
    }
    // loops from end of command until finds an index that contains white space
    // and whose previous index does not contain white space. should be O(1)
    // amortized inside the loop and only O(n) if user adds additional white
    // space at end.
    for (int i = strlen(command) - 1; i > 0; i--) {
      if (isspace(command[i]) && !isspace(command[i - 1])) {
        index = i;
        break;
      }
    }
    command[index] = '\0';
    // initializes a variable finalCommand that allows for the case when a user
    // requests a command within the history.
    char *finalCommand = malloc(CMD_SIZE);
    if (finalCommand == NULL) {
      perror("malloc() error");
    }
    // if user requests a previous command sets the finalCommand as the command
    // within history. if the index is invalid prints statement and continues
    // the loop.
    if (strncmp(command, "!", 1) == 0) {
      int index = atoi(&command[1]);
      if (!isalpha(history[index][0])) {
        printf("Not a valid index for command in history\n");
        continue;
      }
      strncpy(finalCommand, get(index), CMD_SIZE);
      printf("%s\n", finalCommand);
    } else {
      strncpy(finalCommand, command, CMD_SIZE);
    }
    // if user wishes to see the history prints history.
    if (strncmp(finalCommand, "history", 7) == 0) {
      printf("history of commands: \n");
      for (int i = 0; i < BUF_LEN; i++) {
        // if the index of history is blank does not print an index and command
        if (!isalpha(history[i][0])) {
          continue;
        } else {
          printf("%d: %s\n", i, get(i));
        }
      }
      continue;
    }
    put(finalCommand);
    if (strncmp(finalCommand, "exit", 4) == 0) {
      break;
    } else if (strncmp(finalCommand, "pwd", 3) == 0) { // prints working
                                                       // directory
#ifdef _WIN32
      if (GetCurrentDirectory(CMD_SIZE, currDir) == -1) {
        perror("GetCurrentDirectory() error");
      } else {
        printf("%s\n", currDir);
      }
#else
      if (getcwd(currDir, CMD_SIZE) == NULL) {
        perror("getcwd() error");
      } else {
        printf("%s\n", currDir);
      }
#endif
    } else if (strncmp(finalCommand, "cd", 2) == 0) { // changes current
                                                      // directory
      const char space[] = " ";
      // initializes the newDirectory and isolates it from the command.
      char *newDir;
      newDir = strtok(&finalCommand[3], space);
#ifdef _WIN32
      if (SetCurrentDirectory(newDir) == -1) {
        perror("SetCurrentDirectory() error");
      }
#else
      if (chdir(newDir) == -1) {
        perror("chdir() error");
      }
#endif
    } else if (strncmp(finalCommand, "./", 2) == 0) { // runs an executable
      // clears the arguments in argv_exec and places the arguments from the
      // command in it
#ifdef _WIN32
      // Windows-specific code using CreateProcess()
      STARTUPINFO si;
      PROCESS_INFORMATION pi;
      ZeroMemory(&si, sizeof(si));
      si.cb = sizeof(si);
      ZeroMemory(&pi, sizeof(pi));

      // Convert command to a mutable TCHAR string
      TCHAR writableCommand[CMD_SIZE];
      _tcscpy_s(writableCommand, CMD_SIZE, _T(finalCommand));

      // Create the process
      if (!CreateProcess(NULL, writableCommand, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
      {
          printf("Failed to execute command: %s\n", finalCommand);
          return;
      }

      // Wait for the process to finish
      WaitForSingleObject(pi.hProcess, INFINITE);

      // Clean up handles
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
#else
      extern char **environ;
      char *argv_exec[MAX_ARGS];
      memset(argv_exec, '\0', MAX_ARGS - 1);
      char *arg = strtok(finalCommand, " ");
      int index = 0;
      while (arg != NULL) {
        argv_exec[index] = arg;
        arg = strtok(NULL, " ");
        index++;
      }
      pid_t pid = fork();
      if (pid == -1) {
        perror("fork() error");
      } else if (pid == 0) { // child runs the exec
        if (execve(argv_exec[0], argv_exec, environ) == -1) {
          perror("execve() error");
        }
      } else { // parent waits
        wait(NULL);
      }
#endif
    } else { // user wishes to execute an exec that is not in the cwd
      // same as above clears and places new arguments.
#ifdef _WIN32
      // Windows-specific code using CreateProcess()
      STARTUPINFO si;
      PROCESS_INFORMATION pi;
      ZeroMemory(&si, sizeof(si));
      si.cb = sizeof(si);
      ZeroMemory(&pi, sizeof(pi));

      // Convert command to a mutable TCHAR string
      TCHAR writableCommand[CMD_SIZE];
      _tcscpy_s(writableCommand, CMD_SIZE, _T(finalCommand));

      // Create the process
      if (!CreateProcess(NULL, writableCommand, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
      {
          printf("Failed to execute command: %s\n", finalCommand);
          return;
      }

      // Wait for the process to finish
      WaitForSingleObject(pi.hProcess, INFINITE);

      // Clean up handles
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
#else
      extern char **environ;
      char *argv_exec[MAX_ARGS];
      memset(argv_exec, '\0', MAX_ARGS - 1);
      char *arg = strtok(finalCommand, " ");
      int index = 0;
      while (arg != NULL) {
        argv_exec[index] = arg;
        arg = strtok(NULL, " ");
        index++;
      }
      char paths[CMD_SIZE];
      strncpy(paths, pathReturned, CMD_SIZE);
      char *curr = strtok(paths, ":");
      // isolates individual paths and searches for exec within the paths. if
      // found runs the exec
      while (curr != NULL) {
        char temp[CMD_SIZE];
        strncpy(temp, curr, CMD_SIZE);
        strncat(temp, "/", 2);
        strncat(temp, argv_exec[0], strlen(argv_exec[0]));
        if (access(temp, X_OK) == 0) {
          argv_exec[0] = temp;
          pid_t pid = fork();
          if (pid == -1) {
            perror("fork() error");
          } else if (pid == 0) { // child runs exec
            if (execve(argv_exec[0], argv_exec, environ) == -1) {
              perror("execve() error");
            }
          } else { // parent waits
            wait(NULL);
          }
          break;
        }
        curr = strtok(NULL, ":");
      }
#endif
    }
    free(finalCommand);
  }
}

// main(int argc, char **argv) prints path and runs the command loop.
int main(int argc, char **argv) {
  print_Path();
  commandLoop(argc, argv);
  return 0;
}