/********************* Main Source of Infomation  ************************
www.osdever.net/FreeVGA : home of FreeVGA programming information
www.osdever.net/FreeVGA/vga/crtcreg.htm : CRT Controller Registers

Index 00h -- Horizontal Total Register 
     Index 01h -- End Horizontal Display Register 
     Index 02h -- Start Horizontal Blanking Register 
     Index 03h -- End Horizontal Blanking Register 
     Index 04h -- Start Horizontal Retrace Register 
     Index 05h -- End Horizontal Retrace Register 
     Index 06h -- Vertical Total Register 
     Index 07h -- Overflow Register 
     Index 08h -- Preset Row Scan Register 
     Index 09h -- Maximum Scan Line Register 
     Index 0Ah -- Cursor Start Register 
     Index 0Bh -- Cursor End Register 
     Index 0Ch -- Start Address High Register 
     Index 0Dh -- Start Address Low Register 
     Index 0Eh -- Cursor Location High Register 
     Index 0Fh -- Cursor Location Low Register 
     Index 10h -- Vertical Retrace Start Register 
     Index 11h -- Vertical Retrace End Register 
     Index 12h -- Vertical Display End Register 
     Index 13h -- Offset Register 
     Index 14h -- Underline Location Register 
     Index 15h -- Start Vertical Blanking Register 
     Index 16h -- End Vertical Blanking 
     Index 17h -- CRTC Mode Control Register 
     Index 18h -- Line Compare Register 
Among these, only Registers 
      10-11  control CURSOR height (# of scand lines)
      12-13  display origin address
      14-15  CURSOR position
are of interests here:
**************************************************************************/
/************************************************************************
                 HOW does PC's color display work:
 0. Before vid_init(), MUST set to mono mode, otherwise, no display at all.

 1, Screen = 25 rows X 80 cols = 4000 WORDs = [attribue byte|char byte]
                                 8000 bytes 
 2. Video RAM at 0xB800, size=16 to 64 KB >> screen=8000 bytes;
    To display a screen: write 8000 bytes (4000 WORDs) to the video RAM
    at the ORG location; tell the display register where the ORG is.
    The display hardware will display the 4000 chars from ORG to the screen.

 3. Cursor shape and position can be changed by writing to CRT register

 4. Software must keep track of the Cursor position (row,col). For each
    char displayed, it must advance the Curcor by one word, which may change 
    (row,col): When col % 80 which may inc row
               When row>= 25, must scroll up one row

5.  To Scroll up one row:
       inc org by one row (tell Hardware to display from the new org position) 
       write a row of blanks to ORG + one screen  in video RAM 
       if the last row exceeds VRAM size, must physically copy CURRENT screen
       to VRAM beginning and reset org to 0
*****************************************************************/
#define C_6845         0x3D0	/* port for 6845 color board */

#define VINDEX             4	/* 6845's index register */
#define VDATA              5	/* 6845's data register */
#define CUR_SIZE          10	/* 6845's cursor size register */
#define VID_ORG           12	/* 6845's origin register */
#define CURSOR            14	/* 6845's cursor position register */

#define LINE_WIDTH        80	/* # characters on a line */
#define SCR_LINES         25	/* # lines on the screen */
#define SCR_BYTES	8000	/* bytes of ONE screen=25*80 */

#define CURSOR_SHAPE      15    /* block cursor for MDA/HGC/CGA/EGA/VGA... */

// attribute byte: 0x0HRGB, H=highLight; RGB determine color
#define HGREEN          0x0A
#define HCYAN           0x0B
#define HRED            0x0C
#define HPURPLE         0x0D
#define HYELLOW         0x0E

ushort base     = 0xB800;    // VRAM base address
ushort vid_mask = 0x3FFF;    // mask=Video RAM size - 1
int    vid_port = C_6845;    // vid port address

ushort offset;               // current display position
int    color;                // atttirbute byte
int    org;                  // current display origin
int    row, column;          // logical row, col position

// vid_init() initializes the display org=0 (row,col)=(0,0)
int vid_init()
{ 
  int i, w;
  org = row = column = 0;
  color = HYELLOW;

  set_6845(CUR_SIZE, CURSOR_SHAPE);  // set cursor size	
  set_6845(VID_ORG, 0);              // display origin to 0
  set_6845(CURSOR, 0);	             // set cursor position to 0

  //clear screen
  w = 0x0700;    // White, blank char // attribute byte=0000 0111=0000 0RGB
  for (i=0; i<24*80; i++){                               
    put_word(w, base, 0+2*i);        // write 24*80 blanks to VGA memory
  } 
}

/*************************************************************************
 scroll(): scroll UP one line
**************************************************************************/
int scroll()
{
  int i;
  ushort w, bytes;  

  // Normal scrolling using the 6845 registers.

  // try offset = org + ONE screen + ONE more line
  offset = org + SCR_BYTES + 2*LINE_WIDTH;

  if (offset <= vid_mask){   // offset still within vram area
    org += 2*LINE_WIDTH;     // just advance org by ONE line
  }
  else{  // offset exceeds vram area ==> reset to vram beginning by    
         // copy current rows 1-24 to BASE, then reset org to 0

    for (i=0; i<24*80; i++){
      w = get_word(base, org+160+2*i);
      put_word(w, base, 0+2*i);
    }  
    org = 0;
  }

  // org has been set up correctly
  offset = org + 2*24*80;   // offset = beginning of row 24

  // copy a line of BLANKs to row 24

  w = 0x0C00;  // HRGB=1100 ==> HighLight RED, Null char

  for (i=0; i<80; i++)                  
    put_word(w, base, offset + 2*i);

  set_6845(VID_ORG, org >> 1);	  /// 6845 thinks in words 

}

int putc(c) char c;
{
  // display c to cursor position, advance cursor

  int pos, w, offset;

  if (c=='\n'){
    row++;
    if (row>=25){
      row = 24;
      scroll();
    }

    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_6845(CURSOR, offset >> 1);
    return; 
  }

  if (c=='\r'){
    column=0;

    pos = 2*(row*80 + column);
    offset = (org + pos) & vid_mask;
    set_6845(CURSOR, offset >> 1);
    return;
  }

  if (c=='\b'){
    if (column > 0){
      column--;

      pos = 2*(row*80 + column);
      offset = (org + pos) & vid_mask;
      put_word(0x0700, base, offset);

      set_6845(CURSOR, offset >> 1); 
    }
    return;
  }

  pos = 2*(row*80 + column);
  offset = (org + pos) & vid_mask;
  w = color;
  w = (w << 8) + c;

  put_word(w, base, offset);

  column++;

  if (column >= 80){
    column = 0;
    row++;

    if (row>=25){
      row = 24;
      scroll();
    }
  }

  pos = 2*(row*80 + column);
  offset = (org + pos) & vid_mask;
  set_6845(CURSOR, offset >> 1);
}     

/*===========================================================================*
 *				set_6845				     *
 *===========================================================================*/
int set_6845(reg, val) 
int reg;	/* which register pair to set */
int val;	/* 16-bit value to set it to */
{
/* Set a register pair inside the 6845.  
 * Registers 10-11 control the format of the cursor (how high it is, etc).
 * Registers 12-13 tell the 6845 where in video ram to start (in WORDS)
 * Registers 14-15 tell the 6845 where to put the cursor (in WORDS)
 *
 * Note that registers 12-15 work in words, i.e. 0x0000 is the top left
 * character, but 0x0001 (not 0x0002) is the next character.  This addressing
 * is different from the way the 8088 addresses the video ram, where 0x0002
 * is the address of the next character.
 */
  lock();			/* try to stop h/w loading in-between value */
  out_byte(vid_port + VINDEX, reg);	/* set the index register */
  out_byte(vid_port + VDATA, (val>>8) & 0xFF);	/* output high byte */
  out_byte(vid_port + VINDEX, reg + 1);	/* again */
  out_byte(vid_port + VDATA, val&0xFF);	/* output low byte */
  unlock();
}

// change color syscall from Umode, just for fun
int chcolor(y) ushort y;
{
  y &= 0x7F;
  switch(y){
    case 'r' : color=HRED;    break;
    case 'y' : color=HYELLOW; break;
    case 'c' : color=HCYAN;   break;
    case 'g' : color=HGREEN;  break;
  }
}

int ktest()
{
  int y,i,w,pos;
  while(1){
    printf("enter color [r|b|g|y|c|p] : ");
    y = getc() & 0x7F;
    switch(y){
      case 'r' : color=HRED;    break;
      case 'y' : color=HYELLOW; break;
      case 'c' : color=HCYAN;   break;
      case 'g' : color=HGREEN;  break;
      case 'p' : color=HPURPLE; break;
    }
    w = (color << 8) + y;
    for (i=0; i<25*80; i++){                               
       put_word(w, base, 0+2*i);        // write 24*80 blanks to VGA memory
    } 
    org = 0;
    set_6845(VID_ORG, org >> 1);	  /// 6845 thinks in words 
    pos = 2*(25*80 + 80) + 2;
    offset = (org + pos) & vid_mask;

    row = 24; column=0;
    set_6845(CURSOR, offset >> 1);
    if (y=='q') break;
  }
}
