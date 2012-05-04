#include "type.h"

int main(argc, argv) int argc; char* argv[];
{
	int fd;
	STAT info;
	char line[128];
	int size_remain, iter;
	char tempc;
	char charVar;

	printf("zjaquish more\n");

	if (argc != 2)
	{
		printf("usage: more filename\n");
		exit(1);
	}

	fd = open(argv[1], 0);
	stat(argv[1], &info);

	size_remain = info.st_size;
	while(size_remain > 0)
	{
		// get a line
		for(iter = 0; size_remain > 0 && charVar != '\n'; iter++)
		{
			read(fd, &charVar, 1);
			line[iter] = charVar;
			size_remain--;
		}

		if (line[iter] == '\n')
		{
			line[iter + 1] = 0;
		} else {
			line[iter] = 0;
		}

		// Wait for enter
		charVar = 0;
		while(charVar != '\r' && charVar != 'q')
			charVar = getc();
		if (charVar == 'q')
			exit(0);

		printf("%s", line);
	}
	close(fd);
}