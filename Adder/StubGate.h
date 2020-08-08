// StubGate.h

#ifndef __StubGate_h__
#define __StubGate_h__

#include "BinaryGate.h"

// This gate is actually a unary gate.
// It performs no operation on its input.
// It's only perpose is to display an
// input or an output value of the
// dynamically displayed schematic.
class StubGate : public BinaryGate
{
	public:

	StubGate(GLfloat x, GLfloat y, GLfloat angle, int bit);
	~StubGate();

	void Draw(GLenum mode);
	void Operate(void);
	void Tweak(void);

	int bit;
};

#endif __StubGate_h__

// StubGate.h