// level4.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "map.h"
#include "switch.h"
#include "door.h"
#include "orb.h"
#include "macros.h"

// I may have indexed this matrix incorrectly.
static int r, g, b;
static int door_matrix[2][2][2] =
{
	// 0, g, b
	{
		// 0, 0, b
		{
			// 0, 0, 0		(black)
			{
				-1,
			},
			// 0, 0, 1		(blue)
			{
				61,
			},
		},
		// 0, 1, b
		{
			// 0, 1, 0		(green)
			{
				69,
			},
			// 0, 1, 1		(cyan)
			{
				129
			},
		},
	},
	// 1, g, b
	{
		// 1, 0, b
		{
			// 1, 0, 0		(red)
			{
				83,
			},
			// 1, 0, 1		(magneta)
			{
				233,
			},
		},
		// 1, 1, b
		{
			// 1, 1, 0		(yellow)
			{
				159
			},
			// 1, 1, 1		(white)
			{
				-1
			},
		},
	},
};

// If you're peaking at this routine to see
// how to get past level 4, then shame on you.
static void level4_update_doors(void)
{
	extern construct level4_map_constructs[];
	Door *d;
	int k = door_matrix[r][g][b];
	if(k >= 0)
	{
		d = (Door *)level4_map_constructs[k].mem;
		if(d->is_locked)
			d->UnlockDoor();
	}
	else
	{
		int i, j;
		for(i = 0; i < 2; i++)
		{
			for(j = 0; j < 2; j++)
			{
				for(k = 0; k < 2; k++)
				{
					if(door_matrix[i][j][k] < 0)
						continue;

					d = (Door *)level4_map_constructs[door_matrix[i][j][k]].mem;
					if(!d->is_locked)
						d->LockDoor();
				}
			}
		}
	}
}

static void level4_red_switch(SwitchState state, void *data[2])
{
	r = (state == SWITCH_STATE_ON);
	level4_update_doors();
}

static void level4_green_switch(SwitchState state, void *data[2])
{
	g = (state == SWITCH_STATE_ON);
	level4_update_doors();
}

static void level4_blue_switch(SwitchState state, void *data[2])
{
	b = (state == SWITCH_STATE_ON);
	level4_update_doors();
}

static void level4_general_switch(SwitchState state, void *data[2])
{
	extern construct level4_map_constructs[];
	static const int num_switches = 8;	// There are 8 generic switches.
	static int _switch_index[num_switches] = {143, 179, 215, 225, 244, 254, 264, 267};
	Switch *_switch;
	int k;
	for(k = 0; k < num_switches; k++)
	{
		_switch = (Switch *)level4_map_constructs[_switch_index[k]].mem;
		if(_switch->state == SWITCH_STATE_OFF)
			return;
	}
	Door *d = (Door *)level4_map_constructs[91].mem;
	if(d->is_locked)
		d->UnlockDoor();
}

construct level4_map_constructs[] =
{
	/* 000 */ {CONSTRUCT_CELL, -1, -1, {0.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 001 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -6.f, 1.f, -6.f, 0.f, 0.f},  0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 002 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -6.f, -2.f, -5.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 003 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -5.f, -2.f, -4.f, 0.f, 1.f}, 0, TEX_ID_RED_STONE_WALL_0, TEX_ID_NONE},
	/* 004 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-2.f, -4.f, -2.f, -2.f, 0.f}, level4_red_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 005 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, -2.f, -1.f, 0.f, 1.f}, 0, TEX_ID_GREEN_STONE_WALL_0, TEX_ID_NONE},
	/* 006 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-2.f, -1.f, -2.f, 1.f, 0.f}, level4_green_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 007 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, -2.f, 2.f, 0.f, 1.f}, 0, TEX_ID_BLUE_STONE_WALL_0, TEX_ID_NONE},
	/* 008 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-2.f, 2.f, -2.f, 4.f, 0.f}, level4_blue_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 009 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 4.f, -2.f, 5.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 010 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 5.f, -1.f, 6.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 011 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 6.f, 1.f, 6.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 012 */ {CONSTRUCT_WALL, -1, -1, {1.f, -2.999f, 1.f, 2.999f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 013 */ {CONSTRUCT_PORTAL, 44, -1, {1.f, 3.f, 1.f, 6.f, 4.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 014 */ {CONSTRUCT_PORTAL, 18, -1, {1.f, -3.f, 1.f, -6.f, 3.f, -5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 015 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 016 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 017 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 018 */ {CONSTRUCT_CELL, -1, -1, {1.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 019 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 020 */ {CONSTRUCT_WALL, -1, -1, {1.f, -1.f, 2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 021 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 022 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 023 */ {CONSTRUCT_PORTAL, 0, -1, {-2.f, -1.f, -2.f, 2.f, -3.f, 5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 024 */ {CONSTRUCT_PORTAL, 28, -1, {-1.f, 2.f, 2.f, 2.f, 0.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 025 */ {CONSTRUCT_CAMERA, 26, -1, {0.f, 0.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 026 */ {CONSTRUCT_SPENCER, -1, -1, {0.f, 0.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 027 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 028 */ {CONSTRUCT_CELL, -1, -1, {2.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 029 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, -1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 030 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 031 */ {CONSTRUCT_PORTAL, 18, -1, {-1.f, 0.f, 2.f, 0.f, 0.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 032 */ {CONSTRUCT_PORTAL, 36, -1, {-1.f, 1.f, 2.f, 1.f, 0.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 033 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 034 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 035 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 036 */ {CONSTRUCT_CELL, -1, -1, {3.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 037 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, -1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 038 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 039 */ {CONSTRUCT_PORTAL, 28, -1, {-1.f, 0.f, 2.f, 0.f, 0.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 040 */ {CONSTRUCT_PORTAL, 44, -1, {-1.f, 1.f, 2.f, 1.f, 1.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 041 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 042 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 043 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 044 */ {CONSTRUCT_CELL, -1, -1, {4.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 045 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -1.f, -2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 046 */ {CONSTRUCT_WALL, -1, -1, {1.f, -1.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 047 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 2.f, 3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 048 */ {CONSTRUCT_PORTAL, 0, -1, {-3.f, -1.f, -3.f, 2.f, -4.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 049 */ {CONSTRUCT_PORTAL, 53, -1, {3.f, -1.f, 3.f, 2.f, 4.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 050 */ {CONSTRUCT_PORTAL, 36, -1, {-2.f, -1.f, 1.f, -1.f, -1.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 051 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 052 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 053 */ {CONSTRUCT_CELL, -1, -1, {5.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 054 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, -1.f, -1.f, 0.f, 1.f}, 0, TEX_ID_RED_STONE_WALL_0, TEX_ID_NONE},
	/* 055 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 2.f, -1.f, 0.f, 1.f}, 0, TEX_ID_RED_STONE_WALL_0, TEX_ID_NONE},
	/* 056 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, -1.f, 3.f, 0.f, 1.f}, 0, TEX_ID_BLUE_STONE_WALL_0, TEX_ID_NONE},
	/* 057 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 2.f, 3.f, 0.f, 1.f}, 0, TEX_ID_BLUE_STONE_WALL_0, TEX_ID_NONE},
	/* 058 */ {CONSTRUCT_DOOR, 73, 83, {-1.f, -2.f, 2.f, -2.f, 1.f, 0.f, -5.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 059 */ {CONSTRUCT_PORTAL, 44, -1, {-1.f, -1.f, -1.f, 2.f, -4.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 060 */ {CONSTRUCT_PORTAL, 65, -1, {2.f, -1.f, 2.f, 2.f, 3.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 061 */ {CONSTRUCT_DOOR, 121, 130, {-1.f, 3.f, 2.f, 3.f, 1.f, 0.f, 6.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 062 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 063 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 064 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 065 */ {CONSTRUCT_CELL, -1, -1, {6.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 066 */ {CONSTRUCT_WALL, -1, -1, {0.f, -1.f, 1.f, -1.f, 0.f, 1.f}, 0, TEX_ID_GREEN_STONE_WALL_0, TEX_ID_NONE},
	/* 067 */ {CONSTRUCT_WALL, -1, -1, {0.f, 2.f, 1.f, 2.f, 0.f, 1.f}, 0, TEX_ID_GREEN_STONE_WALL_0, TEX_ID_NONE},
	/* 068 */ {CONSTRUCT_PORTAL, 53, -1, {-1.f, -1.f, -1.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 069 */ {CONSTRUCT_DOOR, 87, 90, {1.f, -1.f, 1.f, 2.f, 1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 070 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 0.f, -1.f, 0.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_STONE_WALL_0},
	/* 071 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, 0.f, 2.f, 0.f, 1.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_STONE_WALL_0},
	/* 072 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 073 */ {CONSTRUCT_CELL, -1, -1, {7.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 074 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 3.f, -4.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 075 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 1.f, -2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 076 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, -2.f, -4.f, 0.f, 0.f}, 0, TEX_ID_DREW_CAREY_0, TEX_ID_NONE},
	/* 077 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -4.f, 0.f, -5.f, 0.f, 1.f}, 0, TEX_ID_GREG_PROOPS_2, TEX_ID_NONE},
	/* 078 */ {CONSTRUCT_WALL, -1, -1, {0.f, -5.f, 1.f, -5.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 079 */ {CONSTRUCT_WALL, -1, -1, {1.f, -5.f, 3.f, -4.f, 0.f, 1.f}, 0, TEX_ID_GREG_PROOPS_2, TEX_ID_NONE},
	/* 080 */ {CONSTRUCT_WALL, -1, -1, {3.f, -4.f, 3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_CLIVE_ANDERSON_0, TEX_ID_NONE},
	/* 081 */ {CONSTRUCT_WALL, -1, -1, {3.f, -2.f, 5.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 082 */ {CONSTRUCT_WALL, -1, -1, {5.f, 1.f, 2.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 083 */ {CONSTRUCT_DOOR, 53, 58, {-1.f, 3.f, 2.f, 3.f, 1.f, 0.f, 5.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 084 */ {CONSTRUCT_KEY, 270, -1, {0.5f, -2.5f, 0.5f, 0.5f, 0.5f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 085 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 086 */ {CONSTRUCT_BLOB, 26, -1, {0.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 087 */ {CONSTRUCT_CELL, -1, -1, {8.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 088 */ {CONSTRUCT_WALL, -1, -1, {1.f, -1.f, 3.999f, -1.f, 1.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 089 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 3.999f, 2.f, 1.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 090 */ {CONSTRUCT_DOOR, 65, 69, {-3.f, -1.f, -3.f, 2.f, 1.f, -4.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 091 */ {CONSTRUCT_DOOR, 95, 101, {4.f, -1.f, 4.f, 2.f, 1.f, 5.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_2, TEX_ID_RIGHT_DOOR_2},
	/* 092 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -1.f, 1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 093 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 2.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 094 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 095 */ {CONSTRUCT_CELL, -1, -1, {9.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 096 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, -1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 097 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, -1.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 098 */ {CONSTRUCT_WALL, -1, -1, {2.f, -5.f, 2.f, 6.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 099 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -5.f, 2.f, -5.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 100 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 6.f, 2.f, 6.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 101 */ {CONSTRUCT_DOOR, 87, 91, {-1.f, -1.f, -1.f, 2.f, 1.f, -5.f, 0.f, 0.f}, 0, TEX_ID_LEFT_DOOR_4, TEX_ID_RIGHT_DOOR_4},
	/* 102 */ {CONSTRUCT_PORTAL, 107, -1, {-1.f, -5.f, -1.f, -2.f, -4.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 103 */ {CONSTRUCT_PORTAL, 107, -1, {-1.f, 3.f, -1.f, 6.f, -4.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 104 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 105 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 106 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	
	/* 107 */ {CONSTRUCT_CELL, -1, -1, {10.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 108 */ {CONSTRUCT_WALL, -1, -1, {3.f, -2.f, -3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 109 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -2.f, -3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 110 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 0.f, -3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 111 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 2.f, -3.f, 5.f, 0.f, 1.f}, 0, TEX_ID_GREG_PROOPS_0, TEX_ID_NONE},
	/* 112 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 5.f, -3.f, 7.f, 0.f, 1.f}, 0, TEX_ID_GREG_PROOPS_1, TEX_ID_NONE},
	/* 113 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 7.f, -3.f, 9.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 114 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 9.f, 3.f, 9.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 115 */ {CONSTRUCT_WALL, -1, -1, {3.f, 1.001f, 3.f, 5.999f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 116 */ {CONSTRUCT_PORTAL, 95, -1, {3.f, -2.f, 3.f, 1.f, 4.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 117 */ {CONSTRUCT_PORTAL, 95, -1, {3.f, 6.f, 3.f, 9.f, 4.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 118 */ {CONSTRUCT_SNOWMAN, 26, -1, {0.f, 0.f, 0.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 119 */ {CONSTRUCT_ORB, -1, -1, {-1.f, 3.5f, (float)ORB_OF_PROOPS}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 120 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 121 */ {CONSTRUCT_CELL, -1, -1, {11.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 122 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -3.f, -1.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 123 */ {CONSTRUCT_WALL, -1, -1, {2.f, -3.f, 4.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 124 */ {CONSTRUCT_WALL, -1, -1, {4.f, -3.f, 4.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 125 */ {CONSTRUCT_WALL, -1, -1, {4.f, 1.f, 4.f, 2.f, 0.f, 1.f}, 0, TEX_ID_CYAN_STONE_WALL_0, TEX_ID_NONE},
	/* 126 */ {CONSTRUCT_WALL, -1, -1, {1.f, 4.f, 2.f, 4.f, 0.f, 1.f}, 0, TEX_ID_CYAN_STONE_WALL_0, TEX_ID_NONE},
	/* 127 */ {CONSTRUCT_PORTAL, 133, -1, {-3.f, -3.f, -3.f, 1.f, -5.f, -1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 128 */ {CONSTRUCT_PORTAL, 185, -1, {-3.f, 3.f, 1.f, 4.f, -1.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 129 */ {CONSTRUCT_DOOR, 202, 208, {2.f, 4.f, 4.f, 2.f, 1.f, 5.f, 5.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 130 */ {CONSTRUCT_DOOR, 53, 61, {-1.f, -3.f, 2.f, -3.f, 1.f, 0.f, -6.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 131 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.f, -3.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 132 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 133 */ {CONSTRUCT_CELL, -1, -1, {12.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 134 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 135 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 136 */ {CONSTRUCT_PORTAL, 141, -1, {-1.f, -2.f, -1.f, 2.f, -3.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 137 */ {CONSTRUCT_PORTAL, 121, -1, {2.f, -2.f, 2.f, 2.f, 5.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 138 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 139 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 140 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 141 */ {CONSTRUCT_CELL, -1, -1, {13.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 142 */ {CONSTRUCT_WALL, -1, -1, {2.f, -6.f, -2.f, -6.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 143 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-2.f, -6.f, -2.f, -4.f, 0.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 144 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -4.f, -2.f, 8.f, 0.f, 1.f}, 0, TEX_ID_GALAXY_0, TEX_ID_NONE},
	/* 145 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 8.f, 1.f, 8.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 146 */ {CONSTRUCT_WALL, -1, -1, {1.f, 5.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 147 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 148 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 149 */ {CONSTRUCT_PORTAL, 133, -1, {2.f, -6.f, 2.f, -2.f, 3.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 150 */ {CONSTRUCT_PORTAL, 154, -1, {1.f, 5.f, 1.f, 8.f, 3.f, 7.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 151 */ {CONSTRUCT_SNOWMAN, 26, -1, {0.f, 0.f, 3.f * PI / 2.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 152 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 153 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 154 */ {CONSTRUCT_CELL, -1, -1, {14.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 155 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, -1.f, -2.f, 0.f, 1.f}, 0, TEX_ID_YELLOW_STONE_WALL_0, TEX_ID_NONE},
	/* 156 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 3.f, -2.f, 0.f, 1.f}, 0, TEX_ID_YELLOW_STONE_WALL_0, TEX_ID_NONE},
	/* 157 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 3.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 158 */ {CONSTRUCT_PORTAL, 141, -1, {-2.f, -2.f, -2.f, 1.f, -3.f, -7.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 159 */ {CONSTRUCT_DOOR, 164, 172, {-1.f, -2.f, 2.f, -2.f, 1.f, 0.f, -5.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 160 */ {CONSTRUCT_PORTAL, 193, -1, {3.f, -2.f, 3.f, 1.f, 4.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 161 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 162 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 163 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 164 */ {CONSTRUCT_CELL, -1, -1, {15.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 165 */ {CONSTRUCT_WALL, -1, -1, {2.f, 3.f, 3.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 166 */ {CONSTRUCT_WALL, -1, -1, {3.f, 3.f, 3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 167 */ {CONSTRUCT_WALL, -1, -1, {3.f, 0.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 168 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, -2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 169 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 170 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 3.f, -1.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 171 */ {CONSTRUCT_PORTAL, 176, -1, {-1.f, -2.f, 2.f, -2.f, 0.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 172 */ {CONSTRUCT_DOOR, 154, 159, {-1.f, 3.f, 2.f, 3.f, 1.f, 0.f, 5.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 173 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 174 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 175 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 176 */ {CONSTRUCT_CELL, -1, -1, {16.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 177 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, -1.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 178 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 0.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 179 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {0.f, -1.f, 2.f, -1.f, 0.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 180 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 181 */ {CONSTRUCT_PORTAL, 164, -1, {-1.f, 1.f, 2.f, 1.f, 0.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 182 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 183 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 184 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 185 */ {CONSTRUCT_CELL, -1, -1, {17.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 186 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -1.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 187 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 188 */ {CONSTRUCT_PORTAL, 121, -1, {-2.f, -1.f, 2.f, 0.f, 1.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 189 */ {CONSTRUCT_PORTAL, 193, -1, {-1.f, 1.f, 2.f, 1.f, 0.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 190 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 191 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 192 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 193 */ {CONSTRUCT_CELL, -1, -1, {18.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 194 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -3.f, -1.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 195 */ {CONSTRUCT_WALL, -1, -1, {2.f, -3.f, 2.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 196 */ {CONSTRUCT_WALL, -1, -1, {2.f, 3.f, -1.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 197 */ {CONSTRUCT_PORTAL, 154, -1, {-1.f, 0.f, -1.f, 3.f, -4.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 198 */ {CONSTRUCT_PORTAL, 185, -1, {-1.f, -3.f, 2.f, -3.f, 0.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 199 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 200 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 201 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 202 */ {CONSTRUCT_CELL, -1, -1, {19.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 203 */ {CONSTRUCT_WALL, -1, -1, {0.f, 2.f, 1.f, 2.f, 0.f, 1.f}, 0, TEX_ID_MAGENTA_STONE_WALL_0, TEX_ID_NONE},
	/* 204 */ {CONSTRUCT_WALL, -1, -1, {3.f, -1.f, 3.f, 0.f, 0.f, 1.f}, 0, TEX_ID_MAGENTA_STONE_WALL_0, TEX_ID_NONE},
	/* 205 */ {CONSTRUCT_PORTAL, 212, -1, {-3.f, -1.f, 0.f, 2.f, -2.f, 4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 206 */ {CONSTRUCT_DOOR, 232, 233, {1.f, 2.f, 3.f, 0.f, 1.f, 3.f, 2.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 207 */ {CONSTRUCT_PORTAL, 222, -1, {3.f, -1.f, -1.f, -3.f, 4.f, -3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 208 */ {CONSTRUCT_DOOR, 121, 129, {-3.f, -1.f, -1.f, -3.f, 1.f, -5.f, -5.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 209 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 210 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 211 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 212 */ {CONSTRUCT_CELL, -1, -1, {20.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 213 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -5.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 214 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 215 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-1.f, 2.f, 1.f, 2.f, 0.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 216 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 217 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 218 */ {CONSTRUCT_PORTAL, 202, -1, {-1.f, -5.f, 2.f, -2.f, 2.f, -4.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 219 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -4.999f, -1.f, -4.999f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 220 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 221 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 222 */ {CONSTRUCT_CELL, -1, -1, {21.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 223 */ {CONSTRUCT_WALL, -1, -1, {-4.999f, -1.f, -4.999f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 224 */ {CONSTRUCT_WALL, -1, -1, {-5.f, -1.f, 5.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 225 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {5.f, -1.f, 5.f, 1.f, 0.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 226 */ {CONSTRUCT_WALL, -1, -1, {5.f, 1.f, 5.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 227 */ {CONSTRUCT_WALL, -1, -1, {5.f, 2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 228 */ {CONSTRUCT_PORTAL, 202, -1, {-5.f, 0.f, -1.f, 2.f, -4.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 229 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 230 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 231 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 232 */ {CONSTRUCT_CELL, -1, -1, {22.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 233 */ {CONSTRUCT_DOOR, 202, 206, {-2.f, 0.f, 0.f, -2.f, 1.f, -3.f, -2.f, 0.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 234 */ {CONSTRUCT_PORTAL, 251, -1, {0.f, -2.f, 3.f, 0.f, 4.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 235 */ {CONSTRUCT_DOOR, 261, 270, {3.f, 0.f, 1.f, 2.f, 1.f, 4.f, 2.f, 0.f}, 0, TEX_ID_LEFT_DOOR_4, TEX_ID_RIGHT_DOOR_4},
	/* 236 */ {CONSTRUCT_PORTAL, 240, -1, {1.f, 2.f, -2.f, 0.f, -1.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 237 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 238 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 239 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 240 */ {CONSTRUCT_CELL, -1, -1, {23.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 241 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, -2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 242 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 243 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 244 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-1.f, 2.f, 1.f, 2.f, 0.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 245 */ {CONSTRUCT_WALL, -1, -1, {1.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 246 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 247 */ {CONSTRUCT_PORTAL, 232, -1, {-1.f, -2.f, 2.f, 0.f, 1.f, -2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 248 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 249 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 250 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 251 */ {CONSTRUCT_CELL, -1, -1, {24.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 252 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 0.f, -4.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 253 */ {CONSTRUCT_WALL, -1, -1, {-4.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 254 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {2.f, -1.f, 2.f, 1.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 255 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 256 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 257 */ {CONSTRUCT_PORTAL, 232, -1, {-4.f, 0.f, -1.f, 2.f, -4.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 258 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 259 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 260 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 261 */ {CONSTRUCT_CELL, -1, -1, {25.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 262 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 0.f, -3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 263 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 2.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 264 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {-2.f, 2.f, 0.f, 2.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 265 */ {CONSTRUCT_WALL, -1, -1, {0.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 266 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 267 */ {CONSTRUCT_WALL_SWITCH, -1, -1, {2.f, 1.f, 2.f, -1.f}, level4_general_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 268 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 269 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 270 */ {CONSTRUCT_DOOR, 232, 235, {-3.f, 0.f, -1.f, -2.f, 1.f, -4.f, -2.f, 0.f}, 0, TEX_ID_LEFT_DOOR_3, TEX_ID_RIGHT_DOOR_3},
	/* 271 */ {CONSTRUCT_SNOWMAN, 26, -1, {0.f, 0.f, 5.f * PI / 4.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
	/* 272 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 273 */ {CONSTRUCT_FUDGE, -1, -1, {0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
};

map level4_map =
{
	level4_map_constructs,
	sizeof(level4_map_constructs) / sizeof(construct),
	DYNAMIC_CELL_MODE,
};

// endof level4.cpp