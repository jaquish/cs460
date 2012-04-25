#include "io.c"

int func(data1, data2) char* data1; char* data2;
{
	printf("func()\n");
	printf("data: %s\n", data1);
	printf("data2: %s\n", data2);
	return 7;
}


void func_over_data(func, data) int (*func)(); char* data;
{
	int ret_val;
	printf("func_over_data()\n");
	ret_val = func(data, "DATA 2");
	printf("func() returned: %d\n", ret_val);
}

void main()
{
	char* str = "TEST STRING 123456789 La la la la la";

	printf("main()\n");
	printf("TESTING FUNC OVER DATA\n");
	func_over_data(func, str);
}