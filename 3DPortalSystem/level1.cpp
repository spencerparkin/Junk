// level1.cpp

// Programmed by Spencer T. Parkin

#include <glut.h>
#include "map.h"
#include "switch.h"
#include "door.h"
#include "orb.h"
#include "macros.h"

// Note that the initial state of the switch
// is assumed to be in the off position, and
// the initial state of the door should be locked.
void level1_switch(SwitchState state, void *data[2])
{
	Door *door = (Door *)data[0];
	if(door->is_locked)
		door->UnlockDoor();
	else
		door->LockDoor();
}

construct level1_map_constructs[] =
{
	/* 000 */ {CONSTRUCT_CELL, -1, -1, {4.f, 3.f}, 0, TEX_ID_MOSAIC_0, TEX_ID_MARBLE_0},
	/* 001 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -3.f, 2.f, -3.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 002 */ {CONSTRUCT_WALL, -1, -1, {2.f, -3.f, 4.f, -1.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 003 */ {CONSTRUCT_WALL, -1, -1, {4.f, 2.f, 2.f, 4.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 004 */ {CONSTRUCT_WALL, -1, -1, {2.f, 4.f, -2.f, 4.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 005 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 4.f, -4.f, 2.f, 0.f, 1.f}, 0, TEX_ID_COLIN_MOCHRIE_0, TEX_ID_NONE},
	/* 006 */ {CONSTRUCT_WALL, -1, -1, {-4.f, 2.f, -4.f, -1.f, 0.f, 1.f}, 0, TEX_ID_COLIN_MOCHRIE_1, TEX_ID_NONE},
	/* 007 */ {CONSTRUCT_WALL, -1, -1, {-4.f, -1.f, -2.f, -3.f, 0.f, 1.f}, 0, TEX_ID_COLIN_MOCHRIE_2, TEX_ID_NONE},
	/* 008 */ {CONSTRUCT_DOOR, 10, 14, {4.f, -1.f, 4.f, 2.f, 1.f}, 0, TEX_ID_LEFT_DOOR_0, TEX_ID_RIGHT_DOOR_0},
	/* 009 */ {CONSTRUCT_ORB, -1, -1, {-2.f, 0.f, (float)ORB_OF_MOCHRIE}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 010 */ {CONSTRUCT_CELL, -1, -1, {10.f, 4.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 011 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 012 */ {CONSTRUCT_WALL, -1, -1, {0.f, -2.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 013 */ {CONSTRUCT_WALL, -1, -1, {1.f, 1.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 014 */ {CONSTRUCT_DOOR, 0, 8, {-2.f, -2.f, -2.f, 1.f, 1.f}, 0, TEX_ID_LEFT_DOOR_1, TEX_ID_RIGHT_DOOR_1},
	/* 015 */ {CONSTRUCT_PORTAL, 16, -1, {1.f, 1.f, 3.f, -1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 016 */ {CONSTRUCT_CELL, -1, -1, {12.f, 5.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 017 */ {CONSTRUCT_WALL, -1, -1, {1.f, -2.f, 2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 018 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 0.f, 0.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 019 */ {CONSTRUCT_PORTAL, 10, -1, {-1.f, 0.f, 1.f, -2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 020 */ {CONSTRUCT_PORTAL, 44, -1, {0.f, 2.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 021 */ {CONSTRUCT_CELL, -1, -1, {16.f, 5.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 022 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 023 */ {CONSTRUCT_WALL, -1, -1, {1.f, 0.f, 0.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 024 */ {CONSTRUCT_PORTAL, 44, -1, {-2.f, 0.f, 0.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 025 */ {CONSTRUCT_PORTAL, 26, -1, {-1.f, -2.f, 1.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 026 */ {CONSTRUCT_CELL, -1, -1, {18.f, 4.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 027 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 028 */ {CONSTRUCT_WALL, -1, -1, {-3.f, -1.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 029 */ {CONSTRUCT_PORTAL, 21, -1, {-1.f, 1.f, -3.f, -1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 030 */ {CONSTRUCT_PORTAL, 31, -1, {0.f, -2.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 031 */ {CONSTRUCT_CELL, -1, -1, {21.f, 2.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 032 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 0.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 033 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, 1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 034 */ {CONSTRUCT_WALL, -1, -1, {1.f, -2.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 035 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 036 */ {CONSTRUCT_PORTAL, 26, -1, {-3.f, 0.f, -1.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 037 */ {CONSTRUCT_DOOR, 38, 42, {-1.f, 3.f, 2.f, 3.f, 1.f}, 0, TEX_ID_LEFT_DOOR_3, TEX_ID_RIGHT_DOOR_3},

	/* 038 */ {CONSTRUCT_CELL, -1, -1, {21.f, 7.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 039 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 040 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 041 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 042 */ {CONSTRUCT_DOOR, 31, 37, {-1.f, -2.f, 2.f, -2.f, 1.f}, 0, TEX_ID_LEFT_DOOR_2, TEX_ID_RIGHT_DOOR_2},
	/* 043 */ {CONSTRUCT_KEY, 8, -1, {0.f, 0.f, 1.f, 1.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 044 */ {CONSTRUCT_CELL, -1, -1, {14.f, 8.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 045 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 046 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 047 */ {CONSTRUCT_PORTAL, 51, -1, {-2.f, 1.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 048 */ {CONSTRUCT_PORTAL, 16, -1, {-2.f, -1.f, 0.f, -3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 049 */ {CONSTRUCT_PORTAL, 21, -1, {0.f, -3.f, 2.f, -1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 050 */ {CONSTRUCT_SPENCER, -1, -1, {0.f, 0.f, PI / 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 051 */ {CONSTRUCT_CELL, -1, -1, {14.f, 11.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 052 */ {CONSTRUCT_CAMERA, 50, -1, {0.f, 0.5f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 053 */ {CONSTRUCT_WALL, -1, -1, {-3.f, 1.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 054 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, 3.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 055 */ {CONSTRUCT_PORTAL, 59, -1, {-2.f, 2.f, 2.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 056 */ {CONSTRUCT_PORTAL, 44, -1, {-2.f, -2.f, 2.f, -2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 057 */ {CONSTRUCT_PORTAL, 118, -1, {2.f, -2.f, 3.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 058 */ {CONSTRUCT_PORTAL, 71, -1, {-2.f, -2.f, -3.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 059 */ {CONSTRUCT_CELL, -1, -1, {14.f, 13.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 060 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 061 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 062 */ {CONSTRUCT_PORTAL, 51, -1, {-2.f, 0.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 063 */ {CONSTRUCT_PORTAL, 64, -1, {-2.f, 1.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 064 */ {CONSTRUCT_CELL, -1, -1, {14.f, 16.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 065 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, -3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 066 */ {CONSTRUCT_WALL, -1, -1, {2.f, -2.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 067 */ {CONSTRUCT_PORTAL, 59, -1, {-2.f, -2.f, 2.f, -2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 068 */ {CONSTRUCT_PORTAL, 98, -1, {3.f, -1.f, 2.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 069 */ {CONSTRUCT_PORTAL, 93, -1, {-3.f, -1.f, -2.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 070 */ {CONSTRUCT_PORTAL, 160, -1, {-2.f, 3.f, 2.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 071 */ {CONSTRUCT_CELL, -1, -1, {10.f, 10.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 072 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 073 */ {CONSTRUCT_WALL, -1, -1, {0.f, 2.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 074 */ {CONSTRUCT_PORTAL, 51, -1, {2.f, -1.f, 1.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 075 */ {CONSTRUCT_PORTAL, 76, -1, {-2.f, -1.f, 0.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 076 */ {CONSTRUCT_CELL, -1, -1, {8.f, 11.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 077 */ {CONSTRUCT_WALL, -1, -1, {0.f, -2.f, -2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 078 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 079 */ {CONSTRUCT_WALL, -1, -1, {2.f, 1.f, 1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 080 */ {CONSTRUCT_PORTAL, 71, -1, {0.f, -2.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 081 */ {CONSTRUCT_PORTAL, 82, -1, {-2.f, 2.f, 1.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 082 */ {CONSTRUCT_CELL, -1, -1, {7.f, 14.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 083 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 084 */ {CONSTRUCT_PORTAL, 76, -1, {-1.f, -1.f, 2.f, -1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 085 */ {CONSTRUCT_PORTAL, 123, -1, {-1.f, -1.f, -1.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 086 */ {CONSTRUCT_PORTAL, 87, -1, {-1.f, 2.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 087 */ {CONSTRUCT_CELL, -1, -1, {8.f, 16.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 088 */ {CONSTRUCT_WALL, -1, -1, {1.f, -2.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 089 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 090 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 0.f, 3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 091 */ {CONSTRUCT_PORTAL, 82, -1, {-2.f, 0.f, 1.f, -2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 092 */ {CONSTRUCT_PORTAL, 93, -1, {2.f, -1.f, 0.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 093 */ {CONSTRUCT_CELL, -1, -1, {10.f, 18.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 094 */ {CONSTRUCT_WALL, -1, -1, {0.f, -3.f, 1.f, -3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 095 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 096 */ {CONSTRUCT_PORTAL, 87, -1, {0.f, -3.f, -2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 097 */ {CONSTRUCT_PORTAL, 64, -1, {1.f, -3.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 098 */ {CONSTRUCT_CELL, -1, -1, {18.f, 18.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 099 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -3.f, 0.f, -3.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 100 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 101 */ {CONSTRUCT_PORTAL, 64, -1, {-1.f, -3.f, -2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 102 */ {CONSTRUCT_PORTAL, 103, -1, {0.f, -3.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 103 */ {CONSTRUCT_CELL, -1, -1, {20.f, 16.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 104 */ {CONSTRUCT_WALL_SWITCH, 147, -1, {0.f, 3.f, 2.f, 1.f, 0.f}, level1_switch, TEX_ID_WALL_SWITCH_ON_0, TEX_ID_WALL_SWITCH_OFF_0},
	/* 105 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 106 */ {CONSTRUCT_PORTAL, 98, -1, {-2.f, -1.f, 0.f, 3.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 107 */ {CONSTRUCT_PORTAL, 108, -1, {-1.f, -2.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 108 */ {CONSTRUCT_CELL, -1, -1, {21.f, 14.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 109 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 0.f, -2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 110 */ {CONSTRUCT_WALL, -1, -1, {1.f, 3.f, 1.f, -3.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 111 */ {CONSTRUCT_PORTAL, 103, -1, {-2.f, 0.f, 1.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 112 */ {CONSTRUCT_PORTAL, 113, -1, {-2.f, -1.f, 1.f, -3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 113 */ {CONSTRUCT_CELL, -1, -1, {20.f, 11.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 114 */ {CONSTRUCT_WALL, -1, -1, {-2.f, 1.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 115 */ {CONSTRUCT_WALL, -1, -1, {0.f, -2.f, 2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 116 */ {CONSTRUCT_PORTAL, 108, -1, {-1.f, 2.f, 2.f, 0.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 117 */ {CONSTRUCT_PORTAL, 118, -1, {-2.f, 1.f, 0.f, -2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 118 */ {CONSTRUCT_CELL, -1, -1, {18.f, 10.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 119 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_0, TEX_ID_NONE},
	/* 120 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, 0.f, 2.f, 0.f, 0.f}, 0, TEX_ID_BRICK_WALL_0, TEX_ID_NONE},
	/* 121 */ {CONSTRUCT_PORTAL, 51, -1, {-2.f, -1.f, -1.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 122 */ {CONSTRUCT_PORTAL, 113, -1, {2.f, -1.f, 0.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 123 */ {CONSTRUCT_CELL, -1, -1, {4.f, 14.f}, 0, TEX_ID_ROCKS_0, TEX_ID_STONE_FLOOR_0},
	/* 124 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, 2.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 125 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 126 */ {CONSTRUCT_PORTAL, 128, -1, {-1.f, -1.f, -1.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 127 */ {CONSTRUCT_PORTAL, 82, -1, {2.f, -1.f, 2.f, 2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 128 */ {CONSTRUCT_CELL, -1, -1, {1.f, 17.f}, 0, TEX_ID_ROCKS_0, TEX_ID_STONE_FLOOR_0},
	/* 129 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -3.f, 2.f, -4.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 130 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -3.f, -1.f, 4.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 131 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 132 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 4.f, 0.f, 4.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 133 */ {CONSTRUCT_PORTAL, 123, -1, {2.f, -4.f, 2.f, -1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 134 */ {CONSTRUCT_PORTAL, 135, -1, {2.f, 1.f, 0.f, 4.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 135 */ {CONSTRUCT_CELL, -1, -1, {5.f, 20.f}, 0, TEX_ID_ROCKS_0, TEX_ID_STONE_FLOOR_0},
	/* 136 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -2.f, 1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 137 */ {CONSTRUCT_WALL, -1, -1, {1.f, -2.f, 3.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 138 */ {CONSTRUCT_WALL, -1, -1, {3.f, 0.f, 3.f, 2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 139 */ {CONSTRUCT_PORTAL, 128, -1, {-2.f, -2.f, -4.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 140 */ {CONSTRUCT_PORTAL, 149, -1, {3.f, 2.f, -2.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 141 */ {CONSTRUCT_DOOR, 142, 147, {-4.f, 1.f, -2.f, 3.f, 1.f}, 0, TEX_ID_LEFT_DOOR_6, TEX_ID_RIGHT_DOOR_6},

	/* 142 */ {CONSTRUCT_CELL, -1, -1, {1.f, 23.f}, 0, TEX_ID_STONE_FLOOR_0, TEX_ID_STONE_FLOOR_0},
	/* 143 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -2.f, 0.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 144 */ {CONSTRUCT_WALL, -1, -1, {2.f, 0.f, 2.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 145 */ {CONSTRUCT_WALL, -1, -1, {2.f, 2.f, -1.f, 2.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 146 */ {CONSTRUCT_WALL, -1, -1, {-1.f, 2.f, -1.f, -2.f, 0.f, 0.f}, 0, TEX_ID_MOON_0, TEX_ID_NONE},
	/* 147 */ {CONSTRUCT_DOOR, 135, 141, {0.f, -2.f, 2.f, 0.f, 1.f}, 0, TEX_ID_LEFT_DOOR_5, TEX_ID_RIGHT_DOOR_5},
	/* 148 */ {CONSTRUCT_KEY, 42, -1, {0.f, 0.f, 1.f, 1.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 149 */ {CONSTRUCT_CELL, -1, -1, {8.f, 24.f}, 0, TEX_ID_ROCKS_0, TEX_ID_STONE_FLOOR_0},
	/* 150 */ {CONSTRUCT_WALL, -1, -1, {-5.f, -1.f, -5.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 151 */ {CONSTRUCT_WALL, -1, -1, {-5.f, 1.f, 5.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 152 */ {CONSTRUCT_WALL, -1, -1, {0.f, -2.f, 3.f, -2.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 153 */ {CONSTRUCT_PORTAL, 135, -1, {-5.f, -1.f, 0.f, -2.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 154 */ {CONSTRUCT_PORTAL, 155, -1, {3.f, -2.f, 5.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 155 */ {CONSTRUCT_CELL, -1, -1, {13.f, 22.f}, 0, TEX_ID_ROCKS_0, TEX_ID_STONE_FLOOR_0},
	/* 156 */ {CONSTRUCT_WALL, -1, -1, {-1.f, -1.f, -2.f, 0.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 157 */ {CONSTRUCT_WALL, -1, -1, {0.f, 3.f, 3.f, -1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 158 */ {CONSTRUCT_PORTAL, 149, -1, {-2.f, 0.f, 0.f, 3.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 159 */ {CONSTRUCT_PORTAL, 160, -1, {-1.f, -1.f, 3.f, -1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},

	/* 160 */ {CONSTRUCT_CELL, -1, -1, {14.f, 20.f}, 0, TEX_ID_ROCKS_0, TEX_ID_STONE_FLOOR_0},
	/* 161 */ {CONSTRUCT_WALL, -1, -1, {-2.f, -1.f, -2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 162 */ {CONSTRUCT_WALL, -1, -1, {2.f, -1.f, 2.f, 1.f, 0.f, 0.f}, 0, TEX_ID_STONE_WALL_1, TEX_ID_NONE},
	/* 163 */ {CONSTRUCT_PORTAL, 155, -1, {-2.f, 1.f, 2.f, 1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 164 */ {CONSTRUCT_PORTAL, 64, -1, {-2.f, -1.f, 2.f, -1.f}, 0, TEX_ID_NONE, TEX_ID_NONE},
	/* 165 */ {CONSTRUCT_SNOWMAN, 50, -1, {0.f, 0.f, 3.f * PI / 2.f}, 0, TEX_ID_SNOW_MAN_0, TEX_ID_SNOW_MAN_1},
};

map level1_map =
{
	level1_map_constructs,
	sizeof(level1_map_constructs) / sizeof(construct),
	STATIC_CELL_MODE,
};

// endof level1.cpp