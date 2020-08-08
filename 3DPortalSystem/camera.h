// camera.h

// Programmed by Spencer T. Parkin

#ifndef __camera_h__
#define __camera_h__

#include "entity.h"

class Camera : public Entity
{
	public:

	Camera();
	~Camera();

	virtual bool IsVisible(fovi *f);
	void Draw(fovi *f);
	void Drive(void);
	void SwitchMode(int mode);
	void TetherBreakNotify(struct tether *teth);

	Entity *e;		// The camera's subject if in 1st or 3rd person mode.
	struct tether *cam_teth;
	int mode;
};

enum
{
	CAM_MODE_TOPOLOGICAL,
	CAM_MODE_DEBUG,
	CAM_MODE_1ST_PERSON,
	CAM_MODE_3RD_PERSON,
};

extern Camera *cam;
extern float aspect_ratio;

#define CAM_FOLLOW_DIST		2.f

#endif __camera_h__

// endof camera.h