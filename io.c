// printf should use putc, prints()?
int printf(fmt) void *fmt;
{
	putc('P');
	putc('R');
	putc('I');
	putc('N');
	putc('T');
	putc('F');
	putc('!');
	putc('\n');

	return 7;
}

gets(buf) char* buf;
{
	putc('g');
	putc('e');
	putc('t');
	putc('s');
	putc('\n');
}

unsigned short get_word(segment, offset) unsigned short segment; unsigned short offset;
{
	putc('G');
	putc('W');
	putc('\n');
	return 1;
}

int    put_word(word, segment, offset) unsigned short word; unsigned short segment; unsigned short offset;
{
	putc('P');
	putc('W');
	putc('\n');
	return 1;
}
