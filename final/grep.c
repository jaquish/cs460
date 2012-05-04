#include "type.h"

main(argc, argv) int argc; char* argv[];
{
	int fd;
	STAT info;
	char line[128];
	int size_remain, iter;
	char tempc;
	char charVar;
	int pattern_len;

	printf("zjaquish grep\n");
	printf("argc: %d\n", argc);

	if (argc != 3)
	{
		printf("grep pattern filename\n");
		exit(-1);
	}

	fd = open(argv[2], 0);
	stat(argv[2], &info);
	pattern_len = strlen(argv[1]);

	size_remain = info.st_size;
	while(size_remain > 0)
	{
		for(iter = 0, charVar = 0; size_remain > 0 && charVar != '\n'; iter++)
		{
			read(fd, &charVar, 1);
			line[iter] = charVar;
			size_remain--;
		}

		if (line[iter] == '\n')
			line[iter + 1] == '\0';
		else
			line[iter] = 0;

		for(iter = 0; iter <= strlen(line) - pattern_len; iter++)
		{
			charVar = line[pattern_len + iter];
			line[pattern_len + iter] = 0;

			if (strcmp(argv[1], line + iter) == 0)
			{
				line[pattern_len + iter] = charVar;
				printf("%s\n", line);
				break;
			}

			line[pattern_len + iter] = charVar;
		}
	}

	close(fd);
}