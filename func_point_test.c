#include "io.c"

int func(data) char* data;
{
	printf("func()\n");
	printf("data: %s\n", data);
	return 7;
}


void func_over_data(func, data) int (*func)(); char* data;
{
	int ret_val;
	printf("func_over_data()\n");
	ret_val = func(data);
	printf("func() returned: %d\n", ret_val);
}

void main()
{
	char* str = "TEST STRING 123456789 La la la la la";

	printf("main()\n");
	printf("TESTING FUNC OVER DATA\n");
	func_over_data(func, str);
}