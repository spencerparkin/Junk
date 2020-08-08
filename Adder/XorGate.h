// XorGate.h

#ifndef __XorGate_h__
#define __XorGate_h__

#include "BinaryGate.h"

class XorGate : public BinaryGate
{
	public:

	XorGate(GLfloat x, GLfloat y, GLfloat angle);
	~XorGate();

	void Draw(GLenum mode);
	void Operate(void);
	void Tweak(void);
};

#endif __XorGate_h__

// endof XorGate.h