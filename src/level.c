/* level.c */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mipslab.h"


#define VISIBLE_SQUARES (LEVEL_SQUARE_WALL | LEVEL_SQUARE_CONSUMABLE_COIN)
#define COIN_SPACING 2



extern int8_t const map_level_1[LEVEL_COLUMNS*LEVEL_ROWS];
extern int8_t const map_level_2[LEVEL_COLUMNS*LEVEL_ROWS];



typedef struct LEVEL
{
	int8_t const *map; /* Pointer to the map layout */
	int    coins;    /* Amount of coins */
	int    ghosts;   /* Amount of ghosts */
	Vector origin;   /* Player spawn */
	Vector produce[LEVEL_MAX_PRODUCE];    /* Ghost produce */
	Vector booster[LEVEL_MAX_BOOSTERS];   /* freeze */
}Level;


static int square_exists(Vector const *);


/* All game logic manipulation will occurr on this map */
static int8_t map[LEVEL_COLUMNS][LEVEL_ROWS];
/* Default level data */
static Level default_level[DEFAULT_LEVELS];
/* currently loaded level information */
static Level loaded;




/* Load a level, produce ghosts and populate it with boosters and coins
 */
void level_load(int level, int difficulty)
{
	//to start from level 1
	if(level > DEFAULT_LEVELS)
		level = DEFAULT_LEVELS;
	else if(level < 1)
		level = 1;


// copy the level data into the loaded level
	memcpy(&loaded, &default_level[level - 1], sizeof(struct LEVEL));

	/* Load level map data into the map */
	for(int x = 0; x < LEVEL_COLUMNS; ++x)
	{
		for(int y = 0; y < LEVEL_ROWS; ++y)
		{
			map[x][y] = loaded.map[y*LEVEL_COLUMNS + x];
		}
	}


	/* produce pacman */
		vector_copy(&pacman.pos, &loaded.origin);
	/* Set amount of and produce ghosts */
	loaded.ghosts = difficulty;
	for(int i = 0; i < loaded.ghosts; ++i)
	{
		entity_ghost_cons(&ghosts[i]);    //to get ghosts informations(size and icon)
		level_ghost_produce(&ghosts[i].pos);   //give a position to each ghost
	}

	level_populate();
}




/* Populates the loaded level with coins spaced out evenly
 * with other coins and consumables
 */
void level_populate(void)
{
	for(int y = 0; y < LEVEL_ROWS; ++y)
	{
		for(int x = 0; x < LEVEL_COLUMNS; ++x)
		{
			if(map[x][y] != LEVEL_SQUARE_EMPTY || vector_eq(&(Vector){x, y}, &loaded.origin))   //check that the square does not have a coin already
				goto next_iteration;       //to break from the function

			// Check the surrounding squares for consumables
			for(int i = -COIN_SPACING; i <= COIN_SPACING; ++i)
			{
				for(int j = -COIN_SPACING; j <= COIN_SPACING; ++j)
				{
					if(level_square(&(Vector){x+i, y+j}) & (LEVEL_SQUARE_CONSUMABLE))
						goto next_iteration;
				}
			}
			// If no other consumables found, place a coin
			map[x][y] = LEVEL_SQUARE_CONSUMABLE_COIN;
			loaded.coins++;

			next_iteration: ;
		}
	}
}




/* Check if coordinates are within bounds
 */
static int square_exists(Vector const *coord)
{
	if(coord->x >= LEVEL_COLUMNS || coord->x < 0)
		return 0;
	if(coord->y >= LEVEL_ROWS || coord->y < 0)
		return 0;

	return 1;
}


/* Return the data on a given square
 */
int level_square(Vector const *coord)
{
if(square_exists(coord))
	return map[coord->x][coord->y];
else
	 return LEVEL_SQUARE_EMPTY;

}


/* Clear a position of coins when it is eaten
 */
void level_square_empty(Vector const *coord)
{
	if(square_exists(coord))
		map[coord->x][coord->y] = LEVEL_SQUARE_EMPTY;
}


/* Returns the amount of loadedly active ghosts
 */
int level_active_ghosts(void)
{
	return loaded.ghosts;
}


/* Decrements and returns the remaining coins
 */
int level_take_coin(void)
{
	return --loaded.coins;
}


/* Writes the coordinates of the pacman/ghost produce to the out parameter
 */
void level_pacman_produce(Vector *out)
{
       vector_copy( out,&loaded.origin);
}

void level_ghost_produce(Vector *out)
{
	static int previous = 0;

	if(previous >= LEVEL_MAX_PRODUCE)
		previous = 0;

	vector_copy(out, &loaded.produce[previous++]);
}




/* Updates the graphics video buffer with the level information
 */
void level_draw(void)
{
	/* Draw squares and coins */
	for(int y = 0; y < LEVEL_ROWS; ++y)
	{
		for(int x = 0; x < LEVEL_COLUMNS; ++x)
		{
			graphics_set(x + LEVEL_OFFSET_X, y + LEVEL_OFFSET_Y, map[x][y] & VISIBLE_SQUARES);
		}
	}


	Icon const freeze = (Icon)
	{
		icon_booster_freeze,
		(Vector){ICON_BOOSTER_SIZE, ICON_BOOSTER_SIZE}
	};
	for(int i = 0; i < LEVEL_MAX_BOOSTERS; ++i)
	{
		Vector const position = (Vector)
		{
			loaded.booster[i].x + LEVEL_OFFSET_X - (ICON_BOOSTER_SIZE/2 ), //  /2
			loaded.booster[i].y + LEVEL_OFFSET_Y - (ICON_BOOSTER_SIZE/2 )
		};

		 if((level_square(&loaded.booster[i]) & LEVEL_SQUARE_CONSUMABLE_FREEZE) == LEVEL_SQUARE_CONSUMABLE_FREEZE)
			icon_draw(&freeze, &position);
	}
}




/* Set up all the data for all default levels
 */
void level_init(void)
{
	/* Set up level 1 */
	default_level[0].map	            = map_level_1;
	default_level[0].coins	            = 0;
	default_level[0].ghosts	            = 2;
	default_level[0].origin	            = (Vector){31, 29};
	default_level[0].produce[0]           = (Vector){26, 2};
	default_level[0].produce[1]           = (Vector){31, 2};
	default_level[0].produce[2]           = (Vector){36, 2};
	default_level[0].produce[3]           = (Vector){31, 2};
	default_level[0].booster[0]         = (Vector){59, 2};
	default_level[0].booster[2]         = (Vector){3, 29};



	/* Level 2 */
	default_level[1].map                = map_level_2;
	default_level[1].coins	            = 0;
	default_level[1].ghosts	            = 3;
	default_level[1].origin	            = (Vector){31, 15};
	default_level[1].produce[0]           = (Vector){8, 15};
	default_level[1].produce[1]           = (Vector){54, 15};
	default_level[1].produce[2]           = default_level[1].produce[0];
	default_level[1].produce[3]           = default_level[1].produce[1];
	default_level[1].booster[0]         = (Vector){2, 3};
	default_level[1].booster[1]         = (Vector){2, 27};
	default_level[1].booster[2]         = (Vector){60, 3};
	default_level[1].booster[3]         = (Vector){60, 27};


}
