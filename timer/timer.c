/* timer parameters. */
#define LATCH_COUNT     0x00	/* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	/* ccaammmb, a = access, m = mode, b = BCD */

/************************ NOTICE THE DIVISOR VALUE ***********************/
#define TIMER_FREQ   1193182L	/* timer frequency for timer in PC and AT */
#define TIMER_COUNT ((unsigned) (TIMER_FREQ/60)) /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0


int enable_irq(irq_nr) unsigned irq_nr;
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

/*===========================================================================*
 *				timer_init				     *
 *===========================================================================*/

ushort tick;

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");
  tick = 0;

  out_byte(TIMER_MODE, SQUARE_WAVE);	/* set timer to run continuously */
  out_byte(TIMER0, TIMER_COUNT);	/* load timer low byte */
  out_byte(TIMER0, TIMER_COUNT >> 8);	/* load timer high byte */
  enable_irq(TIMER_IRQ); 
}

/*===========================================================================*
 *				timer_handler				     *
 *===========================================================================*/

int thandler()
{
    tick++; 
    tick %= 60;

    if (tick % 60 == 0)
       printf("1 second timer interrupt\n");

    out_byte(0x20, 0x20);  

}





