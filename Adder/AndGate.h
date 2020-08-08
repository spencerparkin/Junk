// AndGate.h

#ifndef __AndGate_h__
#define __AndGate_h__

#include "BinaryGate.h"

class AndGate : public BinaryGate
{
	public:

	AndGate(GLfloat x, GLfloat y, GLfloat angle);
	~AndGate();

	void Draw(GLenum mode);
	void Operate(void);
	void Tweak(void);
};

#endif __AndGate_h__

// endof AndGate.h