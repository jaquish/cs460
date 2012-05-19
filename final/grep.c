#include "type.h"
#include "ucode.c"

main(argc, argv) int argc; char* argv[];
{
	STAT info;
	
	int size_remain;
	char charVar;

	int fd = open(argv[2], 0);
	int pattern_len = strlen(argv[1]);

	printf("=== zjaquish grep ===\n");

	if (argc != 3)
	{
		printf("grep pattern filename\n");
		exit(-1);
	}

	stat(argv[2], &info);

	size_remain = info.st_size;

	printf("grep pattern=%s len=%d\n", argv[1], pattern_len);
	while(1)
	{
		char line[129];
		int i;

		// load buffer
		for(i = 0; i < 128; i++)
		{
			read(fd, &line[i], 1);	// get a char
			size_remain--;

			if (line[i] == '\n' || size_remain == 0)
			{
				line[i+1] = '\0';
				break;
			}
		}

		// bug: if line is longer than 128 chars..., undefined behavior

		// search buffer
		{
			char* cp = line;
			while(*cp != '\n')
			{
				// printf("cp=%s argv[1]=%s pattern_len=%d\n", cp, argv[1], pattern_len);
				if(strncmp(cp, argv[1], pattern_len) == 0)
				{
					printf("%s\n\r", line);
					break;
				}
				cp++;
			}
		}

		if (size_remain == 0) {
			close(fd);
			exit(0);
		}
	}
}