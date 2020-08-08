// XorGate.cpp

#include "XorGate.h"

XorGate::XorGate(GLfloat x, GLfloat y, GLfloat angle) : BinaryGate(x, y, angle)
{
}

XorGate::~XorGate()
{
}

void XorGate::Draw(GLenum mode)
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
		// Draw the schematic symbole for an XOR gate.
		glBegin(GL_LINES);
		glVertex2f(-0.7f, 1.f);
		glVertex2f(1.f, 1.f);
		glVertex2f(-0.7, -1.f);
		glVertex2f(1.f, -1.f);
		glEnd();
		DrawArc(-1.f, 0.f, -PI / 2.f, 1.f, PI, 5);
		DrawArc(-0.7f, 0.f, -PI / 2.f, 1.f, PI, 5);
		DrawArc(1.f, 0.f, -PI / 2.f, 1.f, PI, 5);

		// Draw the prongs.
		glBegin(GL_LINES);
		glVertex2f(-0.5f, 0.8f);
		glVertex2f(-1.5f, 0.8f);
		glVertex2f(-0.5f, -0.8f);
		glVertex2f(-1.5f, -0.8f);
		glVertex2f(2.f, 0.f);
		glVertex2f(2.5f, 0.f);
		glEnd();

		// Draw the input and output states.
		if(input0)
			DrawBit(-1.25f, 1.f, 1.f, input0->output);
		if(input1)
			DrawBit(-1.25f, -0.6f, 1.f, input1->output);
		DrawBit(2.25f, 0.2f, 1.f, output);
	}

	glPopMatrix();

	if(mode == GL_RENDER)
	{
		// Draw circuit connections, if any.
		if(input0)
			DrawConnection(this, input0, -1.5f, 0.8f, 2.5f, 0.f);
		if(input1)
			DrawConnection(this, input1, -1.5f, -0.8f, 2.5f, 0.f);
	}
}

void XorGate::Operate(void)
{
	if(InputIsReady())
	{
		if(input0->output == input1->output)
			output = ZERO;
		else
			output = ONE;
	}
	else
		output = NOTHING;
}

void XorGate::Tweak(void)
{
	Operate();
}

// endof XorGate.cpp