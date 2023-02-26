#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"



int getsw( void ){
  return (PORTD >> 8) & 0x000F;  // SW1 corresponds to the four least significant bits. All other bits of the return
                                 //value must be zero
}                               //To get digits from 8 to 11 of PORTD



int getbtns(void) {

    int btn1 = PORTF >> 1;   //button 1
    btn1 &= 0x1;
    int btns = PORTD >> 4;   //button2,3,4
    btns &= 0xE;
    btns |= btn1;
    return btns;
  }

  void input_update(void)
  {
  	/* State needs to be saved in case the player pauses whilst under */
  	/* some kind of boost */
  	static int prev_state;

    //buttons while playing
  	if(state & STATE_PLAYING)
  	{
      // if(value & BTN1)
  		if(getbtns() & 1)
  			player.vec.x = 1;   //right
  		// if(value & BTN2)
  		if(getbtns() & 2)
  			player.vec.y = -1;  //up
  		// if(value & BTN3)
  		if(getbtns() & 4)
  			player.vec.y = 1;   //down
  		// if(value & BTN4)
  		if(getbtns() & 8)
  			player.vec.x = -1;  //left
  	}

    /* Menu instructions */
    else if(state & STATE_MENU)
    {
      quicksleep(1000000);

      // if(value & BTN3)
      if(getbtns() & 4)
        interface_button_next();
      // else if(value & BTN2)
      else if(getbtns() & 2)
        interface_button_press();
      // else if(value & BTN1)
      else if(getbtns() & 1)
        interface_button_prev();
      }
    }
