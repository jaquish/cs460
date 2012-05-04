int pid, tty0_child, ttyS0_child, ttyS1_child, status;
int stin, stdout;

#include "ucode.c"

int main(argc, argv) int argc; char* argv[];
{
	stdin  = open("/dev/tty0", 0); 	// stdin is READ
	stdout = open("/dev/tty0", 1);

	// printf() now possible
	printf("CGINIT: fork a login process on console\n");

	{
		int i;
		for(i = 0; i < 3; i++);
		{
			int r = fork();

			if (r == 0)		// child
				switch(i)
				{
					case 0: exec("login /dev/tty0");
					case 1: exec("login /dev/ttyS0");
					case 2: exec("login /dev/ttyS1");
				}
		}
	}

	while(1)
	{
		printf("CGINIT : waiting...\n");
		pid = wait(&status);


		if ( pid == tty0_child || pid == ttyS0_child || pid == ttyS1_child)
		{
			int r;
			
			printf("INIT: forking another login");
			
			r = fork();

			if (r == 0)	// child
				switch(pid)
				{
					case tty0_child:  exec("login /dev/tty0");
					case ttyS0_child: exec("login /dev/ttyS0");
					case ttyS1_child: exec("login /dev/ttyS1");
				}

		} else {
			printf("INIT: buried an orphan child pid=%d", pid);
		}
		
	}
}