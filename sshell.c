#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

#define CMDLINE_MAX 512
#define ARGV_MAX 32

/* Looked at
*https://www.gnu.org/software/libc/manual/html_mono/libc.html#Process-Completion
*https://www.gnu.org/software/libc/manual/html_mono/libc.html#Pipes-and-FIFOs
*https://www.geeksforgeeks.org/string-functions-in-c/
*https://www.geeksforgeeks.org/exit-status-child-process-linux/
*https://www.geeksforgeeks.org/c-program-to-read-contents-of-whole-file/
*/

struct command {
        char *argv[ARGV_MAX];
        char cmdlinestr[CMDLINE_MAX];
        char *outfile;
        int lastArgument;
};

void cmdfunc(char *input, struct command *cmdstruct) {
        strncpy(cmdstruct->cmdlinestr, input, CMDLINE_MAX);
        cmdstruct->cmdlinestr[CMDLINE_MAX - 1] = '\0';
        cmdstruct->outfile = NULL;


        /*if(strchr(cmdline, '<')){

            char *commands = strtok(input, "<");

            char *inputFile = strtok(NULL, "<");

            char *temp;

            strcat(temp, commands);
            strcat(temp, " ");

            int fd = open(inputFile, )






        }
        else{*/
          char temp[CMDLINE_MAX] = "";
          strncpy(temp, input, CMDLINE_MAX);
          temp[CMDLINE_MAX - 1] = '\0';
        //}

        int argc = 0;
        char *tok = strtok(temp, " ");
        while (tok && argc < ARGV_MAX - 1) {
                char *redir = strchr(tok, '>');
                if (redir) {
                        *redir = '\0';
                        if (strlen(tok) > 0) {
                                cmdstruct->argv[argc++] = strdup(tok);
                        }
                        redir++;
                        if (*redir == '\0') {
                                redir = strtok(NULL, " \t");
                        }
                        cmdstruct->outfile = strdup(redir);
                        break;
                }
                else {
                        cmdstruct->argv[argc++] = strdup(tok);
                }
                tok = strtok(NULL, " ");
        }
        cmdstruct->argv[argc] = NULL;
        cmdstruct->lastArgument = argc;
}

int main(void) {
        char cmdline[CMDLINE_MAX];
        char *eof;
        pid_t pid;

        bool isInProgress = false;
        pid_t inprogress;


        // char *args[] = { cmd, NULL };

        while (1) {
                struct command cmd;
                char *nl;
                // int retval;

                /* Print prompt */
                printf("sshell$ ");
                fflush(stdout);

                /* Get command line */
                eof = fgets(cmdline, CMDLINE_MAX, stdin);
                if (!eof)
                        /* Make EOF equate to exit */
                        strncpy(cmdline, "exit\n", CMDLINE_MAX);

                /* Print command line if stdin is not provided by terminal */
                if (!isatty(STDIN_FILENO)) {
                        printf("%s", cmdline);
                        fflush(stdout);
                }

                /* Remove trailing newline from command line */
                nl = strchr(cmdline, '\n');
                if (nl)
                        *nl = '\0';

                if(strchr(cmdline, '<')){

                  char *commands = strtok(cmdline, "<");

                  char *inputFile = strtok(NULL, "<");

                  char *inputFile2[strlen(inputFile)];

                  if(inputFile[0] == ' '){
                      for(int i=0; i < strlen(inputFile); i++){
                          inputFile2[i] = inputFile2[i+1];
                      }
                  }

                  pid = fork();
                  if(pid == 0)
                  {
                      char character;
                      char *last;

                      //perror(inputFile);
                      //exit(EXIT_FAILURE);

                      FILE *file = fopen("/file.txt", "r");

                      while((character = fgetc(file)) != EOF)
                      {
                      }

                      fclose(file);

                      strcat(last, " ");

                      cmdfunc(commands, &cmd);
                      if (cmd.argv[0] == NULL) {
                              continue;
                      }

                      cmd.argv[cmd.lastArgument] = last;
                      cmd.argv[cmd.lastArgument+1] = NULL;

                      perror(last);
                      exit(EXIT_FAILURE);

                      execvp(cmd.argv[0], cmd.argv);
                      perror("execvp");
                      exit(EXIT_FAILURE);

                  }
                  else if (pid > 0) {
                          int status;
                          /*if(isInProgress){
                            int stat;
                            waitpid(inprogress, &stat, 0);
                            if (WIFEXITED(stat)) {
                                int exit_status = WEXITSTATUS(stat);
                                fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, exit_status);
                            }
                          }*/
                          waitpid(pid, &status, 0);
                          if (WIFEXITED(status)) {
                              int exit_status = WEXITSTATUS(status);
                              fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, exit_status);
                          }
                          continue;
                  }
                  else {
                          perror("fork");
                          exit(EXIT_FAILURE);
                  }
                }

                if(strchr(cmdline, '|')){
                    char *holder = strtok(cmdline, "|");
                    char *pcmdline[4];
                    int parts = 0;
                    while(holder != NULL){
                      pcmdline[parts] = holder;
                      parts++;
                      holder = strtok(NULL, "|");
                    }

                    fflush(stdin);

                    pid_t pid1, pid2, pid3, pid4;
                    struct command cmd1, cmd2, cmd3, cmd4;

                    int statuses;
                    int exit_statuses[parts];

                    int fd[2];

                    pipe(fd);

                    pid1 = fork();
                    if (pid1 == 0) {

                           close(fd[0]);
                           dup2(fd[1], STDOUT_FILENO);
                           close(fd[1]);

                           cmdfunc(pcmdline[0], &cmd1);

                            execvp(cmd1.argv[0], cmd1.argv);
                            perror("execvp piping 1");
                            exit(EXIT_FAILURE);


                    }


                    /*for(int i = 0; i < parts - 1; i++){

                    }*/

                    pid2 = fork();
                    if(pid2 == 0){
                        close(fd[1]);
                        dup2(fd[0], STDIN_FILENO);
                        close(fd[0]);

                        //printf("parts: [%d]", parts);

                        if(parts > 2){
                           pipe(fd);

                           dup2(fd[1], STDOUT_FILENO);
                           pid3 = fork();

                           if(pid3 != 0)
                           {
                             close(fd[0]);
                             close(fd[1]);
                           }
                        }


                           cmdfunc(pcmdline[1], &cmd2);

                           waitpid(pid1, &statuses, 0);
                           if (WIFEXITED(statuses)) {
                               exit_statuses[0] = WEXITSTATUS(statuses);
                           }

                           execvp(cmd2.argv[0], cmd2.argv);
                           perror("execvp piping 2");
                           exit(EXIT_FAILURE);

                    }

                    if(parts > 2){
                        if(pid3 == 0){
                          close(fd[1]);
                          dup2(fd[0], STDIN_FILENO);
                          close(fd[0]);

                          if(parts > 3){
                              pipe(fd);

                              dup2(fd[1], STDOUT_FILENO);

                              pid4 = fork();

                              if(pid4 != 0)
                              {
                                close(fd[0]);
                                close(fd[1]);
                              }
                          }

                              waitpid(pid2, &statuses, 0);
                              cmdfunc(pcmdline[2], &cmd3);

                              execvp(cmd3.argv[0], cmd3.argv);
                              perror("execvp piping 3");
                              exit(EXIT_FAILURE);

                      }
                    }

                    if(parts > 3){

                        if(pid4 == 0){
                          close(fd[1]);
                          dup2(fd[0], STDIN_FILENO);
                          close(fd[0]);

                          cmdfunc(pcmdline[3], &cmd4);

                          waitpid(pid3, &statuses, 0);

                          execvp(cmd4.argv[0], cmd4.argv);
                          perror("execvp piping 4");
                          exit(EXIT_FAILURE);

                        }

                    }

                    if(parts == 2){
                      /*waitpid(pid1, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[1] = WEXITSTATUS(statuses);
                      }
                      waitpid(pid2, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[1] = WEXITSTATUS(statuses);
                      }*/
                      waitpid(pid2, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          exit_statuses[1] = WEXITSTATUS(statuses);
                      }
                      fprintf(stderr, "+ completed '%s' [%d][%d]\n", cmdline, exit_statuses[0], exit_statuses[1]);

                    }
                    else if(parts == 3){
                      /*waitpid(pid1, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[2] = WEXITSTATUS(statuses);
                      }
                      waitpid(pid2, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[1] = WEXITSTATUS(statuses);
                      }
                      waitpid(pid3, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[1] = WEXITSTATUS(statuses);
                      }*/
                      waitpid(pid3, NULL, 0);
                      fprintf(stderr, "+ completed '%s' [%d][%d][%d]\n", cmdline, exit_statuses[0], exit_statuses[1], exit_statuses[2]);

                    }
                    else
                    {
                      /*waitpid(pid1, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[3] = WEXITSTATUS(statuses);
                      }
                      waitpid(pid2, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[1] = WEXITSTATUS(statuses);
                      }
                      waitpid(pid3, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[1] = WEXITSTATUS(statuses);
                      }
                      waitpid(pid4, &statuses, 0);
                      if (WIFEXITED(statuses)) {
                          int exit_statuses[1] = WEXITSTATUS(statuses);
                      }*/
                      waitpid(pid4, NULL, 0);
                      fprintf(stderr, "+ completed '%s' [%d][%d][%d][%d]\n", cmdline, exit_statuses[0], exit_statuses[1], exit_statuses[2], exit_statuses[3]);

                    }
                    continue;

                }

                if(strchr(cmdline, '&')){
                  char* newcmdline = strtok(cmdline, "&");

                  isInProgress = true;

                  inprogress = fork();
                  if(inprogress == 0){
                    cmdfunc(newcmdline, &cmd);
                    if (cmd.argv[0] == NULL) {
                            break;
                    }

                    /* Builtin command */
                    if (!strcmp(cmdline, "exit")) {
                            fprintf(stderr, "Bye...\n");
                            break;
                    }

                    /* Builtin command 'pwd' */
                    if (strcmp(cmd.argv[0], "pwd") == 0) {
                            char cwd[CMDLINE_MAX];
                            getcwd(cwd, sizeof(cwd));
                            printf("%s\n", cwd);
                            fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
                            break;
                    }

                    /* Buildin command 'cd' */
                    if (strcmp(cmd.argv[0], "cd") == 0) {
                            chdir(cmd.argv[1]);
                            fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
                            break;
                    }


                    /* Parsing command line arguments */


                    execvp(cmd.argv[0], cmd.argv);
                    perror("execvp");
                    exit(EXIT_FAILURE);

                  }
                  else
                  {
                    continue;
                  }
                }



                cmdfunc(cmdline, &cmd);
                if (cmd.argv[0] == NULL) {
                        continue;
                }

                if(cmd.outfile != NULL){
                    pid = fork();

                    if(pid == 0){

                      int fdout = open(cmd.outfile, O_RDWR);

                      dup2(fdout, STDOUT_FILENO);

                      /* Builtin command 'pwd' */
                      if (strcmp(cmd.argv[0], "pwd") == 0) {
                              char cwd[CMDLINE_MAX];
                              getcwd(cwd, sizeof(cwd));
                              printf("%s\n", cwd);
                              fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
                              continue;
                      }

                      /* Buildin command 'cd' */
                      if (strcmp(cmd.argv[0], "cd") == 0) {
                              chdir(cmd.argv[1]);
                              fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
                              continue;
                      }

                      execvp(cmd.argv[0], cmd.argv);
                      perror("execvp");
                      exit(EXIT_FAILURE);

                    }
                    else if (pid > 0) {
                            int status;
                            waitpid(pid, &status, 0);
                            if (WIFEXITED(status)) {
                                int exit_status = WEXITSTATUS(status);
                                fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, exit_status);
                            }
                            continue;
                    }
                    else {
                            perror("fork");
                            exit(EXIT_FAILURE);
                    }
                }

                /* Builtin command */
                if (!strcmp(cmdline, "exit")) {
                        fprintf(stderr, "Bye...\n");
                        break;
                }

                /* Builtin command 'pwd' */
                if (strcmp(cmd.argv[0], "pwd") == 0) {
                        char cwd[CMDLINE_MAX];
                        getcwd(cwd, sizeof(cwd));
                        printf("%s\n", cwd);
                        if(isInProgress){
                          int stat;
                          waitpid(inprogress, &stat, 0);
                          if (WIFEXITED(stat)) {
                              int exit_status = WEXITSTATUS(stat);
                              fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, exit_status);
                          }
                        }
                        fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
                        continue;
                }

                /* Buildin command 'cd' */
                if (strcmp(cmd.argv[0], "cd") == 0) {
                        chdir(cmd.argv[1]);
                        if(isInProgress){
                          int stat;
                          waitpid(inprogress, &stat, 0);
                          if (WIFEXITED(stat)) {
                              int exit_status = WEXITSTATUS(stat);
                              fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, exit_status);
                          }
                        }
                        fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
                        continue;
                }




                /* Parsing command line arguments */



                pid = fork();
                if (pid == 0) {

                        /*if(strchr(cmdline, '<')){

                            char *commands = strtok(cmdline, "<");
                            char *inputFile = strtok(NULL, "<");

                            dup2(open(inputFile,O_RDONLY), STDIN_FILENO);

                            cmdfunc(commands, &cmd);

                        }*/


                        execvp(cmd.argv[0], cmd.argv);
                        perror("execvp");
                        exit(EXIT_FAILURE);
                }
                else if (pid > 0) {
                        int status;
                        /*if(isInProgress){
                          int stat;
                          waitpid(inprogress, &stat, 0);
                          if (WIFEXITED(stat)) {
                              int exit_status = WEXITSTATUS(stat);
                              fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, exit_status);
                          }
                        }*/
                        waitpid(pid, &status, 0);
                        if (WIFEXITED(status)) {
                            int exit_status = WEXITSTATUS(status);
                            fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, exit_status);
                        }
                }
                else {
                        perror("fork");
                        exit(EXIT_FAILURE);
                }
        }

        return EXIT_SUCCESS;
}
