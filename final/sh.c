#include "type.h"

char cmdLine[64];

char *inShell[]={"?", "help", "pwd", "cd", "logout", 0};

int find_cmd(name) char *name;
{
   int i=0;   
   char *p=inShell[0];

   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;  
         p = inShell[i];
   } 
   return(-1);
}

#include "ucode.c"

char* name_read[20];

void sh_token(line) char* line;
{
	int i;
	
	name_read[0] = strtok(line, " ");
	
	for(i = 0; name_read[i - 1] != 0; i++)
	{
		name_read[i] = strtok(0, " ");
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
	char temp_line[64];	// char long_name_read[128];
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
		int r;

		printf("input command : ");
		gets(cmdLine);
		printf("cmdLine=%s\n", cmdLine);
		if (cmdLine[0] == 0)
			continue;

		strcpy(temp_line, cmdLine);
		sh_token(temp_line);

		// search inShell
		r = find_cmd(name_read[0]);
		switch(r)
		{
			case 0:
			case 1:
				menu();
				continue;

			case 2:
				getcwd(string);
				printf("%s\n", string);
				break;

			case 3:
				chdir(name_read[1]); 
				break;

			case 4:
				exit(0);

			default:
				break;
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

			printf("cmdLine=%s\n", cmdLine);

			for (i = 0, start = 0; name_read[i] != 0; i++)
			{
				// upgraded to case
				switch (name_read[i][0])
				{
					case '>':
						if(name_read[i][1] == '>')
						{
							close(1);
							creat(name_read[i+1]);
							open(name_read[++i], 3);

						} else {
							creat(name_read[i+1]);
							close(1);
							if (open(name_read[++i], 1) != stdout)
							{
								write(2, "Failed to create file\n", 23);
								exit(0);
							}
						}
						break;

					case '<':
						stat(name_read[i + 1], &temp_stat);
						if (temp_stat.st_size == 0)
						{
							printf("%s doesn't exist\n", name_read[i+1]);
							exit(0);
						}
						close(0);
						open(name_read[++i], 0);
						break;

					case '|':
						pipe(pipes);
						pid = fork();
						if (pid)
						{
							name_read[i + 1] = '\0';
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
						strcpy(cp, name_read[i]);
						cp += strlen(name_read[i]);
						*(cp++) = ' ';
				}
			}

			*(cp-1) = 0;

			/* DEBUG CODE */
			write(2, "cmdLine: -", 10);
			write(2, cmdLine, strlen(cmdLine));
			write(2, "-\n\r", 2);

			printf("child process %d exec to %s\n", getpid(), name_read[0]);
			printf("cmdLine=%s\n", cmdLine);
			exec(cmdLine);
			printf("exec() failed\n");
			exit(1);
		}
	}
}