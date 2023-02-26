/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int );

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}


void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */

  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void graphics_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}


// /* Set one pixel to either white or black
//  */
void graphics_set(int x, int y, int value)
{
	if(x >= 128 || x < 0)
		return;
	if(y >= 32  || y < 0)
		return;

			/* As the page size is 8 bits, y divided by 8 will give the correct */
			/* page number and the lower 3 bits will give the bit offset */

			/* Clear the pixel that is being written to */
			textbuffer[x][y >> 3] &= ~(1 << (y & 0x07));
			/* Write */
			textbuffer[x][y >> 3] |= (value ? 1 : 0) << (y & 0x07);
	}



	void display_string(int offset, int line,const char *s)
	{
		if(line < 0 || line >= 4)
			return;
		if(!s)
			return;
		if (offset < 0 || offset >= 128)
			return;

		int i, count_pixels = 0;
		while ((*s != '\0') && (count_pixels + 8) < (127 - offset))
		{
			for(i = 0; i < 8; i++)
				video_buffer[offset + count_pixels + i][line] |= font[(*s)*8 + i];

			s++;
			count_pixels += 6;
		}
	}


void display_image(int x, const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 32; j++)
			spi_send_recv(~data[i*32 + j]);
	}
}


void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Write the private video buffer to screen
 */
void graphics_reload(void) //dispaly update
{
	for(int p = 0; p < 4; ++p)
	{
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		/* Write to display */
		spi_send_recv(0x22);
		/* Specify which page we're writing to */
		spi_send_recv(p);
		/* Set byte ordering to start at left side of screen*/
		spi_send_recv(0x00);
		spi_send_recv(0x10);
		DISPLAY_CHANGE_TO_DATA_MODE;

		/* Write the data to display */
		for(int x = 0; x < 128; ++x)
		{
			spi_send_recv(textbuffer[x][p]);
		}
	}
}



/* Clear (black) the video buffer
 */
void graphics_clear(void)
{
	for(int p = 0; p < 4; ++p)
	{
		for(int x = 0; x < 128; ++x)
		{
			textbuffer[x][p] = 0x00;
		}
	}
}


uint8_t buffer[512];
uint8_t translator(uint8_t point){  // to set a
    switch(point) {
        case 0:
            return 0x3;
        case 1:
            return 0x0;
    }
    return 0;
}

void render_board(const uint8_t level[][64], uint8_t buffer[]){
    int i, j;
    uint8_t point_data;
    for(i = 0; i < 16; i++){
        for(j = 0; j < 64; j++){
            uint8_t point = level[i][j];
            point_data = translator(point);
            if(i%4 == 0) {buffer[j*2+(128*(i/4))] = point_data; buffer[j*2+(128*(i/4))+1] = point_data;}
            else{
                buffer[j*2+(128*(i/4))] = buffer[j*2+(128*(i/4))] + (point_data<<(2*(i%4)));
                buffer[j*2+(128*(i/4))+1] = buffer[j*2+(128*(i/4))+1] + (point_data<<(2*(i%4)));
            }
        }
    }
}
void display_screen(uint8_t *data) {
  int i, j;

  for(i = 0; i < 4; i++) {
    DISPLAY_CHANGE_TO_COMMAND_MODE;

    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(0x0);
    spi_send_recv(0x10);

    DISPLAY_CHANGE_TO_DATA_MODE;

    for(j = 0; j < 128; j++)
      spi_send_recv(data[i*128 + j]);
  }
}
void render_screen(const uint8_t data[][64]){
    uint8_t buffer[512];
    render_board(data,buffer);
    display_screen(buffer);
}




/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n )
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}




#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
