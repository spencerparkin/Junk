// OrGate.h

#ifndef __OrGate_h__
#define __OrGate_h__

#include "BinaryGate.h"

class OrGate : public BinaryGate
{
	public:

	OrGate(GLfloat x, GLfloat y, GLfloat angle);
	~OrGate();

	void Draw(GLenum mode);
	void Operate(void);
	void Tweak(void);
};

#endif __OrGate_h__

// endof OrGate.h