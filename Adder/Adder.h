// Adder.h

// This module does all the work of creating
// and running the circuits made up of connected digital
// components that together impliment an adder.
// Not much error checking is performed.  One and only one
// adder circuit can be instantiated at any given time.

#ifndef __Adder_h__
#define __Adder_h__

#include "BinaryGate.h"

void CreateAdderCircuits(int num_bits, int A, int B);
void DestroyAdderCircuits(void);
void DrawAdderCircuits(void);
void IterateAdderComponents(void);
void UseResult(void);
void TweakAdderGate(int mouse_x, int mouse_y);
void WipeAdderCircuits(void);

#define GATE_BUFFER_SIZE		1024

extern BinaryGate *adder_gate[GATE_BUFFER_SIZE];
extern int num_adder_gates;

#endif __Adder_h__

// endof Adder.h