// map.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include <math.h>
#include <stdio.h>
#include "map.h"
#include "macros.h"
#include "cell.h"
#include "surface.h"
#include "wall.h"
#include "portal.h"
#include "door.h"
#include "camera.h"
#include "artifact.h"
#include "key.h"
#include "healthpack.h"
#include "orb.h"
#include "switch.h"
#include "teleport.h"
#include "snowman.h"
#include "blob.h"
#include "spencer.h"

// This determines how cell locations and
// orientations are to be handled.
int cell_mode = STATIC_CELL_MODE;

// Load the given construct into the given cell.
static Cell *LoadConstruct(construct *con, Cell *c)
{
	Surface *s = 0;
	Entity *e = 0;
	Artifact *a = 0;
	int del_con = 0;

	switch(con->type)
	{
		case CONSTRUCT_CELL:
		{
			printf("Loading cell construct...\n");
			c = new Cell();
			con->mem = c;
			if(cell_mode == STATIC_CELL_MODE)
			{
				c->x = con->data[0];
				c->y = con->data[1];
			}
			c->stretch = (con->data[2] > 0.f ? true : false);
			c->clip = (con->data[3] > 0.f ? true : false);
			if(con->tex_id0)
				c->t_floor = LoadTexture(con->tex_id0);
			if(con->tex_id1)
				c->t_ceil = LoadTexture(con->tex_id1);
			break;
		}
		case CONSTRUCT_PORTAL:
		{
			printf("Loading portal construct...\n");
			Portal *port = new Portal();
			con->mem = port;
			s = port;
			port->l.x0 = con->data[0];
			port->l.y0 = con->data[1];
			port->l.x1 = con->data[2];
			port->l.y1 = con->data[3];
			port->adj_c = 0;		// This gets patched later.
			if(cell_mode == DYNAMIC_CELL_MODE)
			{
				port->dx = con->data[4];
				port->dy = con->data[5];
				port->da = con->data[6];
			}
			break;
		}
		case CONSTRUCT_DOOR:
		{
			printf("Loading door construct...\n");
			Door *door = new Door();
			con->mem = door;
			s = door;
			door->l.x0 = con->data[0];
			door->l.y0 = con->data[1];
			door->l.x1 = con->data[2];
			door->l.y1 = con->data[3];
			door->adj_c = 0;		// This gets patched later.
			door->d = 0;			// This gets patched later too.		
			door->is_locked = con->data[4] > 0.f ? true : false;
			if(con->tex_id0)
				door->t_left_door = LoadTexture(con->tex_id0);
			if(con->tex_id1)
				door->t_right_door = LoadTexture(con->tex_id1);
			if(cell_mode == DYNAMIC_CELL_MODE)
			{
				door->dx = con->data[5];
				door->dy = con->data[6];
				door->da = con->data[7];
			}
			break;
		}
		case CONSTRUCT_WALL:
		{
			printf("Loading wall construct...\n");
			Wall *wall = new Wall();
			con->mem = wall;
			s = wall;
			wall->l.x0 = con->data[0];
			wall->l.y0 = con->data[1];
			wall->l.x1 = con->data[2];
			wall->l.y1 = con->data[3];
			if(con->data[4] > 0.f)
				wall->clip = true;
			else
				wall->clip = false;
			if(con->data[5] > 0.f)
				wall->stretch = true;
			else
				wall->stretch = false;
			if(con->tex_id0)
			{
				wall->t = LoadTexture(con->tex_id0);
			}
			else
			{
				wall->r = con->data[5];
				wall->g = con->data[6];
				wall->b = con->data[7];
			}
			break;
		}
		case CONSTRUCT_WALL_SWITCH:
		{
			Switch *_switch = new Switch();
			con->mem = _switch;
			s = _switch;
			_switch->l.x0 = con->data[0];
			_switch->l.y0 = con->data[1];
			_switch->l.x1 = con->data[2];
			_switch->l.y1 = con->data[3];
			_switch->state = con->data[4] > 0.f ?
								SWITCH_STATE_ON :
								SWITCH_STATE_OFF;
			_switch->switch_CB = (void (*)(enum SwitchState, void *[]))con->ptr;
			if(con->tex_id0)
				_switch->t_on = LoadTexture(con->tex_id0);
			if(con->tex_id1)
				_switch->t_off = LoadTexture(con->tex_id1);
			if(_switch->state == SWITCH_STATE_ON)
				_switch->t = _switch->t_on;
			else
				_switch->t = _switch->t_off;
			break;
		}
		case CONSTRUCT_KEY:
		{
			printf("Loading key construct...\n");
			Key *key = new Key();
			con->mem = key;
			a = key;
			key->x = con->data[0];
			key->y = con->data[1];
			key->r = con->data[2];
			key->g = con->data[3];
			key->b = con->data[4];		
			key->d = 0;		// This gets patched later.
			break;
		}
		case CONSTRUCT_ORB:
		{
			printf("Loading orb construct...\n");
			Orb *orb = new Orb();
			con->mem = orb;
			a = orb;
			orb->x = con->data[0];
			orb->y = con->data[1];
			orb->orb = (OrbType)((int)con->data[2]);
			break;
		}
		case CONSTRUCT_TELEPORT:
		{
			printf("Loading teleport construct...\n");
			Teleport *teleport = new Teleport();
			con->mem = teleport;
			a = teleport;
			teleport->x = con->data[0];
			teleport->y = con->data[1];
			teleport->target_c = 0;	// This gets patched later.
			teleport->target_x = con->data[2];
			teleport->target_y = con->data[3];
			teleport->target_heading = con->data[4];
			teleport->has_power = con->data[5] > 0.f ? true : false;
			teleport->radius = 1.f;
			break;
		}
		case CONSTRUCT_HEALTH_PACK:
		{
			printf("Loading health-pack construct...\n");
			HealthPack *healthpack = new HealthPack();
			con->mem = healthpack;
			a = healthpack;
			healthpack->x = con->data[0];
			healthpack->y = con->data[1];
			healthpack->type = (HealthPackType)int(con->data[2]);
			healthpack->CalcRadius();
			break;
		}
		case CONSTRUCT_CAMERA:
		{
			if(!cam)
			{
				printf("Loading camera construct...\n");
				cam = new Camera();
				con->mem = cam;
				e = cam;
				cam->x = con->data[0];
				cam->y = con->data[1];
				cam->f.heading = con->data[2];
				cam->f.angle = 2.f * PI / 3.f;
				if(cell_mode == STATIC_CELL_MODE)
				{
					cam->f.x = c->x + cam->x;
					cam->f.y = c->y + cam->y;
				}
				else if(cell_mode == DYNAMIC_CELL_MODE)
				{
					cam->f.x = cam->x;
					cam->f.y = cam->y;
				}
				cam->radius = 0.9f;
				cam->e = 0;		// This gets patched later.
			}
			break;
		}
		case CONSTRUCT_SNOWMAN:
		{
			SnowMan *snowman = new SnowMan();
			con->mem = snowman;
			e = snowman;
			snowman->x = con->data[0];
			snowman->y = con->data[1];
			snowman->f.heading = con->data[2];
			snowman->f.angle = PI / 3.f;
			snowman->radius = 0.95f;
			snowman->target_e = 0;	// This gets patched later.
			if(con->tex_id0)
				snowman->t_happy = LoadTexture(con->tex_id0);
			else
				snowman->t_happy = 0;
			if(con->tex_id1)
				snowman->t_mad = LoadTexture(con->tex_id1);
			else
				snowman->t_mad = 0;
			// This is cheating, but explicity specify the texture here.
			snowman->t_backside = LoadTexture(TEX_ID_SNOW_MAN_2);
			break;
		}
		case CONSTRUCT_BLOB:
		{
			Blob *blob = new Blob();
			con->mem = blob;
			e = blob;
			blob->x = con->data[0];
			blob->y = con->data[1];
			blob->f.heading = 0.f;
			blob->f.angle = PI / 8.f;
			blob->radius = 0.95f;
			blob->target_e = 0;		// This gets patched later.
			break;
		}
		case CONSTRUCT_SPENCER:
		{
			if(!spencer)
			{
				spencer = new Spencer();
				con->mem = spencer;
				e = spencer;
				spencer->x = con->data[0];
				spencer->y = con->data[1];
				spencer->f.heading = con->data[2];
				spencer->f.angle = 0.f;
				spencer->radius = 0.5f;
				spencer->t_walk[0] = LoadTexture(TEX_ID_SPENCER_WALK_0);
				spencer->t_walk[1] = LoadTexture(TEX_ID_SPENCER_WALK_1);
				spencer->t_walk[2] = LoadTexture(TEX_ID_SPENCER_WALK_2);
				spencer->t_walk[3] = LoadTexture(TEX_ID_SPENCER_WALK_3);
				spencer->t_stand = LoadTexture(TEX_ID_SPENCER_STAND_0);
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
	if(c)
	{
		if(e && !SpawnEntity(e, c))
			del_con = 1;

		if(s)
		{
			if(!c->InsertSurface(s))
				del_con = 1;
			else
			{
				calc_line_norm(&s->l, 0.f, 0.f);
				s->length = sqrt(line_sqr_dist(&s->l));
			}
		}

		if(a && !c->InsertArtifact(a))
			del_con = 1;
	}

	// If something went wrong during the load
	// process, delete the construct memory, if any.
	if(del_con && con->mem)
	{
		printf("(!!)WARNING(!!): Construct type %d failed to load!\n", con->type);
		getchar();
		delete con->mem;
		con->mem = 0;
	}

	return c;
}

// Construct patching allows us to associate
// constructs in the scene with other constructs.
static void PatchConstruct(construct *con, construct *target_con0, construct *target_con1)
{
	switch(con->type)
	{
		case CONSTRUCT_PORTAL:
		{
			Portal *port = (Portal *)con->mem;
			Cell *c = (Cell *)target_con0->mem;
			port->adj_c = c;
			break;
		}
		case CONSTRUCT_DOOR:
		{
			Door *door = (Door *)con->mem;
			Cell *c = (Cell *)target_con0->mem;
			Door *other_door = (Door *)target_con1->mem;
			door->adj_c = c;
			door->d = other_door;
			break;
		}
		case CONSTRUCT_WALL_SWITCH:
		{
			Switch *_switch = (Switch *)con->mem;
			_switch->data[0] = target_con0 ? target_con0->mem : 0;
			_switch->data[1] = target_con1 ? target_con1->mem : 0;
			break;
		}
		case CONSTRUCT_KEY:
		{
			Key *key = (Key *)con->mem;
			Door *door = (Door *)target_con0->mem;
			key->d = door;
			break;
		}
		case CONSTRUCT_TELEPORT:
		{
			Teleport *teleport = (Teleport *)con->mem;
			Cell *c = (Cell *)target_con0->mem;
			teleport->target_c = c;
			break;
		}
		case CONSTRUCT_CAMERA:
		{
			Camera *cam = (Camera *)con->mem;
			Entity *e = (Entity *)target_con0->mem;
			cam->e = e;		// Set the camera's subject.
			break;
		}
		case CONSTRUCT_SNOWMAN:
		{
			SnowMan *snowman = (SnowMan *)con->mem;
			Entity *e = (Entity *)target_con0->mem;
			snowman->target_e = e;	// Set the snowman's hunting target.
			break;
		}
		case CONSTRUCT_BLOB:
		{
			Blob *blob = (Blob *)con->mem;
			Entity *e = (Entity *)target_con0->mem;
			blob->target_e = e;		// Set the blob's hunting target.
			break;
		}
		default:
		{
			break;
		}
	}
}

// Load the given map.
bool LoadMap(map *m)
{
	int k;

	// Determine what cell mode to use.
	cell_mode = m->cell_mode;

	// Load the constructs of the given map into cells.
	Cell *c = 0;
	for(k = 0; k < m->con_count; k++)
	{
		if(m->con_list[k].type != CONSTRUCT_FUDGE)
			c = LoadConstruct(&m->con_list[k], c);
		else
			m->con_list[k].mem = 0;
	}

	// Patch together constructs that are associated with one another.
	int index0, index1;
	for(k = 0; k < m->con_count; k++)
	{
		if(m->con_list[k].type == CONSTRUCT_FUDGE)
			continue;

		index0 = m->con_list[k].index0;
		index1 = m->con_list[k].index1;
		if(index0 >= 0 && index0 < m->con_count)
		{
			if(index1 >= 0 && index1 < m->con_count)
			{
				PatchConstruct(&m->con_list[k],
							&m->con_list[index0],
							&m->con_list[index1]);
			}
			else
			{
				PatchConstruct(&m->con_list[k],
							&m->con_list[index0],
							0);
			}
		}
	}

	return true;
}

// Free all of the memory allocated by
// the map-loading routine.
bool UnloadMap(map *m)
{
	int k;
	for(k = 0; k < m->con_count; k++)
		if(m->con_list[k].mem)
			delete m->con_list[k].mem;
	return true;
}

// This is a simple debug map...
construct debug_map_constructs[] =
{
	{CONSTRUCT_CELL, -1, -1, {0.f, 0.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MOSAIC_0},
	{CONSTRUCT_CAMERA, -1, -1, {0.f, 0.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {-5.f, -5.f, -2.f, -5.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {2.f, -5.f, 5.f, -5.f, 1.f, 0.f, 0.f}, 0, TEX_ID_REPTILE_SKIN_0, TEX_ID_NONE},
	{CONSTRUCT_DOOR, 13, 14, {-2.f, -5.f, 2.f, -5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {-5.f, 5.f, 5.f, 5.f, 0.f, 1.f, 0.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {-5.f, 5.f, -5.f, -5.f, 0.f, 0.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	{CONSTRUCT_PORTAL, 8, -1, {5.f, 5.f, 5.f, -5.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	{CONSTRUCT_CELL, -1, -1, {7.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	{CONSTRUCT_WALL, -1, -1, {-2.f, 5.f, 3.f, 0.f, 0.5f, 0.5f, 0.5f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {-2.f, -5.f, 3.f, 0.f, 0.4f, 0.4f, 0.4f}, 0, TEX_ID_ROCKS_0, TEX_ID_NONE},
	{CONSTRUCT_KEY, 4, -1, {0.f, 0.f, 1.f, 1.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	{CONSTRUCT_PORTAL, 0, -1, {-2.f, 5.f, -2.f, -5.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	{CONSTRUCT_CELL, -1, -1, {0.f, -7.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	{CONSTRUCT_DOOR, 0, 4, {-2.f, 2.f, 2.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, -3.f, -3.f, 1.f, 1.f, 1.f}, 0, TEX_ID_COLIN_MOCHRIE_0, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 3.f, -3.f, 1.f, 1.f, 1.f}, 0, TEX_ID_COLIN_MOCHRIE_1, TEX_ID_NONE},
	{CONSTRUCT_WALL, -1, -1, {-3.f, -3.f, 3.f, -3.f, 1.f, 1.f, 1.f}, 0, TEX_ID_COLIN_MOCHRIE_2, TEX_ID_NONE},
};
map debug_map =
{
	debug_map_constructs,
	sizeof(debug_map_constructs) / sizeof(construct),
	STATIC_CELL_MODE,
};

construct debug_map2_constructs[] =
{
	/* 000 */ {CONSTRUCT_CELL, -1, -1, {0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 001 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, -1.f, 4.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 002 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 2.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 003 */ {CONSTRUCT_PORTAL, 5, -1, {-1.f, 4.f, 2.f, 1.f, 3.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 004 */ {CONSTRUCT_PORTAL, 30, -1, {-1.f, -2.f, 2.f, -2.f, 1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 005 */ {CONSTRUCT_CELL, -1, -1, {1.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 006 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 3.f, -1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 007 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 2.f, 3.f, 2.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 008 */ {CONSTRUCT_PORTAL, 0, -1, {-1.f, -1.f, -4.f, 2.f, -3.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 009 */ {CONSTRUCT_PORTAL, 15, -1, {3.f, -1.f, 3.f, 2.f, 4.f, 0.f, PI}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 010 */ {CONSTRUCT_CELL, -1, -1, {2.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 011 */ {CONSTRUCT_WALL, -1, -1, {0.f, -2.f, 2.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 012 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, 1.f, 3.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 013 */ {CONSTRUCT_PORTAL, 20, -1, {-2.f, 0.f, 0.f, -2.f, -2.f, -2.f, -PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 014 */ {CONSTRUCT_PORTAL, 15, -1, {2.f, 0.f, 1.f, 3.f, 2.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 015 */ {CONSTRUCT_CELL, -1, -1, {3.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 016 */ {CONSTRUCT_WALL, -1, -1, {0.f, -2.f, 1.f, -2.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 017 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, 1.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 018 */ {CONSTRUCT_PORTAL, 10, -1, {0.f, -2.f, -1.f, 1.f, -2.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 019 */ {CONSTRUCT_PORTAL, 5, -1, {1.f, -2.f, 1.f, 1.f, 4.f, 0.f, PI}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 020 */ {CONSTRUCT_CELL, -1, -1, {4.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 021 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, 0.f, -2.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 022 */ {CONSTRUCT_WALL, -1, -1, {0.f, 2.f, 1.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 023 */ {CONSTRUCT_PORTAL, 10, -1, {-2.f, 0.f, 0.f, 2.f, -2.f, 2.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 024 */ {CONSTRUCT_PORTAL, 25, -1, {0.f, -2.f, 1.f, 1.f, 3.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 025 */ {CONSTRUCT_CELL, -1, -1, {5.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 026 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, 1.f, 2.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 027 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -1.f, 4.f, -1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 028 */ {CONSTRUCT_PORTAL, 20, -1, {-3.f, -1.f, -2.f, 2.f, -3.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 029 */ {CONSTRUCT_PORTAL, 30, -1, {4.f, -1.f, 1.f, 2.f, 3.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 030 */ {CONSTRUCT_CELL, -1, -1, {6.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 031 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 032 */ {CONSTRUCT_WALL, -1, -1, {1.f, -3.f, 1.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 033 */ {CONSTRUCT_PORTAL, 25, -1, {-2.f, 0.f, 1.f, -3.f, -3.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 034 */ {CONSTRUCT_PORTAL, 0, -1, {-2.f, 1.f, 1.f, 1.f, -1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 035 */ {CONSTRUCT_CAMERA, -1, -1, {0.f, 0.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
};

map debug_map2 =
{
	debug_map2_constructs,
	sizeof(debug_map2_constructs) / sizeof(construct),
	DYNAMIC_CELL_MODE,
};

extern map level1_map;
extern map level2_map;
extern map level3_map;
extern map level4_map;

map *game_map[] =
{
	&level1_map,
	&level2_map,
	&level3_map,
	&level4_map,
};

const int num_game_maps = sizeof(game_map) / sizeof(map *);

// endof map.cpp