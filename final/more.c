#include "type.h"
#include "ucode.c"


/*
	Interactive Commands
	----------------------
 	space   - print 1 page
 	enter   - print 1 line
 	any key - print 1 char
 */

int main(argc, argv) int argc; char* argv[];
{
	STAT info;
	char line[128];
	int size_remain;
	char tempc;
	char charVar;

	int fd = open(argv[1], 0);

	printf("zjaquish more\n");

	if (argc != 2)
	{
		printf("usage: more filename\n");
		exit(1);
	}

	stat(argv[1], &info);

	size_remain = info.st_size;

	printf("got file with fd=%d size_remain=%d\n", fd, size_remain);

	if (size_remain == 0)
		exit(0);

	while(1)
	{
		int h;

		// print a page, assume 24 high, 80 wide
		printpage:

		for(h = 0; h < 24; h++)
		{
			int w;
			for(w = 0; w < 80; w++)
			{
				char c;

				read(fd, &c, 1);
				putc(c);

				size_remain--;

				if (size_remain == 0)
					exit(0);

				if (c == '\n' || c == '\r')
					break;	// end of line
			}
		}

		// get user input
		while(1) {
			char c = getc();
			switch(c)
			{
				case 'q':
					close(fd);
					exit(0);

				case '\r':
					// print a line
					{
						int w;
						for(w = 0; w < 80; w++)
						{
							char c;

							read(fd, &c, 1);
							putc(c);

							size_remain--;

							if (size_remain == 0)
								exit(0);

							if (c == '\n' || c == '\r')
								break;	// end of line
						}
					}
					break;
					

				case ' ': goto printpage;	// go back to print another page

				default:
					read(fd, &c, 1);
					putc(c);
					size_remain--;
			}
		}
	}
}