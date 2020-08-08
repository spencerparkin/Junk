// random.h

// Programmed by Spencer T. Parkin

#ifndef __random_h__
#define __random_h__

void InitRandom(unsigned int seed = 0);
void DesinitRandom(void);
int RandomNumber(int min, int max);
float Random(void);

#endif __random_h__

// endof random.h