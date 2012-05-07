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
	int passfd, gid, uid;

	char* tokens[10];
	char delim = ':';
	char* tty = argv[1];
	close(0);
	close(1);
	close(2);

	stdin = open(tty, 0);
	stdout = open(tty, 1);
	stderr = open(tty, 2);

	while(1)
	{
		printf("login: ");
		gets(usrname);
		printf("\n");
		
		printf("password: ");
		gets(password);

		passfd = open("/etc/passwd", 1);
		while(1)
		{
			int numRead = getline(passfd, passbuf, 1024);
			if (numRead == 0)
				break;

			tokenize(passbuf, tokens, delim);
			if((strcmp(usrname, tokens[0]) == 0 && strcmp(password, tokens[1]) == 0))
			{
				gid = atoi(tokens[2]);
				uid = atoi(tokens[3]);
				strcpy(name, tokens[4]);
				strcpy(home, tokens[5]);
				strcpy(shell, tokens[6]);
				printf("%s %s %d %d %s %s %s \n", usrname, password, gid, uid, name, home, shell);
				chdir(home);
				exec("sh");
			}
		}
		close(passfd);
	}
}