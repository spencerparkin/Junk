// texture.h

// Programmed by Spencer T. Parkin

#ifndef __texture_h__
#define __texture_h__

#include <glut.h>

// These are the loadable textures.
enum
{
	TEX_ID_NONE,
	TEX_ID_BRICK_WALL_0,
	TEX_ID_ROCKS_0,
	TEX_ID_STONE_WALL_0,
	TEX_ID_STONE_WALL_1,
	TEX_ID_STONE_FLOOR_0,
	TEX_ID_MOON_0,
	TEX_ID_MOSAIC_0,
	TEX_ID_REPTILE_SKIN_0,
	TEX_ID_MARBLE_0,
	TEX_ID_COLIN_MOCHRIE_0,
	TEX_ID_COLIN_MOCHRIE_1,
	TEX_ID_COLIN_MOCHRIE_2,
	TEX_ID_RYAN_STILES_0,
	TEX_ID_RYAN_STILES_1,
	TEX_ID_RYAN_STILES_2,
	TEX_ID_WAYNE_BRADY_0,
	TEX_ID_WAYNE_BRADY_1,
	TEX_ID_WAYNE_BRADY_2,
	TEX_ID_GREG_PROOPS_0,
	TEX_ID_GREG_PROOPS_1,
	TEX_ID_GREG_PROOPS_2,
	TEX_ID_DREW_CAREY_0,
	TEX_ID_CLIVE_ANDERSON_0,
	TEX_ID_LEFT_DOOR_0,
	TEX_ID_LEFT_DOOR_1,
	TEX_ID_LEFT_DOOR_2,
	TEX_ID_LEFT_DOOR_3,
	TEX_ID_LEFT_DOOR_4,
	TEX_ID_LEFT_DOOR_5,
	TEX_ID_LEFT_DOOR_6,
	TEX_ID_RIGHT_DOOR_0,
	TEX_ID_RIGHT_DOOR_1,
	TEX_ID_RIGHT_DOOR_2,
	TEX_ID_RIGHT_DOOR_3,
	TEX_ID_RIGHT_DOOR_4,
	TEX_ID_RIGHT_DOOR_5,
	TEX_ID_RIGHT_DOOR_6,
	TEX_ID_WALL_SWITCH_ON_0,
	TEX_ID_WALL_SWITCH_OFF_0,
	TEX_ID_SNOW_MAN_0,
	TEX_ID_SNOW_MAN_1,
	TEX_ID_SNOW_MAN_2,
	TEX_ID_RED_STONE_WALL_0,
	TEX_ID_GREEN_STONE_WALL_0,
	TEX_ID_BLUE_STONE_WALL_0,
	TEX_ID_CYAN_STONE_WALL_0,
	TEX_ID_MAGENTA_STONE_WALL_0,
	TEX_ID_YELLOW_STONE_WALL_0,
	TEX_ID_GALAXY_0,
	TEX_ID_PLANET_0,
	TEX_ID_BOOK_SHELF_0,
	TEX_ID_SPENCER_WALK_0,
	TEX_ID_SPENCER_WALK_1,
	TEX_ID_SPENCER_WALK_2,
	TEX_ID_SPENCER_WALK_3,
	TEX_ID_SPENCER_STAND_0,
	NUM_TEX_IDS,
};

// This is my texture object that contains
// information about the texture as well as
// a handle on an OpenGL texture object.
struct texture
{
	int tex_id;
	int ref_count, use_count;
	GLuint name;
	const char *filename;
	GLubyte *data;
	GLsizei width, height;
	texture *next, *prev;
};

// The texture manager routines...
void InitTextureManager(void);
void DeinitTextureManager(void);
void ManageTextures(void);
texture *LoadTexture(int tex_id);
void UnloadTexture(texture *t);
void BeginTextureMapping(texture *t);
void EndTextureMapping(texture *t);
void UsedTexture(texture *t);

#endif __texture_h__

// endof texture.h