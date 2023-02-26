#include <string.h>
#include <stddef.h>
#include "mipslab.h"


#define BUTTONS 4 /* 4 pages/lines available, and as such; 4 buttons */



static Button buttons[BUTTONS];
static int selected = 1;


//BUTTONS

void button_cons(Button *button, char const *string, void (*function)(void))
{

		strcpy(button->string, string);

	button->function = function;
}


void button_press(Button const *button)
{
	if(button->function != NULL)
		button->function();
}


void button_draw(Button const *button, int line)
{
	int const sz     = strlen(button->string);
	int const offset = (DSP_COLUMNS/2) -(sz*3);    //to find the offset every character takes almost 6 columns
	display_string(offset, line, button->string);
}




/* Draw all buttons with an outline around the currently selected one
 */
void interface_draw(void)
{
	/* Draw buttons */
	for(int i = 0; i < BUTTONS; ++i)
	{
		button_draw(&buttons[i], i);
	}

	int const page = selected * 8;

	for(int y = page +1 ; y < page + 7; ++y)   //to draw square beside the selected button
	{
		for(int x = 2; x < 7; ++x)
		{
				graphics_set(x, y, 1);
		}
	}
}




/* Press the currently selected button,
 * and reset the selected index
 */
void interface_button_press(void)
{
	button_press(&buttons[selected]);

}


void interface_button_prev(void)
{
	if(--selected < 0)
		selected = BUTTONS - 1;
}


void interface_button_next(void)
{
	if(++selected >= BUTTONS)
		selected = 0;
}


void interface_menu_load_main(void)
{
	state = STATE_MENU_MAIN;

	button_cons(&buttons[0], "WELCOME",      NULL);
	button_cons(&buttons[1], "PLAY",  interface_menu_load_difficulty);
	button_cons(&buttons[2], "HIGHSCORES",  interface_menu_load_highscores);
	button_cons(&buttons[3], "INSTRUCTIONS", interface_menu_load_help);
}


void interface_menu_load_highscores(void)
{
	state = STATE_MENU_HIGHSCORES;

	char buffer[BUTTON_BUFFER_SIZE];
	buffer[1] = '_';
	buffer[2] = ' ';

	int i;
	for(i = 0; i < GAME_HIGHSCORES; ++i)
	{
		buffer[0] = '1' + i;                        //show high scores from 1 to 3

		button_cons(&buttons[i], itoaconv(highscores[i]), NULL);
	}
	button_cons(&buttons[3], "BACK", interface_menu_load_main);
}


void interface_menu_load_help(void)
{
	state = STATE_MENU_HELP;

	button_cons(&buttons[0], "BTN4-LEFT BTN3-DOWN",  NULL);
	button_cons(&buttons[1], "     BTN2 - UP    ",  NULL);
	button_cons(&buttons[2], "    BTN1 - RIGHT " , NULL);
	button_cons(&buttons[3], "BACK", interface_menu_load_main);
}


void interface_menu_load_difficulty(void)
{
	state = STATE_MENU_DIFFICULTY;

	button_cons(&buttons[0], "CHOOSE A DIFFICULTY", NULL);
	button_cons(&buttons[1], "EASY",   game_start_easy);
	button_cons(&buttons[2], "MEDIUM", game_start_medium);
	button_cons(&buttons[3], "HARD",   game_start_hard);
}

void interface_menu_load_gameover(void)
{
	state = STATE_MENU_GAMEOVER;

	button_cons(&buttons[0], "GAME OVER", NULL);

	char buffer[BUTTON_BUFFER_SIZE];
	strcpy(buffer, "SCORE: ");
	// itodsconv(&buffer[7], game_score());
	button_cons(&buttons[1], itoaconv(game_score()), NULL);

	button_cons(&buttons[2], "RETRY",     interface_menu_load_difficulty);
	button_cons(&buttons[3], "MAIN MENU", interface_menu_load_main);

}


void interface_menu_load_advance(void)
{
	state = STATE_MENU_ADVANCE;

	button_cons(&buttons[0], "CONGRATULATIONS", NULL);

	char buffer[BUTTON_BUFFER_SIZE];
	strcpy(buffer, "FINISHED LEVEL ");
	button_cons(&buttons[1], itoaconv(game_level()-1), NULL);
	button_cons(&buttons[2], "ADVANCE TO NEXT LEVEL", game_continue);
	button_cons(&buttons[3], "RETURN TO MAIN MENU", interface_menu_load_main);

}


void interface_menu_load_won(void)
{
	state = STATE_MENU_WON;

	button_cons(&buttons[0], "YOU WON", NULL);
	button_cons(&buttons[1],"FINAL SCORE: " , NULL);
	button_cons(&buttons[2], itoaconv(game_score()), NULL);
	button_cons(&buttons[3], "RETURN TO MAIN MENU", interface_menu_load_main);
}
