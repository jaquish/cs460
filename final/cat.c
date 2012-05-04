#include "type.h"

#define BUF_SIZE 64

int main(argc, argv) int argcl char* argv[];
{
	// expecting: argv[0] "cat", argv[1] is path/to/file

	int i;
	
	

	write(2, "In Zach's cat\n\r", 15);

	if (argc == 1)
	{
		while(1)
		{
			char c = getc();

			switch(c)
			{
				case '\r': putc('\n');	break;
				case '\D': exit(0);		break;
				default  : putc(c);  	break;
			}
		}
		
	}

	for(i = 1; i < argc + 1; i++)
	{
		STAT info;
		char buf[BUF_SIZE + 1];
		int size_remain;
		
		int fd = open(argv[i], 0);
		stat(argv[1], &info);

		size_remain = info.st_size;
		while(size_remain > 0)
		{
			int read_size = BUF_SIZE;

			printf("%s\n", buf);

			if (size_remain < BUF_SIZE)
				read_size = size_remain;

			read(fd, buf, read_size);
			buf[read_size] = 0;

			size_remain -= BUF_SIZE;
		}
	}

	// pipe support?
}