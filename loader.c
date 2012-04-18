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

char load(pathname, segment) char *pathname; u16 segment;
{

	// SUPER BLOCK PRINT
	// GROUP BLOCK PRINT
	

	char* p1 = pathname;
	char* p2 = p1;
	int str_len;

	print_super();
	print_group();

	while(1)
	{
		str_len = 0;

		while( *p2 != '\0' && *p2 != '/' )
		{
			p2++;
			str_len++;
		}

		if (str_len == 0)
			break;


	}
	printf("success\n");
}

/*
//    {
//       1. break up pathname into tokens:
//          /a/b/c/d  ==> "a", "b", "c", "d" with n=4
 
//       2. locate the inode of /a/b/c/d: let ip->INODE of file 
//          return 0 if can't find the file

//       3. read file header to get size information;
//          print tsize, dsize, bsize

//       3. load a.out data blocks (with header) into segment.
//          move the executable part (|code|dat|....) upward 32 bytes to
//          eliminate the header.

//       4. Clear bss section (after |code|data|) to 0.

//       5. return 1 for success.


///***********************************
///        COPIED FROM BOOTLOADER 
///***********************************
/*

// load the block containing that inode# into the inode_buf, return pointer to that inode
e2_inode* load_inode(i) int i;
{
    i--;
    // calculate correct block
    get_block( ( i / 8 ) + INODE_B1 , &inode_buf);
    return &inode_buf[i % 8];
}

// pathleft = remaining filepath string, inode = inode of directory
int find_file(pathleft, inode_n) char* pathleft; int inode_n;
{
    int i;
    int blockleft;
    e2_inode* inode;
    
    // pop off next directory/file from pathleft string into nextdir
    char* cp_next = nextdir;
    
    pathleft++; // get past '/'
    
    while( (*pathleft != '/') && (*pathleft != '\0') ) {
        *cp_next = *pathleft;
        cp_next++;
        pathleft++;
    }
    *cp_next = '\0';
    
    inode = load_inode(inode_n);
        
    for (i = 0; i < 13; i++)
    {
        dir_entry* dir; // dir entry pointer
        
        // check that block exists
        int datablock = inode->i_block[i];
        
        if (datablock == 0) {
            error();
        }
            
        // load block
        get_block(datablock, &data_buf);
        
        // get pointers to iterate over dir_entries
        dir = (dir_entry*)data_buf;
        
        // iterate 
        blockleft = BLOCK_SIZE;
        while(blockleft > 0) {
            
            // compare name to nextdir
            char* sp = dir->name;
            char* np = nextdir;
            
            // NEED TO TRACK ELAPSED LENGTH
            int size = dir->name_len;
            //prints("filename:");
            
            while (*sp == *np && dir->name_len > 0) {
                if (*(np+1) == '\0') {  // both at end, match made
                    
                    if (pathleft[0] == '\0') {
                        //prints("found!\n\r");
                        return dir->inode;
                        
                    } else {
                        return find_file(pathleft, dir->inode);
                    }
                }
                sp++;
                np++;
            }
                        
            // increment
            blockleft -= dir->rec_len;
            dir = &data_buf[BLOCK_SIZE-blockleft];
        }
        
        // wasn't in that block, try the next
    }
    return 0;
}


int main()
{   
    e2_inode* inode;
    int i;
    long* lp;

    prints("file:");
    my_strcpy(filename, my_gets());
    
    // set to default if user doesn't enter string
     
    if (filename[0] == '\0') {
        my_strcpy(filename,"/boot/mtx");
    }
    
    // get inode of file requested
    inode = load_inode(find_file(filename, ROOT_INDEX));
    
    i = inode->i_block[12];
    
    // load indirect block before messing with segments
    if (i != 0) {
        get_block(i, indirect);
    }
    
    setes(KERNEL_ADDR);  // start loading here
    
    lp = inode->i_block;
    for (i = 0; i < 12; i++) {
        get_block(lp[i], 0);
        inces();
    }
    
    lp = indirect;
    while (*lp != 0) {
        //putc('Z');
        get_block(*lp, 0);
        inces();
        lp++;
    }
    
    
    
    return 1;
}

*/