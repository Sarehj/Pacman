#include <stdint.h>
#include "mipslab.h"



void vector_set(Vector *v, int x, int y)
{
	v->x = x;
	v->y = y;
}

void vector_copy(Vector *v, Vector const *u)
{
	vector_set(v, u->x, u->y);
}

void vector_add(Vector *v, Vector const *u)
{
	v->x += u->x;
	v->y += u->y;
}

void vector_scalar_multiply(Vector *v, int c)
{
	v->x *= c;
	v->y *= c;
}

int vector_eq(Vector const *v, Vector const *u)
{
	return v->x == u->x && v->y == u->y;
}



//PACMAN

#define INACCESSIBLE_SQUARES (LEVEL_SQUARE_WALL | LEVEL_SQUARE_INACCESSIBLE | LEVEL_SQUARE_GHOST)


void entity_pacman_cons (Entity *pacman)
{
	entity_cons(pacman);
	pacman->icon.data   = icon_player_left;
	pacman->icon.size.x = ICON_ENTITY_SIZE;
	pacman->icon.size.y = ICON_ENTITY_SIZE;

}

void entity_pacman_move(Entity *pacman)
{
	int x = pacman->vec.x;
	int y = pacman->vec.y;

  //cehck inaccesible squares no to move
	if(level_square(&(Vector){pacman->pos.x + x, pacman->pos.y}) & INACCESSIBLE_SQUARES)
		x = 0;
	if(level_square(&(Vector){pacman->pos.x, pacman->pos.y + y}) & INACCESSIBLE_SQUARES)
		y = 0;

	vector_set(&pacman->vec, x, y);
	entity_move(pacman);
}




void entity_pacman_tick(Entity *pacman)
{
	/* Update the icon */

	if(pacman->vec.x > 0)
		pacman->icon.data = icon_pacman_right;
	else if(pacman->vec.x < 0)
		pacman->icon.data = icon_pacman_left;
	else if(pacman->vec.y > 0)
		pacman->icon.data = icon_pacman_down;
	else if(pacman->vec.y < 0)
   	pacman->icon.data = icon_pacman_up;

	entity_pacman_move(pacman);
}





//GHOSTS

#define INACCESSIBLE_SQUARES (LEVEL_SQUARE_WALL | LEVEL_SQUARE_INACCESSIBLE)
static void pathfind(Entity *);



void entity_ghost_cons (Entity *ghost)
{
	entity_cons(ghost);
	ghost->icon.data   = icon_ghost;
	ghost->icon.size.x = ICON_ENTITY_SIZE;
	ghost->icon.size.y = ICON_ENTITY_SIZE;
}



// Same function as entity_move() for the ghost with inaccessible places

void entity_ghost_move(Entity *ghost)
{
	int x = ghost->vec.x;
	int y = ghost->vec.y;

	if(level_square(&(Vector){ghost->pos.x + x, ghost->pos.y}) & INACCESSIBLE_SQUARES)
		x = 0;
	if(level_square(&(Vector){ghost->pos.x, ghost->pos.y + y}) & INACCESSIBLE_SQUARES)
		y = 0;

	vector_set(&ghost->vec, x, y);
	entity_move(ghost);
}



void entity_ghost_tick(Entity *ghost)
{
	/* Look around for alternate paths */

	pathfind(ghost);
	entity_ghost_move(ghost);
}



/* Decide which paths to take and update the movement vector
 */
static void pathfind(Entity *ghost)
{
	/* random direction in x or y */
	if(vector_eq(&ghost->vec, &(Vector){0, 0}))
	{
		int8_t *var = (rand() & 1) ? &ghost->vec.x : &ghost->vec.y;
		*var = (rand() & 1) ? -1 : 1;

		return;
	}

	// attempt to find an alternate path not to return the same direction 8 works best
  //if move in y change to x and otherwise

	Vector v, u;
	if((rand() & 8))
		return;
	else if(ghost->vec.y == 0)
		vector_set(&v, 0, 1);
	else
		vector_set(&v, 1, 0);

	/* Check if there is clear space on right/left/above/below sides */
	/* and if there is, update the movement vector */
	vector_copy(&u, &ghost->pos);
	vector_add(&u, &v);
	if(level_square(&u) & INACCESSIBLE_SQUARES)
	{
		vector_scalar_multiply(&v, -1);
		for(int i=0; i<level_active_ghosts(); i++)
		vector_add(&u, &v);

		if(level_square(&u) & INACCESSIBLE_SQUARES)
			return;
	}

	vector_copy(&ghost->vec, &v);
}
