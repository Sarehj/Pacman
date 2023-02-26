

	 #pragma once

	 #include <stdint.h>

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void labwork(void);
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);


// vector

/* Assuming the screen is not larger than 128 pixels in either height
 * or width, 8-bit integers will be enough
 */
typedef struct VECTOR
{
	int8_t x;
	int8_t y;
}Vector;

void vector_set(Vector *, int, int);
void vector_copy(Vector *, Vector const *);
void vector_add(Vector *, Vector const *);
void vector_scalar_multiply(Vector *, int);
int  vector_eq(Vector const *, Vector const *);


// peripherals

#define DSP_COLUMNS 128
#define DSP_ROWS    32
#define DSP_PAGES   4

extern volatile int invert;


void    input_update(void);
int getbtns(void);

void graphics_init(void);
void graphics_set(int, int, int);
void display_string(int, int,const char *);
void graphics_reload(void);
void graphics_postprocess(void);
void graphics_clear(void);

void led_init(void);
void led_clear(void);
void led_set(uint8_t);
void enable_interrupt(void);
uint8_t translator(uint8_t point);



//level

#define LEVEL_COLUMNS     67
#define LEVEL_ROWS	 32
#define LEVEL_OFFSET_X     40
#define LEVEL_OFFSET_Y     0
#define LEVEL_MAX_PRODUCE   4
#define LEVEL_MAX_BOOSTERS 2
#define DEFAULT_LEVELS     2

enum LEVEL_SQUARE
{
	LEVEL_SQUARE_EMPTY	     = 0,
	LEVEL_SQUARE_WALL	      = 1,
	LEVEL_SQUARE_INACCESSIBLE      = 2,
	// LEVEL_SQUARE_PORTAL	    = 4,
	LEVEL_SQUARE_GHOST	     = 8,
	LEVEL_SQUARE_CONSUMABLE	= 16,
	LEVEL_SQUARE_CONSUMABLE_COIN   = 48,
	// LEVEL_SQUARE_CONSUMABLE_SPEED  = 80,
	LEVEL_SQUARE_CONSUMABLE_FREEZE = 144
};

void level_init(void);
void level_load(int, int);
void level_populate(void);
void level_draw(void);

int  level_square(Vector const *);
void level_square_empty(Vector const *);
void level_pacman_produce(Vector *);
void level_ghost_produce(Vector *);
int  level_active_ghosts(void);
int  level_take_coin(void);



// interface

void interface_draw(void);
void interface_button_prev(void);
void interface_button_next(void);
void interface_button_press(void);

/* This is a solution, but it allows menu buttons to
 * interact more efficiently using function pointers
 */
void interface_pacman_load (void);
void interface_menu_load_main(void);
void interface_menu_load_highscores(void);
void interface_menu_load_help(void);
void interface_menu_load_difficulty(void);
void interface_menu_load_paused(void);
void interface_menu_load_dead(void);
void interface_menu_load_gameover(void);
void interface_menu_load_advance(void);
void interface_menu_load_won(void);



/* icon.h extern int8_t const font[][FONT_WIDTH];
extern const uint8_t start[16][64];*/

#define FONT_WIDTH  6
#define FONT_HEIGHT 8
#define ICON_ENTITY_SIZE 5
#define ICON_BOOSTER_SIZE 4


typedef struct ICON
{
	int8_t const *data;
	Vector size;
}Icon;


extern int8_t const icon_pacman_up    [];
extern int8_t const icon_pacman_left     [];
extern int8_t const icon_pacman_down    [];
extern int8_t const icon_pacman_right     [];

extern int8_t const icon_ghost[];
extern int8_t const icon_booster_freeze[];

extern const uint8_t won_pacman[16][64];
extern const uint8_t gameover_pacman[16][64];
extern const uint8_t start[16][64];

extern const uint8_t const font[128*8];

void icon_draw(Icon const *, Vector const *);
void display_screen(uint8_t *data);


//GAME

efine GAME_MAX_GHOSTS      4
#define GAME_HIGHSCORES    3
#define GAME_DEFAULT_LIVES 3
#define GAME_BOOST_TICKS   32

enum DIFFICULTY
{
	DIFFICULTY_EASY   = 2,
	DIFFICULTY_MEDIUM = 3,
	DIFFICULTY_HARD   = 4,
};


enum STATE
{
     STATE_PLAYING              = 0x0001,
     STATE_MENU                 = 0x0002,
     STATE_MENU_MAIN            = 0x0006,
     STATE_MENU_HIGHSCORES      = 0x000A,
     STATE_MENU_DIFFICULTY      = 0x0012,
     STATE_MENU_HELP            = 0x0042,
     STATE_MENU_ADVANCE	        = 0x0082,
     STATE_MENU_GAMEOVER        = 0x0202,
     STATE_PLAYING_BOOST        = 0x0401,
     STATE_PLAYING_BOOST_FREEZE = 0x0801,
     STATE_MENU_WON             = 0x2002
};

extern Entity pacman;
extern Entity ghosts[GAME_MAX_GHOSTS];
extern int    state;
extern int    highscores[3];

void game_start(int);
void game_exit(void);
void game_init(void);
void game_tick(void);
void game_draw(void);

void game_continue(void);
void game_start_easy(void);
void game_start_medium(void);
void game_start_hard(void);

char *difficulty_to_string(void);
int game_score(void);
int game_level(void);
int game_lives(void);


// functions

uint8_t spi_send_recv(uint8_t);
void quicksleep(int);
void delay(int);
void itodsconv(char *, int);
char * itoaconv( int num );
void render_screen(const uint8_t data[][64]);

//ENTITY

typedef struct ENTITY
{
	Icon   icon; /* Pointer to icon data */
	Vector pos;  /* Position vector */
	Vector vec;  /* Movement vector */
}Entity;


void entity_cons(Entity *);
void entity_move(Entity *);
void entity_draw(Entity const *);
int  entity_intersects(Entity const *, Entity const *);

void entity_pacman_cons(Entity *);
void entity_pacman_move(Entity *);
void entity_pacman_tick(Entity *);

void entity_ghost_cons(Entity *);
void entity_ghost_move(Entity *);
void entity_ghost_tick(Entity *);


//BUTTON

#define BUTTON_BUFFER_SIZE 22 /* Highest amount of characters that can fit on one line */

/* Buttons can also be used as menu titles or information,
 * by setting the function variable to NULL
 */
typedef struct BUTTON
{
	void (*function)(void);
	char  string[BUTTON_BUFFER_SIZE];
}Button;

void button_cons(Button *, char const *, void (*)(void));
void button_press(Button const *);
void button_draw(Button const *, int);
