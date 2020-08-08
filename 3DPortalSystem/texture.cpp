// texture.cpp

// Programmed by Spencer T. Parkin

#define _CRT_TERMINATE_DEFINED

#include <glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiff.h>
#include <tiffio.h>
#include "texture.h"
#include "macros.h"
#include "main.h"
#include <direct.h>

static texture *tex_list = 0;

void InitTextureManager(void)
{
	tex_list = 0;

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Find the texture data directory.
	if(chdir("TextureData") < 0)
	{
		// Try one directory back.
		chdir("..");
		if(chdir("TextureData") < 0)
			fatal_error("Failed to find \"TextureData\" directory!");
		else
			chdir("..");
	}
	else
		chdir("..");
}

void DeinitTextureManager(void)
{
	while(tex_list)
	{
		tex_list->ref_count = 1;
		UnloadTexture(tex_list);
	}
}

// This routine should get called once per frame
// after the scene has been drawn.  Notice that
// it really doesn't work because some draw routines
// are drawing the textures only once into a display-
// list, and then calling the list instead of the
// Begin/End texture routines.
void ManageTextures(void)
{
	texture *t;
	
	// Find out which texture(s) is/are used the most.
	int max_use_count = 0;
	for(t = tex_list; t; t = t->next)
		max_use_count = MAX(t->use_count, max_use_count);
	
	// Now go assign priorities accordingly.
	float priority;
	for(t = tex_list; t; t = t->next)
	{
		priority = float(t->use_count) / float(max_use_count);
		glPrioritizeTextures(1, &t->name, &priority);
		t->use_count = 0;
	}
}

// This is a way of indicating that the
// given texture was used, even though a
// Begin/End texture application was not
// specified.  This is done if a display
// list is called, for example.
void UsedTexture(texture *t)
{
	t->use_count++;
}

// Look-up a texture's filename by texture ID.
static const char *TextureFilename(int tex_id)
{
	switch(tex_id)
	{
		case TEX_ID_BRICK_WALL_0:			return("TextureData/BrickWall0.tif");
		case TEX_ID_ROCKS_0:				return("TextureData/Rocks0.tif");
		case TEX_ID_STONE_WALL_0:			return("TextureData/StoneWall0.tif");
		case TEX_ID_STONE_WALL_1:			return("TextureData/StoneWall1.tif");
		case TEX_ID_STONE_FLOOR_0:			return("TextureData/StoneFloor0.tif");
		case TEX_ID_MOON_0:					return("TextureData/Moon0.tif");
		case TEX_ID_MOSAIC_0:				return("TextureData/Mosaic0.tif");
		case TEX_ID_REPTILE_SKIN_0:			return("TextureData/ReptileSkin0.tif");
		case TEX_ID_MARBLE_0:				return("TextureData/Marble0.tif");
		case TEX_ID_COLIN_MOCHRIE_0:		return("TextureData/Colin0.tif");
		case TEX_ID_COLIN_MOCHRIE_1:		return("TextureData/Colin1.tif");
		case TEX_ID_COLIN_MOCHRIE_2:		return("TextureData/Colin2.tif");
		case TEX_ID_RYAN_STILES_0:			return("TextureData/Ryan0.tif");
		case TEX_ID_RYAN_STILES_1:			return("TextureData/Ryan1.tif");
		case TEX_ID_RYAN_STILES_2:			return("TextureData/Ryan2.tif");
		case TEX_ID_WAYNE_BRADY_0:			return("TextureData/Wayne0.tif");
		case TEX_ID_WAYNE_BRADY_1:			return("TextureData/Wayne1.tif");
		case TEX_ID_WAYNE_BRADY_2:			return("TextureData/Wayne2.tif");
		case TEX_ID_GREG_PROOPS_0:			return("TextureData/Greg0.tif");
		case TEX_ID_GREG_PROOPS_1:			return("TextureData/Greg1.tif");
		case TEX_ID_GREG_PROOPS_2:			return("TextureData/Greg2.tif");
		case TEX_ID_DREW_CAREY_0:			return("TextureData/Drew0.tif");
		case TEX_ID_CLIVE_ANDERSON_0:		return("TextureData/Clive0.tif");
		case TEX_ID_LEFT_DOOR_0:			return("TextureData/LeftDoor0.tif");
		case TEX_ID_LEFT_DOOR_1:			return("TextureData/LeftDoor1.tif");
		case TEX_ID_LEFT_DOOR_2:			return("TextureData/LeftDoor2.tif");
		case TEX_ID_LEFT_DOOR_3:			return("TextureData/LeftDoor3.tif");
		case TEX_ID_LEFT_DOOR_4:			return("TextureData/LeftDoor4.tif");
		case TEX_ID_LEFT_DOOR_5:			return("TextureData/LeftDoor5.tif");
		case TEX_ID_LEFT_DOOR_6:			return("TextureData/LeftDoor6.tif");
		case TEX_ID_RIGHT_DOOR_0:			return("TextureData/RightDoor0.tif");
		case TEX_ID_RIGHT_DOOR_1:			return("TextureData/RightDoor1.tif");
		case TEX_ID_RIGHT_DOOR_2:			return("TextureData/RightDoor2.tif");
		case TEX_ID_RIGHT_DOOR_3:			return("TextureData/RightDoor3.tif");
		case TEX_ID_RIGHT_DOOR_4:			return("TextureData/RightDoor4.tif");
		case TEX_ID_RIGHT_DOOR_5:			return("TextureData/RightDoor5.tif");
		case TEX_ID_RIGHT_DOOR_6:			return("TextureData/RightDoor6.tif");
		case TEX_ID_WALL_SWITCH_ON_0:		return("TextureData/WallSwitchOn0.tif");
		case TEX_ID_WALL_SWITCH_OFF_0:		return("TextureData/WallSwitchOff0.tif");
		case TEX_ID_SNOW_MAN_0:				return("TextureData/SnowMan0.tif");
		case TEX_ID_SNOW_MAN_1:				return("TextureData/SnowMan1.tif");
		case TEX_ID_SNOW_MAN_2:				return("TextureData/SnowMan2.tif");
		case TEX_ID_RED_STONE_WALL_0:		return("TextureData/RedStoneWall0.tif");
		case TEX_ID_GREEN_STONE_WALL_0:		return("TextureData/GreenStoneWall0.tif");
		case TEX_ID_BLUE_STONE_WALL_0:		return("TextureData/BlueStoneWall0.tif");
		case TEX_ID_CYAN_STONE_WALL_0:		return("TextureData/CyanStoneWall0.tif");
		case TEX_ID_MAGENTA_STONE_WALL_0:	return("TextureData/MagentaStoneWall0.tif");
		case TEX_ID_YELLOW_STONE_WALL_0:	return("TextureData/YellowStoneWall0.tif");
		case TEX_ID_GALAXY_0:				return("TextureData/Galaxy0.tif");
		case TEX_ID_PLANET_0:				return("TextureData/Planet0.tif");
		case TEX_ID_BOOK_SHELF_0:			return("TextureData/BookShelf0.tif");
		case TEX_ID_SPENCER_WALK_0:			return("TextureData/SpencerWalk0.tif");
		case TEX_ID_SPENCER_WALK_1:			return("TextureData/SpencerWalk1.tif");
		case TEX_ID_SPENCER_WALK_2:			return("TextureData/SpencerWalk2.tif");
		case TEX_ID_SPENCER_WALK_3:			return("TextureData/SpencerWalk3.tif");
		case TEX_ID_SPENCER_STAND_0:		return("TextureData/SpencerStand0.tif");
	}

	return 0;
}

// Return true if the given integer is
// a power of two.  Return false otherwise.
static bool is_pow_of_two(unsigned int x)
{
	while(!(x % 2))
		x /= 2;
	if(x == 1)
		return true;
	return false;
}

// Load the pixel data from a TIF file.
static GLubyte *LoadTIFTextureData(const char *filename, GLsizei *width, GLsizei *height)
{
	TIFF *tif = TIFFOpen(filename, "r");
	if(!tif)
	{
		fprintf(stderr, "Failed to open TIF file %s!\n", filename);
		return 0;
	}
	else
		printf("Loading texture %s...\n", filename);

	// Get information about this TIF file.
	unsigned short bps, spp;
	unsigned int rps;
	unsigned short planar_config;
	unsigned short photo_metric;
	unsigned int img_width, img_height;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &img_width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &img_height);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rps);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar_config);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photo_metric);
	printf("\tDimensions: %d x %d\n", img_width, img_height);
	printf("\tBits per sample: %d\n", bps);
	printf("\tSamples per pixel: %d\n", spp);
	printf("\tRows per strip: %d\n", rps);
	printf("\tPlanar configuration: %d\n", planar_config);
	printf("\tPhoto metric: %d\n", photo_metric);

	// OpenGL can only handle images with dimensions that are powers of two.
	if(!is_pow_of_two(img_width) || !is_pow_of_two(img_height))
	{
		fprintf(stderr, "This image does not have dimensions that are powers of two!\n");
		TIFFClose(tif);
		return 0;
	}

	// Barf if the TIF is compressed.  I'm not going to worry about compression.
	if(planar_config != 1)
	{
		fprintf(stderr, "Arf!  I'm too lazy to decompress your TIF file!\n");
		TIFFClose(tif);
		return 0;
	}

	// I need to write support for reading palatized images.
	// Reading an 8-bit image isn't hard, I just don't know
	// where the CLUT is in TIFF yet.
	if(spp == 1)
	{
		printf("Warning: assuming 8-bit image is not palatized...\n");
	}

	// Allocate a buffer for 32-bit pixel data.
	int buf_size = img_width * img_height * sizeof(GLubyte) * 4;
	GLubyte *data = new GLubyte[buf_size];
	memset((void *)data, 0, buf_size);

	// Allocate a buffer to hold one scanline array of texel data.
	tsize_t scanline_size = TIFFScanlineSize(tif);
	tdata_t scanline = _TIFFmalloc(scanline_size);

	// Now read in the texture data one scanline at a time
	printf("Reading texture data...");
	GLubyte *texel = data;
	int i, j, k;
	for(i = (signed)img_height - 1; i >= 0; i--)
	{
		// Read in the next scanline.
		if(TIFFReadScanline(tif, scanline, i) < 0)
		{
			fprintf(stderr, "Failed to read scanline %d from file %s!\n", i, filename);
			continue;
		}

		// Copy the scanline data into the texture buffer.
		for(j = 0; j < (signed)img_width; j++)
		{
			// Copy the texel data.
			for(k = 0; k < 4; k++)
			{
				if(k < spp)
					texel[k] = ((GLubyte *)scanline + j * spp)[k];
				else
					texel[k] = 0xFF;
			}

			// This RGB color is reserved for indicating full alpha
			// until I can figure out how to create images with alpha
			// in photoshop.  (Stupid photoshop.)
			if(texel[0] == 152 && texel[1] == 0 && texel[2] == 136)
				texel[3] = 0x00;
			else
				texel[3] = 0xFF;

			// Next texel...
			texel += 4;
		}

		printf(".");
		fflush(stdout);
	}

	printf("done!\n");

	// Clean up.
	_TIFFfree(scanline);
	TIFFClose(tif);

	// Return the texture data.
	*width = img_width;
	*height = img_height;
	return data;
}

// Load the pixel data from a BMP file.
static GLubyte *LoadBMPTextureData(const char *filename, GLsizei *width, GLsizei *height)
{
	fatal_error("BMP format not supported yet!");
	return 0;
}

// Figure out what type of image
// file we're trying to load and
// go load it.
static GLubyte *LoadTextureData(const char *filename, GLsizei *width, GLsizei *height)
{
	const char *str = filename;
	while(*str != '.' && *str != '\0')
		str++;
	if(strcmp(str, ".tif") == 0)
		return LoadTIFTextureData(filename, width, height);
	if(strcmp(str, ".bmp") == 0)
		return LoadBMPTextureData(filename, width, height);
	// Unsupported texture format!
	fprintf(stderr, "\"%s\" is an unsupported texture format!\n", str);
	return 0;
}

// Return a texture object to the caller
// corresponding to the given texture ID.
// This texture object can be used to render
// texture-mapped polygons.
texture *LoadTexture(int tex_id)
{
	texture *t;

	// Go see if the desired texture
	// is already loaded.  If it is,
	// increment the reference count and
	// return the already loaded texture data.
	for(t = tex_list; t; t = t->next)
	{
		if(t->tex_id == tex_id)
		{
			t->ref_count++;
			return t;
		}
	}

	// Allocate and initialize a texture object.
	t = new texture;
	t->tex_id = tex_id;
	t->filename = TextureFilename(tex_id);
	t->ref_count = 1;
	t->use_count = 0;
	t->data = LoadTextureData(t->filename, &t->width, &t->height);
	if(!t->data)
	{
		fprintf(stderr, "Failed to load texture data from file %s!\n", t->filename);
		delete t;
		return 0;
	}

	// Create an OpenGL texture object and initialize its state.
	glGenTextures(1, &t->name);
	glBindTexture(GL_TEXTURE_2D, t->name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#if 1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else	// This is the default because it looks better.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->width, t->height,
								0, GL_RGBA, GL_UNSIGNED_BYTE, t->data);

	// Put this texture into the list of texture.
	t->next = tex_list;
	if(tex_list)
		t->next->prev = t;
	t->prev = 0;
	tex_list = t;

	// Finally, return the texture.
	return t;
}

// Decrement the reference count
// for the given texture.  When
// nobody is using it anymore,
// free the texture data.
void UnloadTexture(texture *t)
{
	if(--t->ref_count <= 0)
	{
		// Remove the texture from the list.
		if(t == tex_list)
			tex_list = tex_list->next;
		if(t->next)
			t->next->prev = t->prev;
		if(t->prev)
			t->prev->next = t->next;

		// Free the texture data.
		glDeleteTextures(1, &t->name);
		delete t->data;
		delete t;
	}
}

// Call this function before rendering
// a polygon with the given texture mapped to it.
// Also, bump up the use count so that we can
// give priority to the textures in use.
void BeginTextureMapping(texture *t)
{
	if(!t)
		return;

	// If the texture has alpha in it,
	// then enable blending.
	if(t->tex_id == TEX_ID_SNOW_MAN_0 ||
		t->tex_id == TEX_ID_SNOW_MAN_1 ||
		t->tex_id == TEX_ID_SNOW_MAN_2 ||
		t->tex_id == TEX_ID_SPENCER_WALK_0 ||
		t->tex_id == TEX_ID_SPENCER_WALK_1 ||
		t->tex_id == TEX_ID_SPENCER_WALK_2 ||
		t->tex_id == TEX_ID_SPENCER_WALK_3 ||
		t->tex_id == TEX_ID_SPENCER_STAND_0)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// What this will do is prevent the textured
		// polygon from writing into the Z-buffer,
		// though a depth test is still performed to
		// see which fragments to keep and which to discard.
		// So by doing this, polygons that draw behind this
		// texture, but after this texture is drawn,
		// will possibly overwrite this textured polygon.
		// So this still presents a problem unless I can
		// find a way to draw textures with transparency
		// very last in the list of polygons drawn.  This is
		// because blending only occurs between an incoming
		// fragment and what is already in the F-buffer.  It would
		// be nice if OpenGL wrote into the Z-buffer only
		// the fragments with full opacity, and not the
		// fragments with full transparency, or if OpenGL
		// simply drew fragments with full opacity and
		// discarded fragments with full transparency.
//		glDepthMask(GL_FALSE);		//	We really don't need/want this.
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t->name);
	t->use_count++;
}

// Call this function after rendering a
// polygon that has been textured with
// the given texture.
void EndTextureMapping(texture *t)
{
	if(!t)
		return;

	glDisable(GL_TEXTURE_2D);

	// The following textures have alpha in them.
	if(t->tex_id == TEX_ID_SNOW_MAN_0 ||
		t->tex_id == TEX_ID_SNOW_MAN_1 ||
		t->tex_id == TEX_ID_SNOW_MAN_2 ||
		t->tex_id == TEX_ID_SPENCER_WALK_0 ||
		t->tex_id == TEX_ID_SPENCER_WALK_1 ||
		t->tex_id == TEX_ID_SPENCER_WALK_2 ||
		t->tex_id == TEX_ID_SPENCER_WALK_3 ||
		t->tex_id == TEX_ID_SPENCER_STAND_0)
	{
		glDisable(GL_BLEND);
//		glDepthMask(GL_TRUE);		//	We really don't need/want this.
	}
}

// endof texture.cpp