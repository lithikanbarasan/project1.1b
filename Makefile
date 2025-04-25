CC      = gcc
CFLAGS  = -g -Wall -Wextra -Werror
RM      = rm -f


default: all

all: sshell

sshell: sshell.c
	$(CC) $(CFLAGS) -o sshell sshell.c

clean Clean:
	$(RM) sshell
