#include "type.h"

#define BUF_SIZE 64

int main(argc, argv) int argc; char* argv[];
{
	int fd1, fd2;
	STAT info;
	char buf[BUF_SIZE + 1];
	int size_remain;

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

	fd1 = open(argv[1], 0);
	stat(argv[1], &info);
	if (info.st_size == 0)
	{
		printf("File %s doesn't exist\n", argv[1]);
		exit(3);
	}

	creat(argv[2]);
	fd2 = open(argv[2], 1);

	size_remain = info.st_size;
	while(size_remain > 0)
	{
		int copy_size = BUF_SIZE;
		
		if (size_remain < BUF_SIZE)
			copy_size = size_remain;

		read(fd1, buf, copy_size);
		write(fd2, buf, BUF_SIZE);

		buf[copy_size] = 0;
		size_remain -= BUF_SIZE;
	}

	close(fd1);
	close(fd2);
}