#include "type.h"
#include "ucode.c"

#define BUF_SIZE 64

int main(argc, argv) int argc; char* argv[];
{
	// expecting: argv[0] "cat", argv[1] is path/to/file
	
	write(2, "=== zjaquish cat ===\n\r", 22);

	if (argc == 1)
	{
		// interactive mode
		while(1)
		{
			char c = getc();

			switch(c)
			{
				case '\r': putc('\n');  putc('\r');	break;
				case '~' : exit(0);		break;
				default  : putc(c);  	break;
			}
		}
	
	} else {
		int i;
		// can cat a list of files

		for(i = 1; i < argc; i++)
		{
			char buf[BUF_SIZE];
			STAT info;
			int bytes_left;
			
			int fd = open(argv[i], 0);

			printf("fd=%d\n", fd);

			stat(argv[1], &info);
			bytes_left = info.st_size;

			while(bytes_left > 0)
			{
				int j;
				int read_size = BUF_SIZE;

				if (bytes_left < BUF_SIZE)
					read_size = bytes_left;

				read(fd, buf, read_size);

				for(j = 0; j < read_size; j++)
				{
					char c = buf[j];
					switch(c)
					{
						case '\n': putc('\n');  putc('\r');	break;
						default  : putc(c);  	break;
					}
				}

				bytes_left -= read_size;
			}
			close(fd);
		}
		exit(0);
	}
}