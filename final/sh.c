char cmdLine[64];
char inShell[] = "?		help	pwd		cd 		logout";

#include "ucode.c"

char* name[20];

void sh_token(line) char* line;
{
	int i;
	
	name[0] = strtok(line, " ");
	
	for(i = 0; name[i - 1] != 0; i++)
	{
		name[i] = strtok(0, " ");
	}
}

int menu()
{
	printf("################################ MENU ################################## \n");
	printf("# ls		cd 		pwd		cat		more	cp		mv		>		>> # \n");
	printf("# mkdir		rmdir	creat	rm 		chmod 	chown 	lpr 	< 		|  # \n");
	printf("######################################################################## \n");
}

main (argc, argv) int argc; int argc; char* argv[];
{
	int stdin, stdout, stderr;
	int pid, cid, me, status;
	int i, j;

	int start;
	char temp_line[64];	// char long_name[128];
	char string[64];
	char* cp;
	STAT temp_stat;
	int pipes[2];

	stdin  = 0;
	stdout = 1;
	stderr = 2;

	me = getpid();

	printf("Enter ? for help menu\n");

	while(1)
	{
		printf("input command : ");
		gets(cmdLine);
		if (cmdLine[0] == 0)
			continue;

		strcpy(temp_line, cmdLine);
		sh_token(temp_line);

		// search inShell
		// int r = search(name[0]);
		switch(r)
		{
			case -1:
			case 0:
				menu();
				continue;

			case 1:
				getcwd(string);
				printf("%s\n", string);
				break;

			case 2:
				chdir(name[1]); 
				break;

			case 3:
				exit(0); 
		}

		// not a shell command, try to exec() it
		pid = fork();

		if (pid)
		{
			// parent
			printf("parent sh waiting for child to die...\n");
			pid = wait(&status);

		} else {
			// child
			int i;
			cp = cmdLine;
			for (i = 0, start = 0; name[i] != 0; i++)
			{
				// upgraded to case
				switch (name[i][0])
				{
					case '>':
						if(name[i][1] == '>')
						{
							close(1);
							creat(name[i+1]);
							open(name[++i], 3);

						} else {
							creat(name[i+1]);
							close(1);
							if (open(name[++i], 1) != stdout)
							{
								write(2, "Failed to create file\n", 23);
								exit(0);
							}
						}
						break;

					case '<':
						stat(name[i + 1], &temp_stat);
						if (temp_stat.st_size == 0)
						{
							printf("%s doesn't exist\n", name[i+1]);
							exit(0);
						}
						close(0);
						open(name[++i], 0);
						break;

					case '|':
						pipe(pipes);
						pid = fork();
						if (pid)
						{
							name[i + 1] = '\0';
							close(pipes[0]);
							close(1);
							dup2(pipes[1], 1);

						} else {
							cp = cmdLine;
							close(pipes[1]);
							close(0);
							dup2(pipes[0], 0);

						}
						break;

					default:
						strcpy(cp, name[i]);
						cp += strlen(name[i]);
						*(cp++) = ' ';
				}
			}

			*(cp-1) = 0;

			/* DEBUG CODE */
			printf("test\n");

			write(2, "cmdLine: -", 10);
			write(2, cmdLine, strlen(cmdLine));
			write(2, "-\n\r", 2);

			printf("child process %d exec to %s\n", getpid(), name[0]);
			exec(cmdLine);
			printf("exec() failed\n");
			exit(1);
		}
	}
}