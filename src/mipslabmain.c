

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

void tick(void);
void init(void);



void tick(void)
{
	static int invoc = 0;

	// input_poll();

	/* Only run this routine every other invocation */
	if(++invoc & 1)
	{
		input_update();

		if(state & STATE_PLAYING)
		{
			game_tick();
			game_draw();


		}
		else if(state & STATE_MENU)
		{

			interface_draw();
		}

		// graphics_postprocess();
		graphics_reload();
		graphics_clear();
	}

	/* Reset interrupt flag */
	IFS(0) &= ~(1 << 8);
}


void init (void) {
        /*
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */

	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;

	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);

	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
        SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;


		graphics_init();

		/* Initialize game */
		game_init();

		/* Setup timer used for game loop to run every 0.1 seconds */
		T2CONCLR = 1 << 15;	  // Disable timer
		T2CONSET = 7 << 4;	   // Prescaler = 256
		PR2 = 80000000 / (256 * 10); // Tick every 0.1 seconds
		TMR2 = 0;		    // Reset timer value
		T2CONSET = 1 << 15;	  // Enable timer
		IEC(0) |= 1 << 8;	    // Set interrupt 2 to listen to timer 2
		IPC(2) |= 7 << 0;	    // Set interrupt 2 priority to 7 (highest)

		      // Enable interrupts
		  enable_interrupt();


// 	labinit(); /* Do any lab-specific initialization */
//
// 	while( 1 )
// 	{
// 	  labwork(); /* Do lab-specific things again and again */
// 	}
// 	return 0;
// }
