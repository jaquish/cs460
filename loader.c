3. Implement YOUR loader.c
       
   load(pathname, segment) char *pathname; ushort segment;
   {
      1. break up pathname into tokens:
         /a/b/c/d  ==> "a", "b", "c", "d" with n=4
 
      2. locate the inode of /a/b/c/d: let ip->INODE of file 
         return 0 if can't find the file

      3. read file header to get size information;
         print tsize, dsize, bsize

      3. load a.out data blocks (with header) into segment.
         move the executable part (|code|dat|....) upward 32 bytes to
         eliminate the header.

      4. Clear bss section (after |code|data|) to 0.

      5. return 1 for success.