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

// 
void func_over_data(func, inode_n) int (*func)(); u32 inode_n; 
{
	struct ext2_inode inode_buf[8];
	struct ext2_inode* inode;
	u32 bytes_left;
	int i, j;	// counter
	char data_buf[1024];

	// 1) Load the inode 
	inode_n--;	// inodes count from 1
	get_block( ( inode_n / 8 ) + FIRST_INODE_BLOCK , &inode_buf);
    inode = &inode_buf[inode_n % 8];
	bytes_left = inode->i_size;

	for(i = 0; i < 4; i++)	// four loops: for direct, indirect, 2x indirect, 3x indirect
	{
		int d_blocks = 12;
		int ind_blocks = 0;
		int ind2_blocks = 0;
		int ind3_blocks = 0;

		// pointers to lists of (in)direct blocks
		u32 *d_list    = &inode->i_block[0];
		u32 *ind_list  = &inode->i_block[12];
		u32 *ind2_list = &inode->i_block[13];
		u32 *ind3_list = &inode->i_block[14];

		// 4K of buffers....
		char d_buffer[1024];
		u32 ind_buffer[256];
		u32 ind2_buffer[256];
		u32 ind3_buffer[256];

		int d, i, i2, i3;	// loop counters

		switch(i) {
			default:
				break;

			case 1:
				ind_blocks = 1;
				ind_list = &inode->i_block[12];
				d_blocks = 256;
				break;

			case 2:
				ind2_blocks = 1;
				ind2_list  = &inode->i_block[13];
				ind_blocks = 256;
				break;

			case 3:
				ind3_blocks = 1;
				ind3_list   = &inode->i_block[14];
				ind2_blocks = 256;
				break;
		}

		for (i3 = 0; i3 < ind3_blocks; i3++)
		{
			// LOAD i3 block into buffer
			get_block(*ind3_list, ind3_buffer);
			ind2_list = ind3_buffer;

			for (i2 = 0; i2 < ind2_blocks; i2++)
			{
				get_block(ind2_list[i2], ind2_buffer);
				ind_list = ind2_buffer;

				for (i = 0; i < ind_blocks; i++)
				{
					get_block(ind_list[i], ind_buffer);
					d_list = ind_buffer;

					for(d = 0; d < d_blocks; d++)
					{
						u16 valid_bytes = 1024;
						if (bytes_left < valid_bytes) 
							valid_bytes = bytes_left;

						func(ind_buffer[i], valid_bytes);

						if(func(d_buffer, valid_bytes) == 0)	// func can trigger early end, to stop unnecessary extra work
							return;

						bytes_left -= valid_bytes;
						if (bytes_left == 0)
							return;
					}
				}
			}
		}
	}
}



// THIS FUNCTION COMPLIES WITH FUNC_OVER_DATA
static char* search_str;

u32 search(block_n, block_bytes) u32 block_n; u16 block_bytes;
{
	// NOTE: assumes that directory files only use direct blocks

    // search for name string in the data blocks of this INODE
    // if found, return name's inumber
    // else      return 0

	char data_buf[1024];
	char* d;	// use a char* so we can move it in 1-byte increments, then do dir = d;
	struct ext2_dir_entry_2 *dir = d;

	get_block(block_n, data_buf);

	while(block_bytes)	// iterate over dir entries
	{
		if (dir->name_len == str_len(search_str) && strncmp(dir->name, search_str, dir->name_len))
		{
			// Found it
			return 0;	// tell func_over_data() to stop
		}

		block_bytes -= dir->rec_len;
		d += dir->rec_len;
		dir = (struct ext2_dir_entry_2 *)d;
	}

	return 1;	// tell func_over_data() to continue
}

char load(pathname, segment) char *pathname; u16 segment;
{
	char* p1 = pathname;
	char* p2 = p1;

	int str_len;
	u32 next_inode;

	if (*p1 == '/')	// if absolute path
	{
		next_inode = ROOT_INODE;
		p1++;
		p2++;

	} else {
		printf("FAIL– RELATIVE PATH\n");
		exit(1);
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

			func_over_data(search, next_inode);
			
			// TODO: check for failure

			p2++;
			p1 = p2;
		}
	}

	// ok, we have the inode


	printf("success\n");
}