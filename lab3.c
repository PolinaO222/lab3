#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFSIZE 512

int main(void)
{
	char str[BUFSIZE];
	char username[BUFSIZE];
	char hostname[BUFSIZE];
	char myname[BUFSIZE];
	char tempdir[BUFSIZE];
	pid_t pid;
	pid_t child_pid;
	int i, arg_n, backgr, len;

	/* strcpy(str, "\033[H\033[J"); */
	/* write(1, str,strlen(str)); */
	strcpy(str, "\e[1mYou are in the terminal, trust me. ");
	write(1, str, strlen(str));
	strcpy(str, "But you can use \"exit\" anytime.\n");
	write(1, str, strlen(str));

	gethostname(hostname, BUFSIZE);

	while (1 == 1) {
		for (i = 0; i < BUFSIZE; i++) {
			str[i] = '\0';
			username[i] = '\0';
			myname[i] = '\0';
			tempdir[i] = '\0';
		}
		getcwd(str, BUFSIZE);
		if (strstr(str, "/home/") != NULL) {
			strcpy(username, str+6);
			strcpy(str, strstr(username, "/")+1);
			len = strlen(username) - strlen(str) - 1;
			strcpy(tempdir, "~/");
			strcat(tempdir, str);
			strcpy(str, username);
			strcpy(username, "");
			strncat(username, str, len);
		} else
			strcpy(tempdir, str);

		strcpy(myname, "\e[32m\e[1m");
		strcat(myname, username);
		strcat(myname, "@");
		strcat(myname, hostname);
		strcat(myname, "\e[0m\e[1m:");
		write(1, myname, strlen(myname));
		strcpy(str, "\e[34m\e[1m");
		strcat(str, tempdir);
		strcat(str, "\e[0m$ ");
		write(1, str, strlen(str));
		for (i = 0; i < BUFSIZE; i++)
			str[i] = '\0';

		i = read(0, str, BUFSIZE);
		if (i == 0 || strcmp(str, "\n") == 0)
			continue;
		if (strcmp(str, "exit\n") == 0)
			break;

		for (i = strlen(str)-1; i >= 0; i--) {
			if (isalnum(str[i]) || ispunct(str[i]))
				break;
			if (str[i] == ' ' || str[i] == '\n')
				str[i] = '\0';
		}

		arg_n = 0;
		i = 0;
		while (str[i] != '\0') {
			if (str[i] == ' ')
				arg_n++;
			i++;
		}
		char *args[arg_n+2];

		args[0] = strtok(str, " ");
		i = 1;
		while (args[i-1] != NULL) {
			args[i] = strtok(NULL, " ");
			i++;
		}

		if (strcmp(args[0], "cd") == 0) {
			if (chdir(args[1]) == 0)
				continue;
			else
				perror("Error");
		}

		backgr = 0;
		if (strcmp(args[arg_n], "&") == 0) {
			backgr = 1;
			args[arg_n] = NULL;
		}

		pid = fork();
		if (pid == -1) {
			perror(NULL);
			break;
		} else if (pid == 0) {
			if (execvp(args[0], args) == -1) {
				perror("Error");
				exit(1);
			}
		} else {
			if (backgr != 1) {
				errno = 0;
				/*do
					child_pid = wait(&i);
				while (errno != ECHILD);*/
				waitpid(pid, &i, 0);
			} else
				backgr = 0;
		}
	}
	/* strcpy(str, "\033[H\033[J"); */
	/* write(1, str,strlen(str)); */
	exit(0);
}
