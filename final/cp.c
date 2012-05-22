#include "type.h"
#include "ucode.c"

#define BUF_SIZE 64

int main(argc, argv) int argc; char* argv[];
{
	printf("=== zjaquish cp ===")
	if (argc != 3)
	{
		printf("cp filename filename\n");
		exit(1);
	}

	if (strcmp(argv[1], argv[2]) == 0)
	{
		printf("can't copy file onto itself...\n");
		exit(2);
	}

	{
		int f2;
		STAT info;
		char cp_buf[BUF_SIZE];
		int bytes_left;

		// open source file
		int f1 = open(argv[1], 0);

		stat(argv[1], &info);

		if (info.st_size == 0)
		{
			printf("File %s doesn't exist\n", argv[1]);
			exit(3);
		}

		// open destination file
		creat(argv[2]);
		f2 = open(argv[2], 1);

		printf("f1=%s %d f2=%s %d\n", argv[1], f1, argv[2], f2);

		// printf("f1=%s %d f2=%s %d\n", argv[1], f1, argv[2], f2);
		bytes_left = info.st_size;

		while(bytes_left > 0)
		{
			int copy_size = BUF_SIZE;
			
			printf("bytes_left=%d\n", bytes_left);

			if (bytes_left < BUF_SIZE)
				copy_size = bytes_left;

			printf("read\n");
			read(f1, cp_buf, copy_size);
			printf("write\n");
			write(f2, cp_buf, copy_size);

			bytes_left -= copy_size;
		}

		close(f1);
		close(f2);
		exit(0);
	}
}