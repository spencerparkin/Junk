// macros.h

// Programmed by Spencer T. Parkin

#ifndef __macros_h__
#define __macros_h__

#include <math.h>

// Useful macros and numerical
// constants are defined in
// this header file.

#ifndef EPSILON
#define EPSILON		0.0000001
#endif EPSILON

#ifndef PI
#define PI		3.1415926536f
#endif PI

#ifndef PI2
#define PI2		(2. * PI)
#endif

#ifndef MOD_2PI
//#define MOD_2PI(angle)		((angle) >= 2.f * PI ? ((angle) - 2.f * PI) : ( \
//							(angle) < 0.f ? ((angle) + 2.f * PI) : (angle)))
//#define MOD_2PI(angle)		fmod((angle), 2.f * PI)
#define MOD_2PI(angle)		mod_2pi(angle)
inline float mod_2pi(float angle)		// Warning: this may loop indefinitly if 'angle' is messed up.
{
	while(angle >= 2.f * PI)
		angle -= 2.f * PI;
	while(angle < 0.f)
		angle += 2.f * PI;
	return(angle);
}
#endif MOD_2PI

#define DEGS_TO_RADS(degs)		((degs) * (PI / 180.))
#define RADS_TO_DEGS(rads)		((rads) * (180. / PI))

#ifndef SWAP
#define SWAP(type,a,b)	do { type tmp; tmp = a; a = b; b = tmp; } while(0)
#endif SWAP

#define FAST_SWAP(a,b)	do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while(0)

#ifndef MIN
#define MIN(a,b)		((a) < (b) ? (a) : (b))
#endif MIN

#ifndef MAX
#define MAX(a,b)		((a) > (b) ? (a) : (b))
#endif MAX

#ifndef SIGN
#define SIGN(x)		((x) < 0 ? -1 : 1)
#endif SIGN

#ifndef ABS
//#define ABS(x)		((x) * SIGN(x))
#define ABS(x)		((x) < 0 ? -(x) : (x))
#endif ABS

#define CLAMP(x,min,max)	MIN(MAX(x,min),max)

#define MOD(x,m)		mod((x),(m))
inline int mod(int x, int m)
{
	int y = x % m;
	if(y < 0)
		y += m;
	return(y);
}

#endif __macros_h__

// endof macros.h