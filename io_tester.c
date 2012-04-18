#include "io.c"

main()
{
	u32 a = 100555;
	u32 b = 200251;
	u32 c = 300676;
	u32 d = 400928;
	u32 e = 500123;
	u32 f = 600222;

	printf("****************************\n");
	printf("    I/O FUNCTION TESTING    \n");
	printf("****************************\n");

	printf("print using prints()\n");
	printf("expected: teststring\n");
	printf("%s\n\n", "teststring");

	printf("print char\n");
	printf("expected: h * \" # @\n");
	printf("%c %c %c %c %c\n\n", 'h', '*', '\"', '#', '@');

	printf("print unsigned 16-bit integer printu()\n");
	printf("expected: 0 45 65123 0(negative #)\n");
	printf("%u %u %u %u\n\n", 0, 45, 65123, -5);

	printf("print signed 16-bit integer printi()\n");
	printf("expected: 0 321 -32755 -4\n");
	printf("%d %d %d %d\n\n", 0, 321, -32755, -4);

	printf("ZZZ\n");

	

	printf("print unsigned 32-bit integer printl()\n");
	printf("expected: 100555 200251 300676 400928 500123 600222\n");
	printf("%l%l%l%l%l%l\n\n", a,b,c,d,e,f);

	printf("print unsigned 16-bit hexadecimal printx()\n");
	printf("expected: 0xABCD\n");
	printf("%x\n\n", 0xABCD);

	printf("print unsigned 32-bit hexadecimal printX()\n");
	printf("expected: 0xABCDEF12\n");
	printf("%X\n\n", 0xABCDEF12);
}