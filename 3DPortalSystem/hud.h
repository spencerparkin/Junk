// hud.h

// Programmed by Spencer T. Parkin

#ifndef __hud_h__
#define __hud_h__

bool InitHUD(class Entity *e);
bool DeinitHUD(void);
void DrawHUD(void);

// This is the entity that the HUD
// displays information about.
extern Entity *hud_e;

#endif __hud_h__

// endof hud.h