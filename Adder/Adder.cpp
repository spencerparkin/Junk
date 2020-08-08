// Adder.cpp

#include "Adder.h"
#include "AndGate.h"
#include "OrGate.h"
#include "XorGate.h"
#include "StubGate.h"

int adder_flag = 0;
BinaryGate *adder_gate[GATE_BUFFER_SIZE];
int num_adder_gates = 0;
int num_bits = 0;

// Create a (num_bits)-bit adder capable of adding
// (num_bits)-bit numbers.  Specifically, set it up
// to add the two given numbers.  It is assumed that
// they both fit into (num_bits) bits.  The adder
// built here is a ripple carry adder.  It is a chain
// of full-adders, each being the compasition of two
// half adders: one for the two operands, and one for
// the result of that and a carry-in.  The result is
// of each full adder is a sum and a carry-out.
void CreateAdderCircuits(int num_bits, int A, int B)
{
	if(adder_flag)
		return;
	adder_flag = 1;

	GLfloat x, y;
	int i, j, k = 0;
	int bit;

	::num_bits = num_bits;

	// Barf if more than 32-bits.
	if(num_bits > 32)
		return;

	BinaryGate *in0, *in1, *out;
	BinaryGate *xor0, *xor1, *or;
	BinaryGate *and0, *and1;
	BinaryGate *carry;

	adder_gate[k++] = carry = new StubGate(0.f, 10.f * GLfloat(num_bits / 2) + 5.f, -PI / 4.f, ZERO);

	for(i = 0; i < num_bits; i++)
	{
		y = 10.f * GLfloat(num_bits / 2 - i) - 0.5f;
		bit = (A & 1) ? ONE : ZERO;
		A >>= 1;
		adder_gate[k++] = in0 = new StubGate(-20.f, y - 1.25f, 0.f, bit);
		bit = (B & 1) ? ONE : ZERO;
		B >>= 1;
		adder_gate[k++] = in1 = new StubGate(-15.f, y + 1.25f, 0.f, bit);

		adder_gate[k++] = xor0 = new XorGate(-5.f, y + 1.25f, 0.f);
		adder_gate[k++] = and0 = new AndGate(-5.f, y - 2.5f, -PI / 4.f);

		adder_gate[k++] = xor1 = new XorGate(10.f, y + 1.25f, 0.f);
		adder_gate[k++] = and1 = new AndGate(5.f, y - 2.5f, -PI / 2.f);

		adder_gate[k++] = or = new OrGate(0.f, y - 5.f, -PI / 2.f);

		adder_gate[k++] = out = new StubGate(15.f, y, 0.f, NOTHING);

		or->input0 = and1;
		or->input1 = and0;

		xor0->input0 = in0;
		xor0->input1 = in1;

		and0->input0 = in0;
		and0->input1 = in1;

		xor1->input0 = carry;
		xor1->input1 = xor0;

		and1->input0 = carry;
		and1->input1 = xor0;

		out->input0 = xor1;

		carry = or;
	}

	adder_gate[k++] = carry = new StubGate(0.f, -10.f * GLfloat(num_bits / 2 + num_bits % 2) - 1.f,
														-PI / 4.f, NOTHING);
	carry->input0 = or;

	num_adder_gates = k;
}

void DestroyAdderCircuits(void)
{
	if(!adder_flag)
		return;
	adder_flag = 0;

	int k;
	for(k = 0; k < num_adder_gates; k++)
	{
		delete adder_gate[k];
		adder_gate[k] = 0;
	}
	num_adder_gates = 0;
}

// Put the sum into the first summand.
// This function is only meant to be
// used while the circuit is running.
void UseResult(void)
{
	if(!adder_flag)
		return;

	StubGate *s0, *s1;

	int k;
	for(k = 0; k < num_bits; k++)
	{
		s0 = (StubGate *)adder_gate[k*8 + 1];
		s1 = (StubGate *)adder_gate[k*8 + 8];
		s0->bit = s1->output;
		s1->output = NOTHING;
	}
}

void DrawAdderCircuits(void)
{
	if(!adder_flag)
		return;

	if(adder_gate)
		DrawCircuit(adder_gate, num_adder_gates);
}

void IterateAdderComponents(void)
{
	if(!adder_flag)
		return;

	if(adder_gate)
		IterateCircuit(adder_gate, num_adder_gates);
}

void TweakAdderGate(int mouse_x, int mouse_y)
{
	if(!adder_flag)
		return;

	BinaryGate *gate = SelectGate(adder_gate, num_adder_gates, mouse_x, mouse_y);
	if(gate)
		gate->Tweak();
}

void WipeAdderCircuits(void)
{
	if(!adder_flag)
		return;

	if(adder_gate)
		WipeCircuit(adder_gate, num_adder_gates);
}

// endof Adder.cpp