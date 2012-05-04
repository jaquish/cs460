                BOOTSEG = 0x1000
.globl begtext, begdata, begbss                      !! needed by linker

!               IMPORTS and EXPORTS
.globl _resetVideo,_getc,_putc,_diskr,_setes,_inces 
.globl _main,_prints                                 
.globl _tswitch,_running,_scheduler

.globl _int80h,_kcinth
.globl _goUmode
.globl _get_byte,_put_byte
.globl _proc, _procsize
.globl _color
.globl _lock, _unlock,_restore, _in_byte, _out_byte
.globl _inkmode
.globl _tinth, _thandler

.text                                                !! these tell as:	
begtext:                                             !! text,data,bss segments
.data                                                !! are all the same.
begdata:
.bss
begbss:
.text                                                

        mov     ax,cs                   ! establish segments 
        mov     ds,ax                   ! we know ES,CS=0x1000. Let DS=CS  
        mov     ss,ax                   ! SS = CS ===> all point to 0x1000
        mov     es,ax

        mov     sp,#_proc               ! SP -> proc[0].kstack HIGH end
        add     sp,_procsize

        mov     ax,#0x0003
        int     #0x10

        call _main                      ! call main[] in C

! if ever return, just hang     
        mov   ax, #msg
        push  ax
        call  _prints
dead:   jmp   dead
msg:    .asciz "BACK TO ASSEMBLY AND HANG\n\r"    
	
!*************************************************************
!     KCW  added functions for MT system
!************************************************************
_tswitch:
          push   ax
          push   bx
          push   cx
          push   dx
          push   bp
          push   si
          push   di
	  pushf
	  mov	 bx, _running
 	  mov	 2[bx], sp

find:     call	 _scheduler

resume:	  mov	 bx, _running
	  mov	 sp, 2[bx]
	  popf
	  pop    di
          pop    si
          pop    bp
          pop    dx
          pop    cx
          pop    bx
          pop    ax
          ret

USS =  4
USP =  6

! as86 macro: parameters are ?1 ?2, etc 
! as86 -m -l listing src (generates listing with macro expansion)

         MACRO INTH
          push ax
          push bx
          push cx
          push dx
          push bp
          push si
          push di
          push es
          push ds

          push cs
          pop  ds

          inc _inkmode          ! enter Kmode : ++inkmode
          cmp _inkmode,#1       ! if inkmode == 1 ==> interrupt was in Umode
          jg  ?1                ! imode>1 : was in Kmode: bypass saving uss,usp

          ! was in Umode: save interrupted (SS,SP) into proc
	  mov si,_running   	! ready to access proc
          mov USS[si],ss        ! save SS  in proc.USS
          mov USP[si],sp        ! save SP  in proc.USP

          ! change DS,ES,SS to Kernel segment
          mov  di,ds            ! stupid !!        
          mov  es,di            ! CS=DS=SS=ES in Kmode
          mov  ss,di

          mov  sp, _running     ! sp -> running's kstack[] high end
          add  sp, _procsize

?1:       call  _?1             ! call handler in C

          br    _ireturn        ! return to interrupted point

         MEND


_int80h: INTH kcinth

_tinth:  INTH thandler

!*===========================================================================*
!*		_ireturn  and  goUmode()       				     *
!*===========================================================================*
! ustack contains    flag,ucs,upc, ax,bx,cx,dx,bp,si,di,es,ds
! uSS and uSP are in proc
_ireturn:
_goUmode:
        cli
        dec _inkmode            ! --inkmode
        cmp _inkmode,#0         ! inkmode==0 means was in Umode
        jg  xkmode

! restore uSS, uSP from running PROC
	mov si,_running 	! si -> proc
        mov ax,USS[si]
        mov ss,ax               ! restore SS
        mov sp,USP[si]          ! restore SP
xkmode:                         
	pop ds
	pop es
	pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax 
        iret

	
        !--------------------------------
        ! resetVideo[] : clear screen, home cursor
        !--------------------------------
_resetVideo:	
        mov     ax, #0x0012             ! 640x480 color
        int     0x10                    ! call BIOS to do it
        mov     ax, #0x0200             ! Home the cursor
        xor     bx, bx
        xor     dx, dx
        int     0x10                    ! call BIOS to home cursor 
        ret 

       !---------------------------------------
       ! int diskr[cyl, head, sector, buf] 
       !            4     6     8     10
       !---------------------------------------
_diskr:                             
        push  bp
	mov   bp,sp
	
        movb  dl, #0x00        ! drive 0=fd0
        movb  dh, 6[bp]        ! head
        movb  cl, 8[bp]        ! sector
        incb  cl               ! inc sector by 1 to suit BIOS
        movb  ch, 4[bp]        ! cyl
        mov   ax, #0x0202      ! READ 2 sectors 
        mov   bx, 10[bp]       ! put buf value in BX ==> addr=[ES,BX]
        int  0x13              ! call BIOS to read the block 
        jb   error             ! to error if CarryBit is on [read failed]

	mov   sp,bp
	pop   bp
	ret

        !---------------------------------------------
        !  char getc[]   function: returns a char
        !---------------------------------------------
_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret 

        !----------------------------------------------
        ! void putc[char c]  function: print a char
        !----------------------------------------------
!_putc:           
        push   bp
	mov    bp,sp
	
        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
 
        mov    bx,_color       ! bL = color B Cyan C Red  
        int    0x10            ! call BIOS to display the char

        mov    sp,bp
	pop    bp
	ret
	        
        
_setes:  push  bp
	 mov   bp,sp
	
         mov   ax,4[bp]        
         mov   es,ax

	 mov   sp,bp
	 pop   bp
	 ret

_inces:                        ! inces[] inc ES by 0x40, or 1K
         mov   ax,es
         add   ax,#0x40
         mov   es,ax
         ret

        !------------------------------
        !       error & reboot
        !------------------------------
error:
        mov  bx, #bad
        push bx
        call _prints
        
        int  0x19                       ! reboot
bad:    .asciz  "Error!"


!*===========================================================================*
!*				get_byte				     *
!*===========================================================================*
! This routine is used to fetch a byte from anywhere in memory.
! The call is:
!     c = get_byte[segment, offset]
! where
!     'segment' is the value to put in es
!     'offset'  is the offset from the es value
_get_byte:
	push bp			! save bp
	mov bp,sp		! we need to access parameters

	push es			! save es
        push bx

	mov es,4[bp]		! load es with segment value
	mov bx,6[bp]		! load bx with offset from segment
	seg es			! go get the byte
	movb al,[bx]		! al = byte
	xorb ah,ah		! ax = byte

        pop bx
	pop es			! restore es

        mov bp,sp
	pop bp			! restore bp
	ret			! return to caller


!*===========================================================================*
!*				put_byte				     *
!*===========================================================================*
! This routine is used to put a word to anywhere in memory.
! The call is:
!           put_byte[char,segment,offset]
! where
!     char is a byte
!     'segment' is a segment
!     'offset'  is the offset from the segment
_put_byte:
	push bp			! save bp
	mov  bp,sp		! we need to access parameters

	push es			! save es
        push bx

	mov  es,6[bp]   	! load es with segment value
	mov  bx,8[bp]		! load bx with offset from segment
        movb al,4[bp]           ! load byte in aL
	seg  es			! go put the byte to [ES, BX]
	movb  [bx],al		! there it goes

        pop  bx                 ! restore bx
	pop  es			! restore es

        mov  bp,sp
	pop  bp			! restore bp
	ret			! return to caller
!*===========================================================================*
!*			 old_flag=lock()				     *
!*===========================================================================*
! Disable CPU interrupts.
_lock:  
	pushf			! save flags on stack
	cli			! disable interrupts
	pop ax   		! pop saved flag into ax
	ret			! return old_flag


!*===========================================================================*
!*				unlock					     *
!*===========================================================================*
! Enable CPU interrupts.
_unlock:
	sti			! enable interrupts
	ret			! return to caller

!*===========================================================================*
!*				restore(old_flag)       		     *
!*===========================================================================*
! Restore enable/disable bit to the value it had before last lock.
_restore:
        push bp
        mov  bp,sp
       
        push 4[bp]
	popf			! restore old_flag

        mov  sp,bp
        pop  bp
	ret			! return to caller


!*===========================================================================*
!*				in_byte					     *
!*===========================================================================*
! PUBLIC unsigned in_byte[port_t port];
! Read an [unsigned] byte from the i/o port  port  and return it.

_in_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
	in      ax,dx			! input 1 byte
	subb	ah,ah		! unsign extend
        pop     bp
        ret

!*===========================================================================*
!*				out_byte				     *
!*==============================================================
! out_byte[port_t port, int value];
! Write  value  [cast to a byte]  to the I/O port  port.

_out_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
        mov     ax,6[bp]
	outb	dx,al		! output 1 byte
        pop     bp
        ret
