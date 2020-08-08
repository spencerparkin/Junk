// random.cpp

// Programmed by Spencer T. Parkin

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "random.h"

// Seed the random number generator either
// with the given seed, or with the current time.
// Providing a seed is nice when you want to
// reproduce a bug or reproduce some sequence
// of events in the program.
void InitRandom(unsigned int seed)
{
	if(!seed)
		seed = (unsigned)time(0);

	srand(seed);
}

// Yeah, I know.
void DesinitRandom(void)
{
}

// Return a random integer in [min,max].
// Notice that we don't use the modulus
// operator here, because the rand()
// developers don't like that.
int RandomNumber(int min, int max)
{
	// Handle this case quickly.
	if(min == max)
		return min;

	// Randomly linearly interpolate between the min and max.
	float t = Random();
	float fmin = float(min);
	float fmax = float(max);
	float r = fmin + (fmax - fmin) * t;

	// Choose the nearest integer.
	float r_floor = (float)floor(r);
	float r_ceil = (float)ceil(r);
	if(r - r_floor < r_ceil - r)
		r = r_floor;
	else
		r = r_ceil;

	// Clamp this, just in case.
	int ir = int(r);
	if(ir > max)
		ir = max;
	if(ir < min)
		ir = min;

	return(ir);
}

// Return a random real number in [0,1].
float Random(void)
{
	float t = float(rand()) / float(RAND_MAX);

	// Clamp it, just in case.
	if(t < 0.f)
		t = 0.f;
	if(t > 1.f)
		t = 1.f;

	return(t);
}

// endof random.cpp