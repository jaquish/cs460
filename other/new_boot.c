{
	for(int i = 0; i < 4; i++)	// four loops: for direct, indirect, 2x indirect, 3x indirect
	{
		int d_blocks = 12;
		int ind_blocks = 0;
		int ind2_blocks = 0;
		int ind3_blocks = 0;

		// pointers to lists of (in)direct blocks
		u32 *d_list    = &INODE->i_block[0];
		u32 *ind_list  = &INODE->i_block[12];
		u32 *ind2_list = &INODE->i_block[13];
		u32 *ind3_list = &INODE->i_block[14];

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
				ind_list = &INODE->i_block[12];
				d_blocks = 256;
				break;

			case 2:
				ind2_blocks = 1;
				ind2_list  = &INDDE->i_block[13];
				ind_blocks = 256;
				break;

			case 3:
				ind3_blocks = 1;
				ind3_list   = &INODE->i_block[14];
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
						get_block(d_list[d], d_buffer);

						// DO WORK ON DIRECT BLOCKS
					}
				}
			}
		}


		// load and scan direct blocks
		for (d = 0; d < d_blocks; d++)
		{
			// load block into buffer
			// scan block for dir names
			// if a match, return inode number
		}
	}
}