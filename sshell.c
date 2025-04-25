#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

#define CMDLINE_MAX 512
#define ARGV_MAX 32

struct command {
    char *argv[ARGV_MAX];
    char cmdlinestr[CMDLINE_MAX];
    char *outfile;
    char *infile;
    int lastArgument;
};

void cmdfunc(char *input, struct command *cmdstruct) {
    strncpy(cmdstruct->cmdlinestr, input, CMDLINE_MAX);
    cmdstruct->cmdlinestr[CMDLINE_MAX - 1] = '\0';
    cmdstruct->outfile = NULL;
    cmdstruct->infile = NULL;

    char temp[CMDLINE_MAX];
    strncpy(temp, input, CMDLINE_MAX);
    temp[CMDLINE_MAX - 1] = '\0';

    char *redir_in = strchr(temp, '<');
    char *redir_out = strchr(temp, '>');

    if (redir_out) {
        *redir_out = '\0';
        redir_out++;
        while (*redir_out == ' ') redir_out++;
        cmdstruct->outfile = strdup(strtok(redir_out, " \t"));
    }
    
    if (redir_in) {
        *redir_in = '\0';
        redir_in++;
        while (*redir_in == ' ') redir_in++;
        cmdstruct->infile = strdup(strtok(redir_in, " \t"));
    }

    int argc = 0;
    char *tok = strtok(temp, " ");
    while (tok && argc < ARGV_MAX - 1) {
        cmdstruct->argv[argc++] = strdup(tok);
        tok = strtok(NULL, " ");
    }
    cmdstruct->argv[argc] = NULL;
    cmdstruct->lastArgument = argc;
}

int main(void) {
    char cmdline[CMDLINE_MAX];
    pid_t bg_pid = -1;
    bool is_background = false;

    while (1) {
        struct command cmd;
        char *nl;
        is_background = false;

        printf("sshell$ ");
        fflush(stdout);

        if (!fgets(cmdline, CMDLINE_MAX, stdin)) {
            strncpy(cmdline, "exit\n", CMDLINE_MAX);
        }

        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmdline);
            fflush(stdout);
        }

        nl = strchr(cmdline, '\n');
        if (nl) *nl = '\0';

        if (strchr(cmdline, '&')) {
            is_background = true;
            *strchr(cmdline, '&') = '\0';
        }

        cmdfunc(cmdline, &cmd);
        if (cmd.argv[0] == NULL) continue;

        if (strcmp(cmd.argv[0], "exit") == 0) {
            if (bg_pid > 0) {
                int status;
                waitpid(bg_pid, &status, 0);
                fprintf(stderr, "+ completed '%s &' [%d]\n", cmd.cmdlinestr, WEXITSTATUS(status));
            }
            fprintf(stderr, "Bye...\n");
            break;
        }

        if (strcmp(cmd.argv[0], "cd") == 0) {
            chdir(cmd.argv[1]);
            fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
            continue;
        }

        if (strcmp(cmd.argv[0], "pwd") == 0) {
            char cwd[CMDLINE_MAX];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
            fprintf(stderr, "+ completed '%s' [0]\n", cmd.cmdlinestr);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (cmd.infile) {
                int fdin = open(cmd.infile, O_RDONLY);
                if (fdin < 0) { perror("open infile"); exit(EXIT_FAILURE); }
                dup2(fdin, STDIN_FILENO);
                close(fdin);
            }

            if (cmd.outfile) {
                int fdout = open(cmd.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fdout < 0) { perror("open outfile"); exit(EXIT_FAILURE); }
                dup2(fdout, STDOUT_FILENO);
                close(fdout);
            }

            execvp(cmd.argv[0], cmd.argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            if (is_background) {
                bg_pid = pid;
                continue;
            }
            int status;
            waitpid(pid, &status, 0);
            fprintf(stderr, "+ completed '%s' [%d]\n", cmd.cmdlinestr, WEXITSTATUS(status));
        } else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
