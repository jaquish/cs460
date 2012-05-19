#include "type.h"
#include "ucode.c"

char*tty;

int stdin, stdout, stderr;

int tokenize(path, tokens, delim) char* path; char* tokens[]; char delim;
{
	int index = 1;
	tokens[0] = path;
	while(*path)
	{
		if (*path == delim)
		{
			*path = 0;
			tokens[index++] = ++path;
		} else {
			path++;
		}
	}
	return index;
}

main (argc, argv) int argc; char* argv[];
{
	char usrname[64], password[64], passbuf[1024];
	char name[64], home[64], shell[64];
	int gid, uid;

	char* tokens[10];
	char delim = ':';
	char* tty = argv[1];

	close(0);
	close(1);
	close(2);

	stdin = open(tty, 0);
	stdout = open(tty, 1);
	stderr = open(tty, 2);

	printf("fd's for stdin=%d stdout=%d stderr=%d\n", stdin, stdout, stderr);

	while(1)
	{
		STAT s;
		int size_remain;
		int passfd = open("/etc/passwd", READ);

		printf("zjaquish login: ");
		gets(usrname);
		
		printf("password: ");
		gets(password);

		stat("/etc/passwd", &s);
		size_remain = s.st_size;

		printf("size_remain=%d\n", size_remain);

		while(1)
		{
			int i;
			for (i = 0; i < 1024 && size_remain > 0; i++)
			{
				read(passfd, &passbuf[i], 1);
				size_remain--;

				if (passbuf[i] == '\n') {
					passbuf[i+1] = '\0';
					break;
				}
			}

			tokenize(passbuf, tokens, ':');
			if((strcmp(usrname, tokens[0]) == 0 && strcmp(password, tokens[1]) == 0))
			{
				gid = atoi(tokens[2]);
				uid = atoi(tokens[3]);
				strcpy(name, tokens[4]);
				strcpy(home, tokens[5]);
				strcpy(shell, tokens[6]);
				printf("%s %s %d %d %s %s %s \n", usrname, password, gid, uid, name, home, shell);
				chdir(home);

				//strcpy(shell, "/bin/");
				//strcat(shell, tokens[6]);

				exec("/bin/sh");
			} else {
				printf("user was %s with pw %s \n", tokens[0], tokens[1]);
			}

			if (size_remain == 0)
				break;
		}
		close(passfd);
	}
	
}