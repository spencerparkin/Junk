// StubGate.cpp

#include <GL/glut.h>
#include "StubGate.h"

StubGate::StubGate(GLfloat x, GLfloat y, GLfloat angle, int bit) : BinaryGate(x, y, angle)
{
	// If the given bit is not the indeterminant
	// state, then this stub acts as an input into
	// the digital circuit.  Otherwise, this gate
	// acts as an output, intermediary or final.
	this->bit = bit;

	Operate();
}

StubGate::~StubGate()
{
}

void StubGate::Draw(GLenum mode)
{
	glColor3f(0.f, 0.f, 0.f);

	glPushMatrix();
	glTranslatef(x, y, 0.f);
	glRotatef(angle * 180.f / PI, 0.f, 0.f, 1.f);

	if(mode == GL_SELECT)
	{
		// Draw a hot-spot for selection.
		glBegin(GL_QUADS);
		glVertex2f(-1.5f, 1.f);
		glVertex2f(1.5f, 1.f);
		glVertex2f(1.5f, -1.f);
		glVertex2f(-1.5f, -1.f);
		glEnd();
	}
	else
	{
		glBegin(GL_LINE_LOOP);
		glVertex2f(-2.f, 1.f);
		glVertex2f(2.f, 1.f);
		glVertex2f(2.f, -1.f);
		glVertex2f(-2.f, -1.f);
		glEnd();

		// Draw the prongs.
		glBegin(GL_LINES);	
		glVertex2f(2.f, 0.f);
		glVertex2f(2.5f, 0.f);
		glVertex2f(-2.f, 0.f);
		glVertex2f(-2.5f, 0.f);
		glEnd();

		if(bit != NOTHING)
			DrawBit(0.f, 0.f, 3.f, bit);
		else if(input0)
			DrawBit(0.f, 0.f, 3.f, input0->output);
	}

	glPopMatrix();

	if(mode == GL_RENDER)
	{
		if(input0)
			DrawConnection(this, input0, -2.5f, 0.f, 2.5f, 0.f);
	}
}

// Note that input1 is ignored.
void StubGate::Operate(void)
{
	if(bit != NOTHING)
		output = bit;
	else if(input0)
		output = input0->output;
}

void StubGate::Tweak(void)
{
	if(bit != NOTHING)
		bit = (bit == ONE) ? ZERO : ONE;
	Operate();
}

// endof StubGate.cpp