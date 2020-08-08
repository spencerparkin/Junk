// map.h

// Programmed by Spencer T. Parkin

#ifndef __map_h__
#define __map_h__

// A map is merely a list of constructs.
// The constructs define the geometry,
// topology, and contents of the map.
// This list is ordered.  Specifically,
// non-cell constructs are built in the
// context of cell constructs.  And
// some construct may require pointers
// to other constructs in the list.

#include "construct.h"

enum
{
	DYNAMIC_CELL_MODE,
	STATIC_CELL_MODE,
};

extern int cell_mode;

struct map
{
	construct *con_list;
	int con_count;
	int cell_mode;
};

bool LoadMap(map *m);
bool UnloadMap(map *m);

extern map debug_map;
extern map *game_map[];
extern const int num_game_maps;

#endif __map_h__

// endof map.h