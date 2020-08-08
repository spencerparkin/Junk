// Texture.cpp

#include "Texture.h"
#include <string.h>
#include <assert.h>

static List TextureList;

void InitTextureManager(void)
{
	WipeList(&TextureList);
}

void DeinitTextureManager(void)
{
	Texture *tex;
	while(TextureList.item_count > 0)
	{
		tex = (Texture *)TextureList.head;
		ListRemove((ListItem *)tex);
		UnloadTexture(tex);
	}
}

void ManageTextureMemory(void)
{
	// manage texture priority

	// maybe monitor residency...

	// think about it.
}

static Texture *LoadTIFTexture(char *filename)
{
	return 0;
}

static Texture *LoadBMPTexture(char *filename)
{
	return 0;
}

Texture *LoadTexture(char *filename)
{
	// Search the list of textures to
	// see if the desired texture is
	// already loaded.  If it is, return
	// that texture instead of creating
	// a new one.  For large applications,
	// using a binary tree to store the
	// textures would be a good idea.
	Texture *tex;
	for(tex = (Texture *)TextureList.head; tex; tex = tex->next)	
		if(strcmp(tex->filename, filename) == 0)
			return tex;		

	// What type of image file is it?
	char *ext = filename;
	while(*ext != '.' && *ext != '\0')
		ext++;

	// Go load it up.
	if(strcmp(ext, ".tif") == 0)
		return LoadTIFTexture(filename);
	if(strcmp(ext, ".bmp") == 0)
		return LoadBMPTexture(filename);

	// Oops; image format unsupported.
	return 0;
}

// Notice that this unloads the texture
// for everyone.  No reference couting
// is done here.
void UnloadTexture(Texture *tex)
{
	ListRemove((ListItem *)tex);
	delete [] tex->data;
	delete tex;
}

// One calls this before drawing
// a texture-mapped polygon.
void ApplyTexture(Texture *tex)
{
	// bind this texture and
	// setup its material properties.
}

// endof Texture.cpp