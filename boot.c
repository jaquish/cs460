typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

//#include "print.h"

#define BLOCK_SIZE 1024
#define SUPER_BL   1
#define GROUP_BL   2
#define INODE_B1  5     // we know inodes start at block 5 from analysis of floppy
#define ROOT_INDEX 2    // the 2nd inode is the root one
#define EXT2_NAME_LEN 255

#define KERNEL_ADDR 0x1000

char indirect[1024];    // buffer for indirect block
char filename[255]; // global name storage
char tempbuf[255];
char nextdir[255];
char str[255];

typedef struct {
    //*************************************************************************
    unsigned short i_mode;          // will be explained in detail 
    u16  i_uid;                       // ownerID
    u32  i_size;                    // file size in bytes
    u32  i_atime;                   // time fields  
    u32  i_ctime;
    u32  i_mtime;
    u32  i_dtime;
    u16  i_gid;                     // groupID
    u16  i_links_count;             // link count
    u32  i_blocks;                  // IGNORE
    u32  i_flags;                   // IGNORE
    u32  i_reserved1;               // IGNORE
    u32  i_block[15];               // See details below
    //**************************************************************************
    
    // IGNORE THE FOLLOWING
    u32  i_version;
    u32  i_file_acl;
    u32  i_dir_acl;
    u32  i_faddr;
    u8   i_frag;
    u8   i_fsize;
    u16  i_pad1;
    u32  i_reserved2[2];
    
} e2_inode;

typedef struct  {
u32  inode;        // Inode number; count from 1, NOT from 0
u16  rec_len;      // This entry length in bytes
u8  name_len;     // Name length in bytes
u8  file_type;    // for future use
char   name[EXT2_NAME_LEN];  // File name: 1-255 chars, no NULL byte
} dir_entry;

e2_inode inode_buf[8];
char data_buf[BLOCK_SIZE];


// copy string
void my_strcpy(dst, src) char *dst; char *src;
{
    while(*dst++ = *src++);
}

// get a string
char* my_gets()
{
    char* p = str;
    
    while(1) {
        *p = getc();
        putc(*p);
        
        if (*p == '\r') {
            *p = '\0'; // change to null, keyboard strings end in '\r'
            break;
        }
        
        p++;
    }
    return str;
}

void prints(s) char* s;
{
    while(*s != '\0') {
        putc(*s);
        s++;
    }
}

void get_block(blk, buf) int blk; char buf[];
{
    /*
    //convert blk to (cyl,head,sector);
    u16 cyl = blk / 18;
    u16 head = (blk / 9) % 2;
    u16 sector = (blk * 2) % 18 ;
     */

    diskr(blk / 18, (blk / 9) % 2, (blk * 2) % 18, buf);
}

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
