#include  "type.h"
#include "ucode.c"

#define BUF_SIZE   64
#define LINE_SIZE  128

char* tty;

int main(argc, argv) int argc; char* argv[];
{
	char user_name[BUF_SIZE], password[BUF_SIZE], line[LINE_SIZE];
	char exec_line[BUF_SIZE];

	int fd;
	STAT info;
	int done;
	int uid, mult, iter;
	char *cp, *cptemp;

	tty = argv[1];

	// close 0, 1, 2 in case INIT had opened them
	close(0);
	close(1);
	close(2);

	open(tty, 0);	// stdin
	open(tty, 1);	// stdout
	open(tty, 1);	// stderr

	settty(tty);

	// printf() up and running
	printf("CGLOGIN: open %s as stdin, stdout, stderr\n", tty);
	signal(2,1);

	while(1)
	{
		printf("username: ");
		gets(user_name);
		
		printf("password: ");
		gets(password);

		// scan password file for a match
		fd = open("/etc/passwd", 0);
		stat("etc/passwd", &info);
		
		{
			int size_remain = info.st_size;
			done = 0;

			while(size_remain > 0 && !done)
			{
				// grab line
				char charVar = '\0';

				for(iter = 0; size_remain > 0 && charVar != '\n'; iter++)
				{
					read(fd, &charVar, 1);
					line[iter] = charVar;
					size_remain--;
				}

				line[iter] = 0;
				cp = strtok(line, ":");

				if (strcmp(cp, user_name) == 0)
				{
					cp = strtok(0, ":");
					if (strcmp(cp, password) == 0)
					{
						done = 1;
						cp = strtok(0, ":");	// ignore gid
						cp = strtok(0, ":");

						// a to i
						uid = 0;
						cptemp = cp;
						cptemp += strlen(cp) - 1;
						mult = 1;
						while(cptemp >= cp)
						{
							uid += mult * (*(cptemp--) - '0');
						}
						
						setuid(uid);
						cp = strtok(0, ":");	// ignore full name
						cp = strtok(0, ":");
						chdir(cp);
						cp = strtok(0, ":\n");

						strcpy(exec_line, "/bin/");
						strcat(exec_line, cp);
						exec(exec_line);
					} else {
						printf("Invalid password!\n");
					}
				}
			}

			printf("login failed, try again\n");
		}
	}
}