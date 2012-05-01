#include "my_types.h"
#include "ext2.h"
#include "print.c"
/*                         I/O Functions.                     */

u16 get_word(segment, offset) u16 segment; u16 offset;
{
	char left  = get_byte(segment, offset);
	char right = get_byte(segment, offset + 1);

  u16 merge = ((left << 8) & 0xF0) & right;

	return merge;
}

void put_word(word, segment, offset) u16 word; u16 segment; u16 offset;
{
  char left  = (word >> 8) & 0x0F;
  char right = word & 0x0F;

  put_byte(left , segment, offset);
  put_byte(right, segment, offset + 1);
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

struct ext2_inode* get_inode(inode_n, buf) u16 inode_n; struct ext2_inode* buf;
{
  inode_n--;  // inodes count from 1

  get_block( ( inode_n / 8 ) + FIRST_INODE_BLOCK , buf);
  return &buf[inode_n % 8];
}
