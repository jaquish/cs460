#ifndef EXT2_H
#define EXT2_H

#include "my_types.h"

/*
struct ext2_super_block 
{
  	u32  s_inodes_count;       // total number of inodes
  	u32  s_blocks_count;       // total number of blocks
  	u32  s_r_blocks_count;     
  	u32  s_free_blocks_count;  // current number of free blocks
  	u32  s_free_inodes_count;  // current number of free inodes 
  	u32  s_first_data_block;   // first data block in this group
  	u32  s_log_block_size;     // 0 for 1KB block size
  	u32  s_log_frag_size;
  	u32  s_blocks_per_group;   // 8192 blocks per group 
  	u32  s_frags_per_group;
  	u32  s_inodes_per_group;    
  	u32  s_mtime;
  	u32  s_wtime;
  	u16  s_mnt_count;          // number of times mounted 
  	u16  s_max_mnt_count;      // mount limit
  	u16  s_magic;              // 0xEF53

	u16	s_state;		// File system state 
	u16	s_errors;		// Behaviour when detecting errors 
	u16	s_minor_rev_level; 	// minor revision level 
	u32	s_lastcheck;		// time of last check 
	u32	s_checkinterval;	// max. time between checks 
	u32	s_creator_os;		// OS 
	u32	s_rev_level;		// Revision level 
	u16	s_def_resuid;		// Default uid for reserved blocks 
	u16	s_def_resgid;		// Default gid for reserved blocks 
	u32	s_first_ino; 		// First non-reserved inode 
	u16     s_inode_size; 		// size of inode structure 

  // some MORE non-essential fileds
};*/

struct ext2_group_desc
{
	u32	bg_block_bitmap;		/* Blocks bitmap block */
	u32	bg_inode_bitmap;		/* Inodes bitmap block */
	u32	bg_inode_table;			/* Inodes table block */
	u16	bg_free_blocks_count;	/* Free blocks count */
	u16	bg_free_inodes_count;	/* Free inodes count */
	u16	bg_used_dirs_count;		/* Directories count */
	u16	bg_pad;
	u32	bg_reserved[3];
};

struct ext2_inode 
{
	u16	i_mode;		/* File mode */
	u16	i_uid;		/* Owner Uid */
	u32	i_size;		/* Size in bytes */
	u32	i_atime;	/* Access time */
	u32	i_ctime;	/* Creation time */
	u32	i_mtime;	/* Modification time */
	u32	i_dtime;	/* Deletion Time */
	u16	i_gid;		/* Group Id */
	u16	i_links_count;	/* Links count */
	u32	i_blocks;	/* Blocks count */
	u32	i_flags;	/* File flags */
    u32 dummy;
	u32	i_block[15];    /* Pointers to blocks */
    u32     pad[7];    	/* inode size MUST be 128 bytes */
};

struct ext2_dir_entry_2 
{
	u32	inode;			/* Inode number */
	u16	rec_len;		/* Directory entry length */
	u8	name_len;		/* Name length */
	u8	file_type;
	char	name[255];      	/* File name */
};

#endif