// construct.h

// Programmed by Spencer T. Parkin

#ifndef __construct_h__
#define __construct_h__

// Constructs are elements of a map.
// A map is used to generate a scene-graph.

enum
{
	CONSTRUCT_FUDGE,		// This is a place-holder construct.
	CONSTRUCT_CELL,
	CONSTRUCT_WALL,
	CONSTRUCT_PORTAL,
	CONSTRUCT_DOOR,
	CONSTRUCT_WALL_SWITCH,
	CONSTRUCT_CAMERA,
	CONSTRUCT_KEY,
	CONSTRUCT_ORB,
	CONSTRUCT_SNOWMAN,
	CONSTRUCT_BLOB,
	CONSTRUCT_SPENCER,
	CONSTRUCT_HEALTH_PACK,
	CONSTRUCT_TELEPORT,
	NUMBER_OF_CONSTRUCTS,
};

struct construct
{
	int type;				// The type of construct.
	int index0;				// A reference to some other associated construct.
	int index1;				// And yet another.
	float data[16];			// Floating-point data for this construct.
	void *ptr;				// A pointer to something.  Who knows what for now.
	int tex_id0;			// Used if this construct has a texture.
	int tex_id1;			// And yet another.
	void *mem;				// Temorary memory pointer used during map un/loading.
};

#endif __construct_h__

// endof construct.h