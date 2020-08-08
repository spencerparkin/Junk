// NotGate.cpp

#include <GL/glut.h>
#include "NotGate.h"

NotGate::NotGate(GLfloat x, GLfloat y, GLfloat angle) : BinaryGate(x, y, angle)
{
}

NotGate::~NotGate()
{
}

void NotGate::Draw(GLenum mode)
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
		// Draw the schematic symbol for a NOT gate.
		glBegin(GL_LINE_LOOP);
		glVertex2f(-1.f, -1.f);
		glVertex2f(-1.f, 1.f);
		glVertex2f(1.f, 0.f);
		glEnd();
		DrawArc(1.2f, 0.f, 0.f, 0.2f, 2.f * PI, 20);

		// Draw the prongs.
		glBegin(GL_LINES);
		glVertex2f(1.4f, 0.f);
		glVertex2f(2.5f, 0.f);
		glVertex2f(-1.f, 0.f);
		glVertex2f(-1.5f, 0.f);
		glEnd();

		// Draw the input and output states.
		if(input0)
			DrawBit(-1.25f, 0.2f, 1.f, input0->output);	
		DrawBit(2.25f, 0.2f, 1.f, output);
	}

	glPopMatrix();

	if(mode == GL_RENDER)
	{
		// Draw circuit connections, if any.	
		if(input0)	
			DrawConnection(this, input0, -1.5f, 0.f, 2.5f, 0.f);
	}
}

// Note that input1 is ignored.
void NotGate::Operate(void)
{
	if(input0 && input0->output != NOTHING)
	{
		if(input0->output == ONE)
			output = ZERO;
		else
			output = ONE;
	}
	else
		output = NOTHING;
}

void NotGate::Tweak(void)
{
	Operate();
}

// endof NotGate.cpp