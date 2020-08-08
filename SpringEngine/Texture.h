// Texture.h

#ifndef __Texture_h__
#define __Texture_h__

#include "List.h"
#include <GL/glut.h>

// a texture will contain texel data and material properties.
// the texture manager should be smart enough to not reload
// already loaded textures.  opengl texture objects will
// be used here.  texture management should include the balancing
// of texture priority and texture memory.  find a way to
// query for the amount of used and available texture memory,
// and whether or not it is resident texture memory.  support
// tif and the ability to expand the code to support more format.

struct Texture
{
	Texture *next, *prev;
	List *list;
	GLint name;
	char filename[64];
	GLubyte *data;
	int width, height;
};

void InitTextureManager(void);
void DeinitTextureManager(void);
void ManageTextureMemory(void);
Texture *LoadTexture(char *filename);
void UnloadTexture(Texture *tex);
void ApplyTexture(Texture *tex);

#endif __Texture_h__

// endof Texture.h