void print_super()
{
	char buffer[1024];

	struct ext2_super_block* super = buffer;

	get_block(1,buffer);

	printf("*******************************\n");
	printf("          Super Block          \n");
	printf("*******************************\n");
	printf("s_inodes_count: %l\n", super->s_inodes_count);
	printf("s_blocks_count: %l\n", super->s_blocks_count);
	printf("s_free_blocks_count: %l\n", super->s_free_blocks_count);
	printf("s_free_inodes_count: %l\n", super->s_free_inodes_count);
	printf("s_first_data_block: %l\n", super->s_first_data_block);
	printf("s_magic (should be EF53): %x\n", super->s_magic);
	printf("s_first_ino: %l\n\n", super->s_first_ino);
}

void print_group()
{
	char buffer[1024];

	struct ext2_group_desc* group = buffer;

	get_block(2,buffer);

	printf("*******************************\n");
	printf("          Group Block          \n");
	printf("*******************************\n");	
	printf("bg_block_bitmap: %l\n", group->bg_block_bitmap);
	printf("bg_inode_bitmap: %l\n", group->bg_inode_bitmap);
	printf("bg_inode_table: %l\n", group->bg_inode_table);
	printf("bg_free_blocks_count: %u\n", group->bg_free_blocks_count);
	printf("bg_free_inodes_count: %u\n", group->bg_free_inodes_count);
	printf("bg_used_dirs_count: %u\n\n", group->bg_used_dirs_count);
}

void func_over_data(func, inode_n) int (*func)(); u16 inode_n;
{
	struct ext2_inode inode_buf[8];
	struct ext2_inode* inode;
	u32 bytes_left;
	int loop;	// counter
	char data_buf[1024];
	

	printf("func_over_data()\n");
	// 1) Load the inode

	inode = get_inode(inode_n, inode_buf);

	/*
	inode_n--;	// inodes count from 1

	get_block( ( inode_n / 8 ) + FIRST_INODE_BLOCK , inode_buf);

    inode = &inode_buf[inode_n % 8];
	*/

	bytes_left = inode->i_size;

	for(loop = 0; loop < 4; loop++)	// four loops: for direct, indirect, 2x indirect, 3x indirect
	{
		// pointers to lists of (in)direct blocks
		u32 *ind1  = &inode->i_block[0];	// point to an indirect  block, a list of direct blocks
		u32 *ind2  = &inode->i_block[12];	// point to an indirect2 block, a list of indirect blocks
		u32 *ind3  = &inode->i_block[13];	// point to an indirect3 block, a list of indirect2 blocks

		int ind1_size = 12;
		int ind2_size = 1;
		int ind3_size = 1;

		// 4K of buffers....
		u32 ind1_buffer[256];
		u32 ind2_buffer[256];
		u32 ind3_buffer[256];

		int i1, i2, i3;

		printf("func loop\n");

		switch(loop) {
			case 0:
				goto INDIRECT_1;

			case 1:
				ind1 = ind1_buffer;
				ind1_size = 256;

				goto INDIRECT_2;

			case 2:
				ind2 = ind2_buffer;
				ind2_size = 256;

				goto INDIRECT_3;

			case 3:
				ind3 = ind3_buffer;
				ind3_size = 256;
				// Fall through
		}


		// LOAD i3 block into buffer
		get_block(inode->i_block[14], ind3_buffer);

		INDIRECT_3:
		for (i3 = 0; i3 < ind3_size; i3++)
		{
			get_block(ind3[i3], ind2_buffer);

			INDIRECT_2:
			for (i2 = 0; i2 < ind2_size; i2++)
			{
				get_block(ind2[i2], ind1_buffer);

				INDIRECT_1:
				for(i1 = 0; i1 < ind1_size; i1++)
				{
					u16 valid_bytes = 1024;
					if (bytes_left < valid_bytes) 
						valid_bytes = bytes_left;

					printf("ind1[i1]=%u and valid_bytes=%u\n", (u16)ind1[i1], valid_bytes);

					if(func((u16)ind1[i1], valid_bytes) == 0)	// func can trigger early end, to stop unnecessary extra work
						return;

					bytes_left -= valid_bytes;
					if (bytes_left == 0)
						return;
				}
			}
		}
	}
}

// THIS FUNCTION COMPLIES WITH FUNC_OVER_DATA
static char* search_input;	// search str
static u16  search_output;	// search found inode number

u16 search(block_n, block_bytes) u16 block_n; u16 block_bytes;
{
	// NOTE: assumes that directory files only use direct blocks

    // search for name string in the data blocks of this INODE
    // if found, return name's inumber
    // else      return 0

	char data_buf[1024];
	char* d = data_buf;	// use a char* so we can move it in 1-byte increments, then do dir = d;
	struct ext2_dir_entry_2 *dir = d;

	printf("search() with input:%s block_n:%u and block_bytes:%d\n", search_input, block_n, block_bytes);

	get_block(block_n, data_buf);
	printf("got block\n");

	while(block_bytes)	// iterate over dir entries
	{
		int i;
		char* n = dir->name;
		printf("dir_entry:");
		for (i = 0; i < dir->name_len; ++i)
		{
			printf("%c", dir->name[i]);
		}
		printf(" has inode: %u\n", dir->inode);

		if ( dir->name_len == str_len(search_input) && (strncmp(dir->name, search_input, dir->name_len) == 0 ))
		{
			// found it
			search_output = dir->inode;
			return 0;	// tell func_over_data() to stop
		}

		block_bytes -= dir->rec_len;
		d += dir->rec_len;
		dir = d;
	}

	return 1;	// tell func_over_data() to continue if there is more data
}


typedef struct file_header {
	u32 space_type;		// 0 0x04100301 //(combined I,D space) or 0x04200301=(separate I,D space)
    u32 magic_type;    	// 1 0x00000020
    u32 text_size;		// 2 text size  // code size         
    u32 data_size;	    // 3 data size  // initialized data size
    u32 bss_size;	    // 4 bss  size  // uninitialized data size ==> cleared to 0
    u32 zero;		    // 5 0x00000000
    u32 total_mem;	    // 6 total memory      // ignore this field
    u32 symbol_size;    // 7 symbolTable size  // symbol Table size
} file_header;

void print_file_header(fh) file_header* fh;
{
	printf("*******************************\n");
	printf("          File Header          \n");
	printf("*******************************\n");
	
	if      (fh->space_type == 0x04100301)	printf("1 - combined I,D space\n");
	else if (fh->space_type == 0x04200301)  printf("1 - seperate I,D space\n");
	else									printf("1 - ERROR: no space type, got %X\n", (u32)fh->space_type);
	
	if (fh->magic_type == 0x00000020) {
		printf("2 - Magic type matches: %X\n", fh->magic_type);
	} else {
		printf("2 - ERROR: magic doesn't match\n");
	}

	printf("3 - text_size: %l\n", (u32)fh->text_size);
	printf("4 - data_size: %l\n", (u32)fh->data_size);
	printf("5 - bss_size: %l\n", (u32)fh->bss_size);

	if (fh->zero == 0x00000000) {
		printf("6 - (0x00000000)\n");
	} else {
		printf("6 - ERROR: expected zero value\n");
	}

	printf("7 - total_mem: %l\n", (u32)fh->total_mem);
	printf("8 - symbol_size: %l\n\n", (u32)fh->symbol_size);	
	return;
}

static u16 load_offset;
static u16 load_block_count;

u16 load_block(block_n, block_bytes) u16 block_n; u16 block_bytes;
{
	u16 segment, offset;
	char block_buf[1024];
	char* load_point = block_buf;

	get_block(block_n, block_buf);

	printf("load_block() with block_n:%u and block_bytes:%d\n", block_n, block_bytes);

	if (load_block_count == 0)
	{
		print_file_header(block_buf);
		load_point += 32;
		block_bytes -= 32;
	}

	setes(0x2000);
	{
		int i;
		for (i = 0; i < load_block_count; i++) {
			inces();
		}
			
	}
	get_block(block_n,0);

	setes(0x1000);
	/*
	while(block_bytes > 0) 
	{
		printf("block_bytes:%u\n", block_bytes);
		put_byte(*load_point, 0x2000, load_offset);
		load_point++;
		block_bytes--;
		load_offset++;
	}
	*/

	load_block_count++;
	return 1;
}


int load(pathname, segment) char *pathname; u16 segment;
{
	
	char* p1 = pathname;
	char* p2 = p1;

	int str_len;
	u16 next_inode;

	if (*p1 == '/')	// if absolute path
	{
		next_inode = ROOT_INODE;
		p1++;
		p2++;

	} else {
		printf("FAIL– RELATIVE PATH\n");
		return -1;
	}

	while(1)	// iterate over pieces of name
	{
		str_len = 0;

		while( *p2 != '\0' && *p2 != '/' )
		{
			p2++;
			str_len++;
		}

		if (str_len == 0){
			break;

		} else {
			*p2 = '\0';

			printf("filepiece is: %s \n", p1);

			search_input = p1;					// set func_over_data input
			search_output = 0;
			func_over_data(search, next_inode);
			
			// check for failure
			if (search_output != 0) {
				next_inode = search_output;
				printf("Next inode is: %u \n", next_inode);
			} else {
				printf("FAIL!\n");
				return -1;	// failure
			}

			p2++;
			p1 = p2;
		}
	}

	// ok, we have the inode
	{
		load_block_count = 0;
		load_offset = 0;
		func_over_data(load_block, next_inode);
	}

	printf("success\n");
	return 1;
}