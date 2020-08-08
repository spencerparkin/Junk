// NotGate.h

#ifndef __NotGate_h__
#define __NotGate_h__

#include "BinaryGate.h"

// This gate is actually a unary gate.
// So it will only operate upon the first input.
class NotGate : public BinaryGate
{
	public:

	NotGate(GLfloat x, GLfloat y, GLfloat angle);
	~NotGate();

	void Draw(GLenum mode);
	void Operate(void);
	void Tweak(void);
};

#endif __NotGate_h__

// endof NotGate.h