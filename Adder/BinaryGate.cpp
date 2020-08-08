// BinaryGate.cpp

#include "BinaryGate.h"

BinaryGate::BinaryGate(GLfloat x, GLfloat y, GLfloat angle)
{
	output = NOTHING;
	input0 = 0;
	input1 = 0;	

	this->x = x;
	this->y = y;
	this->angle = angle;
}

BinaryGate::~BinaryGate()
{
}

// Tell the caller if this digital component
// has input ready and is therefore able
// to operate upon it.
int BinaryGate::InputIsReady(void)
{
	if(!input0 || !input1)
		return 0;

	if(input0->output == NOTHING ||
		input1->output == NOTHING)
		return 0;

	return 1;
}

// Simply draw all the gates.  This routine
// is called on a per-frame basis.
void DrawCircuit(BinaryGate **gate, int num_gates)
{
	int k;
	for(k = 0; k < num_gates; k++)
		gate[k]->Draw(GL_RENDER);
}

// Simply operate all the gates.
// This does one iteration of circuit
// simulation.  Call this routine repeatedly
// to simulate the running of the circuit.
// Note that this circuit will not nessisarily
// update every gate since each gate is operated
// in an arbitrary order.  This is why this routine
// should be called repeatedly.  Once per frame, perhaps.
void IterateCircuit(BinaryGate **gate, int num_gates)
{
	int k;
	for(k = 0; k < num_gates; k++)
		gate[k]->Operate();
}

// Draw an arc centered at the given location starting at
// the given angle, CCW the given radians, at the given
// radius, with line segments in the number of given steps.
void DrawArc(GLfloat x, GLfloat y, GLfloat angle, GLfloat radius, GLfloat arc_radians, int num_steps)
{
	// Arc-radians is the arc-length of the arc
	// about the unit circle.
	double angle_delta = arc_radians / double(num_steps);

	// Draw the arc as a set of connected line segments.
	glBegin(GL_LINE_STRIP);
	int k;
	for(k = 0; k <= num_steps; k++)
	{
		glVertex2f(x + radius * cos(angle),
					y + radius * sin(angle));
		angle += angle_delta;
	}
	glEnd();
}

// Draw a connection between the two given gates.
// The coordinates passed in are relative to the
// coordinates of each gate.  This is a convenient
// routine that takes into account the rotational
// orientation of the digital component.
void DrawConnection(BinaryGate *gate0, BinaryGate *gate1, GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1)
{
	void RotatePoint(GLfloat *x, GLfloat *y, GLfloat angle);
	RotatePoint(&x0, &y0, gate0->angle);
	RotatePoint(&x1, &y1, gate1->angle);
	DrawConnection(gate0->x + x0, gate0->y + y0, gate1->x + x1, gate1->y + y1);
}

// Draw a set of 3 connected line segments from one
// given point to the other so that the middle of
// these is orthogonal to the other two.
void DrawConnection(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1)
{
#if 0
	double dx = fabs(x0 - x1);
	double dy = fabs(y0 - y1);
	
	glBegin(GL_LINE_STRIP);

	if(dx > dy)
	{
		double x = (x0 + x1) / 2.f;
		glVertex2f(x0, y0);
		glVertex2f(x, y0);
		glVertex2f(x, y1);
		glVertex2f(x1, y1);
	}
	else
	{
		double y = (y0 + y1) / 2.f;
		glVertex2f(x0, y0);
		glVertex2f(x0, y);
		glVertex2f(x1, y);
		glVertex2f(x1, y1);
	}

	glEnd();
#else
	// Just draw the lines diagonaly.
	glBegin(GL_LINES);
	glVertex2f(x0, y0);
	glVertex2f(x1, y1);
	glEnd();
#endif
}

// Draw a zero, a one, or nothin'.
void DrawBit(GLfloat x, GLfloat y, GLfloat scale, int bit)
{
	glPushMatrix();
	glTranslatef(x, y, 0.f);
	glScalef(scale, scale, 1.f);

	switch(bit)
	{
		case ZERO:
		{
			DrawArc(0.f, 0.f, 0.f, 0.1f, 2.f * PI, 20);
			break;
		}
		case ONE:
		{
			glBegin(GL_LINES);
			glVertex2f(0.f, -0.1f);
			glVertex2f(0.f, 0.1f);			
			glEnd();
			break;
		}
		case NOTHING:
		default:
		{
			// Draw nothing.
			break;
		}
	}

	glPopMatrix();
}

// Wipe the output of each gate in the given circuit.
void WipeCircuit(BinaryGate **gate, int num_gates)
{
	int k;
	for(k = 0; k < num_gates; k++)
		gate[k]->output = NOTHING;
}

// Draw all the given gates in GL_SELECT mode,
// then process the hit records and determine
// which gate it was that was selected.
BinaryGate *SelectGate(BinaryGate **gate, int num_gates, GLfloat mouse_x, GLfloat mouse_y)
{
	GLuint SelectBuffer[512];
	GLint SelectHits;
	BinaryGate *SelectedGate = 0;

	glSelectBuffer(512, SelectBuffer);
	(void)glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(-1);

	void SetupProjectionMatrix(GLenum mode, int mouse_x, int mouse_y);
	SetupProjectionMatrix(GL_SELECT, mouse_x, mouse_y);

	int i, k;
	for(k = 0; k < num_gates; k++)
	{
		glPushName(k);
		gate[k]->Draw(GL_SELECT);
	}

	SelectHits = glRenderMode(GL_RENDER);

	GLuint *HitData = SelectBuffer;
	GLuint NumNames;

	for(i = -1, k = 0; k < SelectHits; k++)
	{
		NumNames = *HitData++;
		HitData += NumNames + 1;
		i = *HitData++;
	}

	if(i >= 0 && i < num_gates)
		SelectedGate = gate[i];

	return SelectedGate;
}

// endof BinaryGate.cpp