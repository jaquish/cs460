                BOOTSEG = 0x1000
.globl begtext, begdata, begbss                      !! needed by linker

!               IMPORTS and EXPORTS
.globl _main, _tswitch, _running, _scheduler
.globl _int80h, _kcinth, _goUmode
.globl _proc, _procsize
.globl _color
	
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

        call _main                      ! call main[] in C

! if ever return, just hang     
dead:   jmp   dead

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

!These offsets are defined in struct proc
USS =   4
USP =   6

_int80h:
        push ax                 ! save all Umode registers in ustack
        push bx
        push cx
        push dx
        push bp
        push si
        push di
        push es
        push ds

! ustack contains : flag,uCS,uPC, ax,bx,cx,dx,bp,si,di,ues,uds
        push cs
        pop  ds                 ! KDS now

	mov si,_running  	! ready to access proc
        mov USS[si],ss          ! save uSS  in proc.USS
        mov USP[si],sp          ! save uSP  in proc.USP

! Change ES,SS to kernel segment
        mov  di,ds              ! stupid !!        
        mov  es,di              ! CS=DS=SS=ES in Kmode
        mov  ss,di

! set sp to HI end of running's kstack[]
	mov  sp,_running        ! proc's kstack [2 KB]
        add  sp,_procsize       ! HI end of PROC

        call  _kcinth

        jmp   _goUmode
  
_goUmode:
        cli
	mov bx,_running 	! bx -> proc
        mov cx,USS[bx]
        mov ss,cx               ! restore uSS
        mov sp,USP[bx]          ! restore uSP
  
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
