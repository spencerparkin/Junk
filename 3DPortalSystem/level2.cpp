// level2.cpp

// Programmed by Spencer T. Parkin

// Notice that to save time, not all cells
// have been constructed as a convex polygon
// part or slightly concave.  They're not
// so concave, however, that more than one
// wall may needlessly draw.  Note also that
// I did get burned by breaking this rule.
// But I think I've fixed the problem.

// BUG: A bug with this map is that two of
// the portals in the big open room with
// the pillars point to cells that may be
// occupied.  This causes nothign to be
// drawn through them since the cell flags
// are set.  But if you approach the portals
// closer, then you'll eventually see what's
// through them.

// BUG: The bug with this map is that I haven't
// figured out the best drawing algorithm for
// the floor and ceiling yet.  If I could draw
// only what of the floor and ceiling is visible,
// then it would illiminate the bug with this
// map which can be seen by going into the large
// open area with pillars everywhere.

// After doing all of this by hand, (yes, I'm
// embarassed to say that I don't have a life
// and did this all by hand), I wish I would
// have taken the time to write a level editor.
// Maybe I'll still do that.

#include "map.h"
#include "switch.h"
#include "spencer.h"
#include "camera.h"
#include "macros.h"
#include "portal.h"
#include "healthpack.h"
#include "door.h"
#include "key.h"
#include "orb.h"
#include "teleport.h"

struct level2_mat_data
{
	int cell_index;			// Cell to patch portal to.
	int portal_index;		// Portal to disable.
	float dx, dy, da;
}		// I may have indexed this matrix incorrectly.
level2_switch_matrix[2][2] =
{
	// 0?
	{
		// 00
		{
			74, 78,
			0.f, 4.f, 0.f,
		},
		// 01
		{
			154, 158,
			0.f, 3.f, 0.f,
		},
	},
	// 1?
	{
		// 10
		{
			164, 168,
			0.f, 6.f, 0.f,
		},
		// 11
		{
			434, 440,
			0.f, 5.f, 0.f,
		},
	},
};

// In the map, both switches default to zero.
static int switch0 = 0;
static int switch1 = 0;

void update_level2_portal(void)
{
	level2_mat_data *data = &level2_switch_matrix[switch0][switch1];
	extern construct level2_map_constructs[];
	Portal *p = (Portal *)level2_map_constructs[data->portal_index].mem;
//	p->no_pass = true;		// Oops, these flags should have been set from the beginning.  Oh-well.  This is a mess.
	p = (Portal *)level2_map_constructs[10].mem;	// The portal in the map we patch.
//	p->no_pass = false;
	p->adj_c = (Cell *)level2_map_constructs[data->cell_index].mem;
	p->dx = data->dx;
	p->dy = data->dy;
	p->da = data->da;
}

void level2_switch0(SwitchState state, void *data[2])
{
	switch0 = (state == SWITCH_STATE_ON);
	update_level2_portal();
}

void level2_switch1(SwitchState state, void *data[2])
{
	switch1 = (state == SWITCH_STATE_ON);
	update_level2_portal();
}

void level2_generic_switch(SwitchState state, void *data[2])
{
	Door *d = (Door *)data[0];
	if(state == SWITCH_STATE_ON)
		d->UnlockDoor();
	else
		d->LockDoor();
}

void level2_telepad_switch(SwitchState state, void *data[2])
{
	Teleport *tele = (Teleport *)data[0];
	tele->has_power = (state == SWITCH_STATE_ON) ? true : false;
}

construct level2_map_constructs[] =
{
	/*
	 *
	 * Main section:
	 *
	 */

	/* 000 */ {CONSTRUCT_CELL, -1, -1, {0.f, 0.f, 1.f, 1.f}, 0, TEX_ID_MOON_0, TEX_ID_PLANET_0},
	/* 001 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 002 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 003 */ {CONSTRUCT_WALL, -1, -1, {-4.f, -1.f, -3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 004 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 1.f, -3.f, 2.f, 1.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 005 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.998f, -1.001f, 1.998f, 1.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 006 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 007 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 3.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 008 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-3.f, -2.f, -1.f, -2.f, 0.f}, level2_switch0, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 009 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {0.f, -2.f, 2.f, -2.f, 0.f}, level2_switch1, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 010 */ {CONSTRUCT_PORTAL, 74, -1, {-1.f, 2.f, 1.f, 2.f, 0.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 011 */ {CONSTRUCT_DOOR, 42, 46, {3.f, 1.f, 3.f, -1.f, 1.f, 6.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_4, TEX_ID_RIGHT_DOOR_4},
	/* 012 */ {CONSTRUCT_DOOR, 18, 21, {-4.f, -1.f, -4.f, 1.f, 1.f, -6.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_2, TEX_ID_RIGHT_DOOR_2},
	/* 013 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 014 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 015 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 016 */ {CONSTRUCT_SPENCER, -1, -1, {0.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 017 */ {CONSTRUCT_CAMERA, 16, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 018 */ {CONSTRUCT_CELL, -1, -1, {1.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 019 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 020 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 021 */ {CONSTRUCT_DOOR, 0, 12, {2.f, -1.f, 2.f, 1.f, 1.f, 6.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_1, TEX_ID_RIGHT_DOOR_1},
	/* 022 */ {CONSTRUCT_PORTAL, 28, -1, {-1.f, -1.f, -1.f, 1.f, -4.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 023 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 024 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 025 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 026 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 027 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 028 */ {CONSTRUCT_CELL, -1, -1, {2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_REPTILE_SKIN_0},
	/* 029 */ {CONSTRUCT_WALL, -1, -1, {3.f, 0.f, 2.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 030 */ {CONSTRUCT_WALL, -1, -1, {2.f, 3.f, -1.f, 3.f, 0.f, 1.f}, 0, TEX_ID_RYAN_STILES_2, TEX_ID_NONE},
	/* 031 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 3.f, -3.f, 1.f, 0.f, 1.f}, 0, TEX_ID_RYAN_STILES_1, TEX_ID_NONE},
	/* 032 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.f, -3.f, -2.f, 0.f, 1.f}, 0, TEX_ID_RYAN_STILES_0, TEX_ID_NONE},
	/* 033 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -2.f, -2.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 034 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -3.f, 2.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 035 */ {CONSTRUCT_WALL, -1, -1, {2.f, -3.f, 3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 036 */ {CONSTRUCT_PORTAL, 18, -1, {3.f, 0.f, 3.f, -2.f, 4.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 037 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 038 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 039 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 040 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 041 */ {CONSTRUCT_ORB, -1, -1, {0.f, 0.f, (float)ORB_OF_STILES}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 042 */ {CONSTRUCT_CELL, -1, -1, {3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 043 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -1.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 044 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 045 */ {CONSTRUCT_WALL, -1, -1, {3.f, -1.f, 3.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 046 */ {CONSTRUCT_DOOR, 0, 11, {-3.f, -1.f, -3.f, 1.f, 1.f, -6.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_2, TEX_ID_RIGHT_DOOR_2},
	/* 047 */ {CONSTRUCT_PORTAL, 53, -1, {1.f, 1.f, 3.f, 1.f, 2.f, 12.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 048 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 049 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 050 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 051 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 052 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 053 */ {CONSTRUCT_CELL, -1, -1, {4.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 054 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -11.f, -1.f, 11.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 055 */ {CONSTRUCT_WALL, -1, -1, {1.f, -11.f, 1.f, 11.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 056 */ {CONSTRUCT_PORTAL, 42, -1, {-1.f, -11.f, 1.f, -11.f, -2.f, -12.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 057 */ {CONSTRUCT_PORTAL, 63, -1, {-1.f, 11.f, 1.f, 11.f, -1.f, 12.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 058 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 059 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 060 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 061 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 062 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 063 */ {CONSTRUCT_CELL, -1, -1, {5.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 064 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 065 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 066 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 0.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 067 */ {CONSTRUCT_WALL_SWITCH, 12, -1, {-2.f, -1.f, -2.f, 1.f, 0.f}, level2_generic_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 068 */ {CONSTRUCT_PORTAL, 53, -1, {0.f, -1.f, 2.f, -1.f, 1.f, -12.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 069 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 070 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 071 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 072 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 073 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/*
	 *
	 * Section 00:
	 *
	 */

	/* 074 */ {CONSTRUCT_CELL, -1, -1, {0.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 075 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, -1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 076 */ {CONSTRUCT_WALL, -1, -1, {1.f, -2.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 077 */ {CONSTRUCT_WALL, -1, -1, {1.f, 1.f, 0.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 078 */ {CONSTRUCT_PORTAL, 0, -1, {-1.f, -2.f, 1.f, -2.f, 0.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 079 */ {CONSTRUCT_PORTAL, 85, -1, {-1.f, 1.f, 0.f, 3.f, -1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 080 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 081 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 082 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 083 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 084 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 085 */ {CONSTRUCT_CELL, -1, -1, {1.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 086 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 087 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, 0.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 088 */ {CONSTRUCT_PORTAL, 74, -1, {0.f, -2.f, 1.f, 0.f, 1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 089 */ {CONSTRUCT_PORTAL, 95, -1, {0.f, 2.f, 1.f, 0.f, 1.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 090 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 091 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 092 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 093 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 094 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 095 */ {CONSTRUCT_CELL, -1, -1, {2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 096 */ {CONSTRUCT_WALL, -1, -1, {0.f, -2.f, 1.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 097 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 098 */ {CONSTRUCT_WALL, -1, -1, {1.f, 0.f, 0.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 099 */ {CONSTRUCT_PORTAL, 85, -1, {-1.f, 0.f, 0.f, -2.f, -1.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 100 */ {CONSTRUCT_PORTAL, 106, -1, {-2.f, 2.f, 0.f, 2.f, -1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 101 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 102 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 103 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 104 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 105 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 106 */ {CONSTRUCT_CELL, -1, -1, {3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 107 */ {CONSTRUCT_WALL, -1, -1, {-5.f, -1.f, -3.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 108 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.f, 4.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 109 */ {CONSTRUCT_WALL, -1, -1, {4.f, 1.f, 6.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 110 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 111 */ {CONSTRUCT_WALL, -1, -1, {1.f, -1.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 112 */ {CONSTRUCT_PORTAL, 95, -1, {-1.f, -1.f, 1.f, -1.f, 1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 113 */ {CONSTRUCT_PORTAL, 120, -1, {-5.f, -1.f, -2.f, -1.f, -4.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 114 */ {CONSTRUCT_PORTAL, 130, -1, {3.f, -1.f, 6.f, -1.f, 4.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 115 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 116 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 117 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 118 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 119 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 120 */ {CONSTRUCT_CELL, -1, -1, {4.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 121 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 122 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 123 */ {CONSTRUCT_PORTAL, 106, -1, {-1.f, 2.f, 2.f, 2.f, 4.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 124 */ {CONSTRUCT_PORTAL, 140, -1, {-1.f, -1.f, 2.f, -1.f, 7.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 125 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 126 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 127 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 128 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 129 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 130 */ {CONSTRUCT_CELL, -1, -1, {5.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 131 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 132 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 133 */ {CONSTRUCT_PORTAL, 106, -1, {-1.f, 2.f, 2.f, 2.f, -4.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 134 */ {CONSTRUCT_PORTAL, 140, -1, {-1.f, -1.f, 2.f, -1.f, -1.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 135 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 136 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 137 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 138 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 139 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 140 */ {CONSTRUCT_CELL, -1, -1, {6.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 141 */ {CONSTRUCT_WALL, -1, -1, {-8.f, -3.f, -8.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 142 */ {CONSTRUCT_WALL, -1, -1, {3.f, -3.f, 3.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 143 */ {CONSTRUCT_WALL, -1, -1, {-5.f, 3.f, 0.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 144 */ {CONSTRUCT_WALL, -1, -1, {-6.f, -3.f, 1.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 145 */ {CONSTRUCT_WALL_SWITCH, 455, -1, {-8.f, -3.f, -6.f, -3.f, 0.f}, level2_generic_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 146 */ {CONSTRUCT_WALL_SWITCH, 508, -1, {1.f, -3.f, 3.f, -3.f, 0.f}, level2_generic_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 147 */ {CONSTRUCT_PORTAL, 120, -1, {-8.f, 3.f, -5.f, 3.f, -7.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 148 */ {CONSTRUCT_PORTAL, 130, -1, {0.f, 3.f, 3.f, 3.f, 1.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 149 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 150 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 151 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 152 */ {CONSTRUCT_HEALTH_PACK, -1, -1, {-5.f, 0.f, (float)MEGA_HEALTH_PACK}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 153 */ {CONSTRUCT_HEALTH_PACK, -1, -1, {0.f, 0.f, (float)MEGA_HEALTH_PACK}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/*
	 *
	 * Section 01:
	 *
	 */

	/* 154 */ {CONSTRUCT_CELL, -1, -1, {0.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 155 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, -1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 156 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 157 */ {CONSTRUCT_WALL, -1, -1, {1.f, 1.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 158 */ {CONSTRUCT_PORTAL, 0, -1, {-1.f, -1.f, 1.f, -1.f, 0.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 159 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 160 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 161 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 162 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 163 */ {CONSTRUCT_KEY, 389, -1, {0.f, 0.f, 1.f, 1.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/*
	 *
	 * Section 10:
	 *
	 */

	/* 164 */ {CONSTRUCT_CELL, -1, -1, {0.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 165 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -4.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 166 */ {CONSTRUCT_WALL, -1, -1, {1.f, -4.f, 1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 167 */ {CONSTRUCT_WALL, -1, -1, {1.f, 4.f, -1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 168 */ {CONSTRUCT_PORTAL, 0, -1, {-1.f, -4.f, 1.f, -4.f, 0.f, -6.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 169 */ {CONSTRUCT_PORTAL, 175, -1, {-1.f, 2.f, -1.f, 4.f, -3.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 170 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 171 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 172 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 173 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 174 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 175 */ {CONSTRUCT_CELL, -1, -1, {1.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 176 */ {CONSTRUCT_WALL, -1, -1, {0.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 177 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 178 */ {CONSTRUCT_PORTAL, 164, -1, {2.f, 1.f, 2.f, -1.f, 3.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 179 */ {CONSTRUCT_PORTAL, 185, -1, {-2.f, -1.f, 0.f, -1.f, -1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 180 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 181 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 182 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 183 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 184 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 185 */ {CONSTRUCT_CELL, -1, -1, {2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 186 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 187 */ {CONSTRUCT_WALL, -1, -1, {1.f, 0.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 188 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, 1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 189 */ {CONSTRUCT_PORTAL, 175, -1, {-1.f, 2.f, 1.f, 2.f, 1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 190 */ {CONSTRUCT_PORTAL, 196, -1, {1.f, 0.f, 1.f, -2.f, 6.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 191 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 192 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 193 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 194 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 195 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 196 */ {CONSTRUCT_CELL, -1, -1, {3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 197 */ {CONSTRUCT_WALL, -1, -1, {-5.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 198 */ {CONSTRUCT_WALL, -1, -1, {-5.f, 1.f, 0.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 199 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 200 */ {CONSTRUCT_PORTAL, 185, -1, {-5.f, -1.f, -5.f, 1.f, -6.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 201 */ {CONSTRUCT_PORTAL, 207, -1, {0.f, 1.f, 2.f, 1.f, 1.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 202 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 203 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 204 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 205 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 206 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 207 */ {CONSTRUCT_CELL, -1, -1, {4.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 208 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -3.f, -1.f, 3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 209 */ {CONSTRUCT_WALL, -1, -1, {1.f, -3.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 210 */ {CONSTRUCT_WALL, -1, -1, {1.f, 1.f, 1.f, 3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 211 */ {CONSTRUCT_PORTAL, 218, -1, {-1.f, 3.f, 1.f, 3.f, -1.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 212 */ {CONSTRUCT_PORTAL, 196, -1, {-1.f, -3.f, 1.f, -3.f, -1.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 213 */ {CONSTRUCT_PORTAL, 324, -1, {1.f, -1.f, 1.f, 1.f, 3.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 214 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 215 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 216 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 217 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 218 */ {CONSTRUCT_CELL, -1, -1, {5.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 219 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -1.f, 0.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 220 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.f, 0.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 221 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 5.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 222 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 5.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 223 */ {CONSTRUCT_WALL, -1, -1, {-7.f, -1.f, -5.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 224 */ {CONSTRUCT_WALL, -1, -1, {-7.f, 1.f, -5.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 225 */ {CONSTRUCT_PORTAL, 207, -1, {0.f, -1.f, 2.f, -1.f, 1.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 226 */ {CONSTRUCT_PORTAL, 236, -1, {0.f, 1.f, 2.f, 1.f, 1.f, 5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 227 */ {CONSTRUCT_PORTAL, 282, -1, {5.f, -1.f, 5.f, 1.f, 6.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 228 */ {CONSTRUCT_PORTAL, 314, -1, {-5.f, 1.f, -3.f, 1.f, -4.f, 3.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 229 */ {CONSTRUCT_PORTAL, 346, -1, {-5.f, -1.f, -3.f, -1.f, -4.f, -3.f, -PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 230 */ {CONSTRUCT_PORTAL, 356, -1, {-7.f, -1.f, -7.f, 1.f, -8.f, 6.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 231 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 232 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 233 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 234 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 235 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 236 */ {CONSTRUCT_CELL, -1, -1, {6.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 237 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -4.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 238 */ {CONSTRUCT_WALL, -1, -1, {1.f, -4.f, 1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 239 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 240 */ {CONSTRUCT_WALL, -1, -1, {1.f, 0.f, 1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 241 */ {CONSTRUCT_WALL, -1, -1, {1.f, 4.f, -1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 242 */ {CONSTRUCT_PORTAL, 271, -1, {-1.f, -2.f, -1.f, 0.f, -3.f, -1.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 243 */ {CONSTRUCT_PORTAL, 251, -1, {1.f, 0.f, 1.f, -2.f, 2.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 244 */ {CONSTRUCT_PORTAL, 261, -1, {-1.f, 2.f, -1.f, 4.f, -2.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 245 */ {CONSTRUCT_PORTAL, 218, -1, {-1.f, -4.f, 1.f, -4.f, -1.f, -5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 246 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 247 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 248 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 249 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 250 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 251 */ {CONSTRUCT_CELL, -1, -1, {7.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 252 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 253 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 254 */ {CONSTRUCT_PORTAL, 236, -1, {-1.f, -1.f, -1.f, 1.f, -2.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 255 */ {CONSTRUCT_PORTAL, 356, -1, {1.f, -1.f, 1.f, 1.f, 2.f, 0.f, PI}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 256 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 257 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 258 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 259 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 260 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 261 */ {CONSTRUCT_CELL, -1, -1, {8.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 262 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 263 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 264 */ {CONSTRUCT_PORTAL, 271, -1, {-1.f, -1.f, -1.f, 1.f, -2.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 265 */ {CONSTRUCT_PORTAL, 236, -1, {1.f, -1.f, 1.f, 1.f, 2.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 266 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 267 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 268 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 269 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 270 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 271 */ {CONSTRUCT_CELL, -1, -1, {9.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 272 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 273 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 274 */ {CONSTRUCT_WALL, -1, -1, {1.f, -2.f, 1.f, 0.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 275 */ {CONSTRUCT_PORTAL, 261, -1, {1.f, 0.f, 1.f, 2.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 276 */ {CONSTRUCT_PORTAL, 236, -1, {-1.f, -2.f, 1.f, -2.f, 1.f, -3.f, -PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 277 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 278 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 279 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 280 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 281 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 282 */ {CONSTRUCT_CELL, -1, -1, {10.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 283 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, -1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 284 */ {CONSTRUCT_WALL, -1, -1, {1.f, -3.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 285 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 4.f, 1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 286 */ {CONSTRUCT_PORTAL, 218, -1, {-1.f, -3.f, -1.f, -1.f, -6.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 287 */ {CONSTRUCT_PORTAL, 293, -1, {1.f, 2.f, 1.f, 4.f, 3.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 288 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -3.f, 1.f, -3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 289 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 290 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 291 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 292 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 293 */ {CONSTRUCT_CELL, -1, -1, {11.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 294 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 295 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 0.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 296 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 297 */ {CONSTRUCT_PORTAL, 282, -1, {-2.f, -1.f, -2.f, 1.f, -3.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 298 */ {CONSTRUCT_PORTAL, 304, -1, {0.f, -1.f, 2.f, -1.f, 1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 299 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 300 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 301 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 302 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 303 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 304 */ {CONSTRUCT_CELL, -1, -1, {12.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 305 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, -1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 306 */ {CONSTRUCT_WALL, -1, -1, {1.f, -1.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 307 */ {CONSTRUCT_PORTAL, 293, -1, {-1.f, 2.f, 1.f, 2.f, -1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 308 */ {CONSTRUCT_PORTAL, 314, -1, {-1.f, -1.f, 1.f, -1.f, -1.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 309 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 310 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 311 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 312 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 313 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 314 */ {CONSTRUCT_CELL, -1, -1, {13.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 315 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 316 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 0.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 317 */ {CONSTRUCT_PORTAL, 304, -1, {0.f, 1.f, 2.f, 1.f, 1.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 318 */ {CONSTRUCT_PORTAL, 218, -1, {-2.f, -1.f, -2.f, 1.f, -3.f, -4.f, -PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 319 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 320 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 321 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 322 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 323 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 324 */ {CONSTRUCT_CELL, -1, -1, {14.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 325 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 0.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 326 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 327 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 328 */ {CONSTRUCT_PORTAL, 207, -1, {-2.f, -1.f, -2.f, 1.f, -3.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 329 */ {CONSTRUCT_PORTAL, 335, -1, {0.f, -1.f, 2.f, -1.f, 1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 330 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 331 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 332 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 333 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 334 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 335 */ {CONSTRUCT_CELL, -1, -1, {15.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 336 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 337 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 338 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, 1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 339 */ {CONSTRUCT_PORTAL, 324, -1, {-1.f, 2.f, 1.f, 2.f, -1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 340 */ {CONSTRUCT_PORTAL, 346, -1, {-1.f, 0.f, -1.f, -2.f, -13.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 341 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 342 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 343 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 344 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 345 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 346 */ {CONSTRUCT_CELL, -1, -1, {16.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 347 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 12.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 348 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 12.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 349 */ {CONSTRUCT_PORTAL, 335, -1, {12.f, -1.f, 12.f, 1.f, 13.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 350 */ {CONSTRUCT_PORTAL, 218, -1, {-2.f, -1.f, -2.f, 1.f, -3.f, 4.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 351 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 352 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 353 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 354 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 355 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 356 */ {CONSTRUCT_CELL, -1, -1, {17.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 357 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -7.f, -1.f, 8.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 358 */ {CONSTRUCT_WALL, -1, -1, {1.f, -5.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 359 */ {CONSTRUCT_WALL, -1, -1, {1.f, 1.f, 1.f, 6.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 360 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 8.f, 1.f, 8.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 361 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -7.f, 1.f, -7.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 362 */ {CONSTRUCT_PORTAL, 251, -1, {1.f, -1.f, 1.f, 1.f, 2.f, 0.f, PI}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 363 */ {CONSTRUCT_PORTAL, 218, -1, {1.f, -5.f, 1.f, -7.f, 8.f, -6.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 364 */ {CONSTRUCT_PORTAL, 370, -1, {1.f, 6.f, 1.f, 8.f, 4.f, 7.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 365 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 366 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 367 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 368 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 369 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 370 */ {CONSTRUCT_CELL, -1, -1, {18.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 371 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.f, 3.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 372 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -1.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 373 */ {CONSTRUCT_PORTAL, 356, -1, {-3.f, -1.f, -3.f, 1.f, -4.f, -7.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 374 */ {CONSTRUCT_DOOR, 380, 389, {3.f, 1.f, 3.f, -1.f, 1.f, 7.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_2, TEX_ID_RIGHT_DOOR_2},
	/* 375 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 376 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 377 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 378 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 379 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 380 */ {CONSTRUCT_CELL, -1, -1, {19.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 381 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 1.f, -4.f, 3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 382 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 3.f, 0.f, 3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 383 */ {CONSTRUCT_WALL_SWITCH, 396, -1, {0.f, 3.f, 2.f, 3.f, 0.f}, level2_telepad_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 384 */ {CONSTRUCT_WALL, -1, -1, {2.f, 3.f, 4.f, 3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 385 */ {CONSTRUCT_WALL, -1, -1, {4.f, 0.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 386 */ {CONSTRUCT_WALL, -1, -1, {3.f, -1.f, 3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 387 */ {CONSTRUCT_WALL, -1, -1, {0.f, -3.f, -4.f, -3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 388 */ {CONSTRUCT_WALL, -1, -1, {-4.f, -3.f, -4.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 389 */ {CONSTRUCT_DOOR, 370, 374, {-4.f, -1.f, -4.f, 1.f, 1.f, -7.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_3, TEX_ID_RIGHT_DOOR_3},
	/* 390 */ {CONSTRUCT_PORTAL, 397, -1, {0.f, -3.f, 3.f, -2.f, 3.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 391 */ {CONSTRUCT_PORTAL, 422, -1, {4.f, 0.f, 4.f, 3.f, 6.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 392 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 393 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 394 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 395 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 396 */ {CONSTRUCT_TELEPORT, 0, -1, {0.f, 0.f, 0.f, 0.f, PI / 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},	// put active/inactive textures here

	/* 397 */ {CONSTRUCT_CELL, -1, -1, {20.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 398 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -2.f, -3.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 399 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -2.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 400 */ {CONSTRUCT_WALL, -1, -1, {0.f, 2.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 401 */ {CONSTRUCT_WALL, -1, -1, {1.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 402 */ {CONSTRUCT_PORTAL, 380, -1, {-3.f, 1.f, 0.f, 2.f, -3.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 403 */ {CONSTRUCT_PORTAL, 409, -1, {2.f, 1.f, 2.f, -2.f, 4.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 404 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 405 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 406 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 407 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 408 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 409 */ {CONSTRUCT_CELL, -1, -1, {21.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 410 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 411 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 412 */ {CONSTRUCT_WALL_SWITCH, 655, -1, {2.f, 1.f, 2.f, 3.f, 0.f}, level2_generic_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 413 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 414 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, -1.f, 3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 415 */ {CONSTRUCT_PORTAL, 397, -1, {-2.f, -2.f, -2.f, 1.f, -4.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 416 */ {CONSTRUCT_PORTAL, 422, -1, {-1.f, 3.f, 2.f, 3.f, -1.f, 5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 417 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 418 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 419 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 420 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 421 */ {CONSTRUCT_SNOWMAN, 16, -1, {0.f, 0.f, 0.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},

	/* 422 */ {CONSTRUCT_CELL, -1, -1, {22.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 423 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 424 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 425 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, 3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 426 */ {CONSTRUCT_WALL, -1, -1, {3.f, -2.f, 3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 427 */ {CONSTRUCT_PORTAL, 380, -1, {-2.f, -1.f, -2.f, 2.f, -6.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 428 */ {CONSTRUCT_PORTAL, 409, -1, {0.f, -2.f, 3.f, -2.f, 1.f, -5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 429 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 430 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 431 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 432 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 433 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/*
	 *
	 * Section 11:
	 *
	 */

	/* 434 */ {CONSTRUCT_CELL, -1, -1, {0.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 435 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -3.f, -1.f, -3.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 436 */ {CONSTRUCT_WALL, -1, -1, {1.f, -3.f, 2.f, -3.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 437 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 1.f, -4.f, 3.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 438 */ {CONSTRUCT_WALL, -1, -1, {0.f, 4.f, 2.f, 4.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 439 */ {CONSTRUCT_WALL, -1, -1, {5.f, 1.f, 5.f, -1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 440 */ {CONSTRUCT_PORTAL, 0, -1, {-1.f, -3.f, 1.f, -3.f, 0.f, -5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 441 */ {CONSTRUCT_PORTAL, 527, -1, {-2.f, -3.f, -4.f, 1.f, -5.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 442 */ {CONSTRUCT_PORTAL, 450, -1, {-4.f, 3.f, 0.f, 4.f, -4.f, 5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 443 */ {CONSTRUCT_PORTAL, 476, -1, {2.f, 4.f, 5.f, 1.f, 7.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 444 */ {CONSTRUCT_PORTAL, 548, -1, {2.f, -3.f, 5.f, -1.f, 7.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 445 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 446 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 447 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 448 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 449 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 450 */ {CONSTRUCT_CELL, -1, -1, {1.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 451 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 452 */ {CONSTRUCT_WALL, -1, -1, {4.f, -1.f, 4.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 453 */ {CONSTRUCT_PORTAL, 434, -1, {0.f, -2.f, 4.f, -1.f, 4.f, -5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 454 */ {CONSTRUCT_PORTAL, 462, -1, {4.f, 1.f, -3.f, 3.f, 5.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 455 */ {CONSTRUCT_DOOR, 492, 497, {-5.f, 1.f, -3.f, 3.f, 1.f, -5.f, 3.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 456 */ {CONSTRUCT_PORTAL, 514, -1, {-5.f, 1.f, -2.f, -2.f, -5.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 457 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 458 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 459 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 460 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 461 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 462 */ {CONSTRUCT_CELL, -1, -1, {2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 463 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, 1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 464 */ {CONSTRUCT_WALL, -1, -1, {-8.f, 0.f, -8.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 465 */ {CONSTRUCT_WALL, -1, -1, {-8.f, 2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 466 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 6.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 467 */ {CONSTRUCT_WALL, -1, -1, {6.f, 2.f, 6.f, 0.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 468 */ {CONSTRUCT_PORTAL, 476, -1, {1.f, -2.f, 6.f, 0.f, 6.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 469 */ {CONSTRUCT_PORTAL, 706, -1, {-1.f, 2.f, 1.f, 2.f, 0.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 470 */ {CONSTRUCT_PORTAL, 450, -1, {-1.f, -2.f, -8.f, 0.f, -5.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 471 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 472 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 473 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 474 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 475 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 476 */ {CONSTRUCT_CELL, -1, -1, {3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 477 */ {CONSTRUCT_WALL, -1, -1, {-5.f, 0.f, -5.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 478 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 4.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 479 */ {CONSTRUCT_WALL, -1, -1, {4.f, 2.f, 4.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 480 */ {CONSTRUCT_WALL, -1, -1, {4.f, -2.f, 4.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 481 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -3.f, 0.f, -3.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 482 */ {CONSTRUCT_PORTAL, 462, -1, {-5.f, 2.f, 0.f, 4.f, -6.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 483 */ {CONSTRUCT_DOOR, 503, 508, {0.f, 4.f, 2.f, 2.f, 1.f, 2.f, 4.f, 0.f}, 0, TEX_ID_LEFT_DOOR_3, TEX_ID_RIGHT_DOOR_3},
	/* 484 */ {CONSTRUCT_PORTAL, 538, -1, {0.f, -3.f, 4.f, -3.f, 2.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 485 */ {CONSTRUCT_PORTAL, 434, -1, {-2.f, -3.f, -5.f, 0.f, -7.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 486 */ {CONSTRUCT_PORTAL, 696, -1, {4.f, 0.f, 4.f, -2.f, 5.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 487 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 488 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 489 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 490 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 491 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 492 */ {CONSTRUCT_CELL, -1, -1, {4.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 493 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 494 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 495 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 496 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 497 */ {CONSTRUCT_DOOR, 450, 455, {2.f, 0.f, 0.f, -2.f, 1.f, 5.f, -3.f, 0.f}, 0, TEX_ID_LEFT_DOOR_1, TEX_ID_RIGHT_DOOR_1},
	/* 498 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 499 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 500 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 501 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 502 */ {CONSTRUCT_BLOB, 16, -1, {0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 503 */ {CONSTRUCT_CELL, -1, -1, {5.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 504 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 505 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 506 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 507 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 508 */ {CONSTRUCT_DOOR, 476, 483, {-2.f, 0.f, 0.f, -2.f, 1.f, -2.f, -4.f, 0.f}, 0, TEX_ID_LEFT_DOOR_2, TEX_ID_RIGHT_DOOR_2},
	/* 509 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 510 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 511 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 512 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 513 */ {CONSTRUCT_BLOB, 16, -1, {0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 514 */ {CONSTRUCT_CELL, -1, -1, {6.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 515 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 5.f, 0.f, 5.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 516 */ {CONSTRUCT_WALL, -1, -1, {3.f, 0.f, 3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 517 */ {CONSTRUCT_WALL, -1, -1, {1.f, -4.f, 1.f, -6.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 518 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -6.f, -2.f, 5.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 519 */ {CONSTRUCT_PORTAL, 450, -1, {0.f, 5.f, 3.f, 2.f, 5.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 520 */ {CONSTRUCT_PORTAL, 527, -1, {3.f, 0.f, 1.f, -4.f, 4.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 521 */ {CONSTRUCT_PORTAL, 587, -1, {-2.f, -6.f, 1.f, -6.f, 0.f, -10.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 522 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 523 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 524 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 525 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 526 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 527 */ {CONSTRUCT_CELL, -1, -1, {7.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 528 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 529 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -2.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 530 */ {CONSTRUCT_PORTAL, 514, -1, {-1.f, 2.f, -3.f, -2.f, -4.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 531 */ {CONSTRUCT_PORTAL, 434, -1, {1.f, 2.f, 3.f, -2.f, 5.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 532 */ {CONSTRUCT_PORTAL, 716, -1, {-1.f, -2.f, 3.f, -2.f, 1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 533 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 534 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 535 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 536 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 537 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 538 */ {CONSTRUCT_CELL, -1, -1, {8.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 539 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 540 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 541 */ {CONSTRUCT_PORTAL, 476, -1, {-2.f, 1.f, 2.f, 1.f, -2.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 542 */ {CONSTRUCT_PORTAL, 548, -1, {-2.f, -1.f, 2.f, -1.f, -2.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 543 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 544 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 545 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 546 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 547 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 548 */ {CONSTRUCT_CELL, -1, -1, {9.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 549 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 3.f, 0.f, 3.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 550 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -3.f, 0.f, -3.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 551 */ {CONSTRUCT_WALL, -1, -1, {4.f, 3.f, 4.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 552 */ {CONSTRUCT_WALL, -1, -1, {-5.f, 0.f, -5.f, 0.999f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 553 */ {CONSTRUCT_PORTAL, 538, -1, {0.f, 3.f, 4.f, 3.f, 2.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 554 */ {CONSTRUCT_PORTAL, 434, -1, {-5.f, 1.f, -2.f, 3.f, -7.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 555 */ {CONSTRUCT_PORTAL, 562, -1, {-5.f, 0.f, -2.f, -3.f, -6.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 556 */ {CONSTRUCT_PORTAL, 611, -1, {0.f, -3.f, 4.f, -3.f, 2.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 557 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 558 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 559 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 560 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 561 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 562 */ {CONSTRUCT_CELL, -1, -1, {10.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 563 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 0.f, -1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 564 */ {CONSTRUCT_WALL, -1, -1, {-2.99f, 2.f, 0.99f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 565 */ {CONSTRUCT_PORTAL, 573, -1, {-3.f, 2.f, -2.f, -1.f, -5.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 566 */ {CONSTRUCT_PORTAL, 601, -1, {0.f, -1.f, 4.f, -1.f, 2.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 567 */ {CONSTRUCT_PORTAL, 548, -1, {4.f, -1.f, 1.f, 2.f, 6.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 568 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 569 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 570 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 571 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 572 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 573 */ {CONSTRUCT_CELL, -1, -1, {11.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 574 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 575 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 576 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -3.f, 0.f, -3.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 577 */ {CONSTRUCT_WALL, -1, -1, {3.f, -3.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 578 */ {CONSTRUCT_PORTAL, 716, -1, {-2.f, 2.f, 2.f, 2.f, 0.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 579 */ {CONSTRUCT_PORTAL, 562, -1, {2.f, 2.f, 3.f, -1.f, 5.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 580 */ {CONSTRUCT_PORTAL, 621, -1, {3.f, -3.f, 0.f, -3.f, 8.f, -5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 581 */ {CONSTRUCT_PORTAL, 587, -1, {-2.f, -3.f, -2.f, 1.f, -5.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 582 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 583 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 584 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 585 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 586 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 587 */ {CONSTRUCT_CELL, -1, -1, {12.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 588 */ {CONSTRUCT_WALL, -1, -1, {1.f, 4.f, 3.f, 4.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 589 */ {CONSTRUCT_WALL, -1, -1, {3.f, 0.f, 3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 590 */ {CONSTRUCT_WALL, -1, -1, {0.f, -4.f, -2.f, -4.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 591 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -4.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 592 */ {CONSTRUCT_PORTAL, 573, -1, {3.f, 0.f, 3.f, 4.f, 5.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 593 */ {CONSTRUCT_PORTAL, 514, -1, {1.f, 4.f, -2.f, 4.f, 0.f, 10.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 594 */ {CONSTRUCT_PORTAL, 706, -1, {-2.f, 2.f, -2.f, 4.f, -3.f, 3.f, -PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 595 */ {CONSTRUCT_PORTAL, 638, -1, {0.f, -4.f, 3.f, -2.f, 4.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 596 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 597 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 598 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 599 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 600 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 601 */ {CONSTRUCT_CELL, -1, -1, {13.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 602 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 603 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 604 */ {CONSTRUCT_PORTAL, 562, -1, {-2.f, 1.f, 2.f, 1.f, -2.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 605 */ {CONSTRUCT_PORTAL, 621, -1, {-2.f, -1.f, 2.f, -1.f, 1.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 606 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 607 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 608 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 609 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 610 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 611 */ {CONSTRUCT_CELL, -1, -1, {14.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 612 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 613 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 614 */ {CONSTRUCT_PORTAL, 548, -1, {-2.f, 1.f, 2.f, 1.f, -2.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 615 */ {CONSTRUCT_PORTAL, 621, -1, {-2.f, -1.f, 2.f, -1.f, -5.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 616 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 617 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 618 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 619 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 620 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 621 */ {CONSTRUCT_CELL, -1, -1, {15.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 622 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 623 */ {CONSTRUCT_WALL, -1, -1, {7.f, 2.f, 7.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 624 */ {CONSTRUCT_WALL, -1, -1, {7.f, -2.f, 5.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 625 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -2.f, -5.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 626 */ {CONSTRUCT_WALL, -1, -1, {-8.f, 0.f, -8.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 627 */ {CONSTRUCT_WALL, -1, -1, {-5.f, 2.f, -3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 628 */ {CONSTRUCT_PORTAL, 601, -1, {-3.f, 2.f, 1.f, 2.f, -1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 629 */ {CONSTRUCT_PORTAL, 611, -1, {3.f, 2.f, 7.f, 2.f, 5.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 630 */ {CONSTRUCT_PORTAL, 673, -1, {5.f, -2.f, -3.f, -2.f, 0.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 631 */ {CONSTRUCT_PORTAL, 638, -1, {-5.f, -2.f, -8.f, 0.f, -9.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 632 */ {CONSTRUCT_PORTAL, 573, -1, {-8.f, 2.f, -5.f, 2.f, -8.f, 5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 633 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 634 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 635 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 636 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 637 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 638 */ {CONSTRUCT_CELL, -1, -1, {16.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 639 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 640 */ {CONSTRUCT_WALL, -1, -1, {4.f, 0.f, 4.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 641 */ {CONSTRUCT_PORTAL, 621, -1, {1.f, 2.f, 4.f, 0.f, 9.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 642 */ {CONSTRUCT_PORTAL, 587, -1, {-4.f, 0.f, -1.f, 2.f, -4.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 643 */ {CONSTRUCT_PORTAL, 661, -1, {-2.f, -2.f, 4.f, -2.f, 2.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 644 */ {CONSTRUCT_DOOR, 650, 655, {-4.f, 0.f, -2.f, -2.f, 1.f, -4.f, -2.f, 0.f}, 0, TEX_ID_LEFT_DOOR_4, TEX_ID_RIGHT_DOOR_4},
	/* 645 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 646 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 647 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 648 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 649 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 650 */ {CONSTRUCT_CELL, -1, -1, {17.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 651 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 652 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, -2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 653 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 654 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, 0.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 655 */ {CONSTRUCT_DOOR, 638, 644, {0.f, 2.f, 2.f, 0.f, 1.f, 4.f, 2.f, 0.f}, 0, TEX_ID_LEFT_DOOR_3, TEX_ID_RIGHT_DOOR_3},
	/* 656 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 657 */ {CONSTRUCT_KEY, 679, -1, {0.f, 0.f, 1.f, 1.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 658 */ {CONSTRUCT_SNOWMAN, 16, -1, {-1.f, 1.f, -PI / 2.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 659 */ {CONSTRUCT_SNOWMAN, 16, -1, {1.f, -1.f, PI}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 660 */ {CONSTRUCT_SNOWMAN, 16, -1, {0.f, 0.f, 5.f * PI / 4.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},

	/* 661 */ {CONSTRUCT_CELL, -1, -1, {18.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 662 */ {CONSTRUCT_WALL, -1, -1, {-4.f, -1.f, -4.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 663 */ {CONSTRUCT_WALL, -1, -1, {-4.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 664 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 4.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 665 */ {CONSTRUCT_PORTAL, 638, -1, {-4.f, 1.f, 2.f, 1.f, -2.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 666 */ {CONSTRUCT_PORTAL, 673, -1, {4.f, 1.f, 4.f, -1.f, 7.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 667 */ {CONSTRUCT_PORTAL, 696, -1, {2.f, -1.f, 4.f, -1.f, 3.f, -2.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 668 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 669 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 670 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 671 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 672 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 673 */ {CONSTRUCT_CELL, -1, -1, {19.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 674 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 2.f, -3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 675 */ {CONSTRUCT_WALL, -1, -1, {3.f, 0.f, 3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 676 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -2.f, 3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 677 */ {CONSTRUCT_PORTAL, 661, -1, {-3.f, -2.f, -3.f, 0.f, -7.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 678 */ {CONSTRUCT_PORTAL, 621, -1, {-3.f, 2.f, 5.f, 2.f, 0.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 679 */ {CONSTRUCT_DOOR, 685, 690, {3.f, 0.f, 5.f, 2.f, 1.f, 5.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 680 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 681 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 682 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 683 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 684 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 685 */ {CONSTRUCT_CELL, -1, -1, {20.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 686 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 687 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 688 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 689 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 0.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 690 */ {CONSTRUCT_DOOR, 673, 679, {-2.f, 0.f, 0.f, 2.f, 1.f, -5.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_1, TEX_ID_RIGHT_DOOR_1},
	/* 691 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 692 */ {CONSTRUCT_SNOWMAN, 16, -1, {1.f, 1.f,  PI}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 693 */ {CONSTRUCT_SNOWMAN, 16, -1, {-1.f, -1.f, PI / 2.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 694 */ {CONSTRUCT_SNOWMAN, 16, -1, {0.f, 0.f, 3.f * PI / 4.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 695 */ {CONSTRUCT_KEY, 11, -1, {0.f, 0.f, 1.f, 1.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 696 */ {CONSTRUCT_CELL, -1, -1, {21.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 697 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 698 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 699 */ {CONSTRUCT_PORTAL, 476, -1, {-1.f, -1.f, -1.f, 1.f, -5.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 700 */ {CONSTRUCT_PORTAL, 661, -1, {1.f, 1.f, 1.f, -1.f, 2.f, 3.f, -PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 701 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 702 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 703 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 704 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 705 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 706 */ {CONSTRUCT_CELL, -1, -1, {22.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 707 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, -1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 708 */ {CONSTRUCT_WALL, -1, -1, {1.f, -1.f, 1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 709 */ {CONSTRUCT_PORTAL, 462, -1, {-1.f, -1.f, 1.f, -1.f, 0.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 710 */ {CONSTRUCT_PORTAL, 587, -1, {-1.f, 1.f, 1.f, 1.f, 3.f, 3.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 711 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 712 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 713 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 714 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 715 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 716 */ {CONSTRUCT_CELL, -1, -1, {23.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 717 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 718 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, 0.999f, 0.f, 0.f}, 0, TEX_ID_MARBLE_0, TEX_ID_NONE},
	/* 719 */ {CONSTRUCT_PORTAL, 527, -1, {-2.f, 1.f, 2.f, 1.f, -1.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 720 */ {CONSTRUCT_PORTAL, 573, -1, {-2.f, 0.f, 2.f, 0.f, 0.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 721 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 722 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 723 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 724 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 725 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
};

map level2_map =
{
	level2_map_constructs,
	sizeof(level2_map_constructs) / sizeof(construct),
	DYNAMIC_CELL_MODE,
};

// endof level2.cpp