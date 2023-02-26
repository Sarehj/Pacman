
/* game.c */

#include <stdlib.h>
#include "mipslab.h"





Entity ghosts[GAME_MAX_GHOSTS];
Entity player;
int    state;
int    highscores[GAME_HIGHSCORES];


static int score;
static int level;
static int lives;
static int boost;
static int difficulty;

static void reproduce(void);
static void calculate_highscore(void);
static void die(void);
static void gameover(void);
static void advance(void);
static void pickup_consumable(void);
static void reproduce_ghosts(void);




/* Initialize the default levels, and load the main menu
 */
void game_init(void)
{
	boost = 0;
	level_init();
	render_screen(start);
	delay(6000);
	graphics_clear();
	interface_menu_load_main();

	for(int i = 0; i < GAME_HIGHSCORES; ++i)
	{
		highscores[i] = 0;
	}
}




/* Tick once, move pacman, ghosts and adjust state
 */
void game_tick(void)
{
	/* First check and adjust boost status */
	if(boost)
		boost--;
	else
		state = STATE_PLAYING;


	/* Perform a tick for the pacman */

	for(int i = 0; i < 2; ++i)   //speed
	{
		entity_pacman_tick(&pacman);

		if(level_square(&pacman.pos) & LEVEL_SQUARE_CONSUMABLE)
			pickup_consumable();
	}
	//reset the speed so the pacman can move over coins one by one
	vector_set(&player.vec, 0, 0);


	/* If the freeze boost is not active, perform a tick for the ghosts */
	/* Also perform collision checks against pacman */
	if(state == STATE_PLAYING_BOOST_FREEZE)
		return;

	for(int i = 0; i < level_active_ghosts(); ++i)
	{
		entity_ghost_tick(&ghosts[i]);

		if(entity_intersects(&player, &ghosts[i]))
			die();
}
}




//If the game is in the PLAYING state show lives and score

void game_draw(void)
{
	level_draw();
	entity_draw(&pacman);
	for(int i = 0; i < level_active_ghosts(); ++i)
	{
		entity_draw(&ghosts[i]);
	}

	display_string(0, 0, "LIVES");
	display_string(0, 1, itoaconv(lives));
	display_string(0, 2, "SCORE");
	display_string(0, 3, itoaconv(score));

}



/* Start a game at level 1, and set the difficulty
 */
void game_start(int diff)
{
	state = STATE_PLAYING;
	difficulty = diff;

	entity_pacman_cons(&pacman);
	if(difficulty == DIFFICULTY_EASY)
	lives = GAME_DEFAULT_LIVES ;
	else if(difficulty == DIFFICULTY_MEDIUM)
	lives = GAME_DEFAULT_LIVES - 1;
	else if(difficulty == DIFFICULTY_HARD)
	lives = GAME_DEFAULT_LIVES - 2;

	score = 0;

	level = 1;
	level_load(level, difficulty);
}



static void reproduce(void)
{
	level_pacman_produce(&pacman.pos);

	reproduce_ghosts();
}



/* Update the highscores
 */
static void calculate_highscore(void)
{
	for(int i = 0; i < GAME_HIGHSCORES; ++i)
	{
		if(score >= highscores[i])
		{
			for(int j = GAME_HIGHSCORES - 1; j > i; --j)
			{
				highscores[j] = highscores[j-1];   //to compare the score with the other scores and the highest one
			}

			highscores[i] = score;
			break;
		}
	}
}




static void die(void)
{
	if(--lives == 0)
		gameover();
		//put dealay here
		else
		game_continue();
}




static void gameover(void)
{

	render_screen(gameover_pacman);
	delay(5000);
	graphics_clear();
	interface_menu_load_gameover();
	calculate_highscore();
}



/* Advance to the next level
 */
static void advance(void)
{
	if(++level > DEFAULT_LEVELS)
	{
		render_screen(won_pacman);
		delay(1000);
		graphics_clear();

		interface_menu_load_won();
		calculate_highscore();
	}
	else
	{
		render_screen(won_pacman);
		delay(1000);
		graphics_clear();

		interface_menu_load_advance();
		level_load(level, difficulty);
	}
}




/* If the pacman is on a consumable/booster; pick it up and
 * set the correct state
 */
static void pickup_consumable(void)
{
	int const square = level_square(&player.pos);

	if((square & LEVEL_SQUARE_CONSUMABLE_COIN) == LEVEL_SQUARE_CONSUMABLE_COIN)
	{
		score += 1;

		if(level_take_coin() == 0)    //if pacman eats all coins go no next level
			advance();
	}
	if((square & LEVEL_SQUARE_CONSUMABLE_FREEZE) == LEVEL_SQUARE_CONSUMABLE_FREEZE)
	{
		boost = GAME_BOOST_TICKS;
		state = STATE_PLAYING_BOOST_FREEZE;

		reproduce_ghosts();
	}

	level_square_empty(&pacman.pos);
}




static void reproduce_ghosts(void)
{
	for(int i = 0; i < level_active_ghosts(); ++i)
	{

		level_ghost_produce(&ghosts[i].pos);

	}
}


/* The solution that allows menu buttons to interact with the game
 */
void game_start_easy(void)
{
	game_start(DIFFICULTY_EASY);
}

void game_start_medium(void)
{
	game_start(DIFFICULTY_MEDIUM);
}

void game_start_hard(void)
{
	game_start(DIFFICULTY_HARD);
}
/* If the player dies, this is how the game continues */
void game_continue(void)
{
	state = STATE_PLAYING;
	reproduce();

}



char *difficulty_to_string(void)
{
	return difficulty == DIFFICULTY_EASY   ? "EASY"
	     : difficulty == DIFFICULTY_MEDIUM ? "MEDIUM"
	     : "HARD"
	;
}

int game_score(void)
{
	return score;
}

int game_level(void)
{
	return level;
}

int game_lives(void)
{
	return lives;
}
