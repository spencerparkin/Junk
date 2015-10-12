// c3ga_util.h

#ifndef _C3GA_UTIL_H_
#define _C3GA_UTIL_H_

// Unlike the other c3ga* files, this one is not auto-generated!

namespace c3ga
{
	enum EulerAngleOrder
	{
		EULER_XYZ,
		EULER_XZY,
		EULER_YXZ,
		EULER_YZX,
		EULER_ZXY,
		EULER_ZYX,
	};

	vectorE3GA OrthoVector( const c3ga::vectorE3GA& vector );
	rotorE3GA RotorFromAxisAngle( const c3ga::vectorE3GA& unitAxis, double halfAngle );
	vectorE3GA RotorToAxisAngle( const c3ga::rotorE3GA& unitRotor, double& halfAngle );
	rotorE3GA UnitRotorFromEulerAngles( double xAngle, double yAngle, double zAngle, EulerAngleOrder eulerAngleOrder );
	bool UnitRotorToEulerAngles( const c3ga::rotorE3GA& unitRotor, double& xAngle, double& yAngle, double& zAngle, EulerAngleOrder eulerAngleOrder );
	vectorE3GA CrossProduct( const c3ga::vectorE3GA& left, const c3ga::vectorE3GA& right );
	vectorE3GA Lerp( const c3ga::vectorE3GA& pt0, const c3ga::vectorE3GA& pt1, double t );
	vectorE2GA Lerp( const c3ga::vectorE2GA& pt0, const c3ga::vectorE2GA& pt1, double t );
	
}

#endif //_C3GA_UTIL_H_

// c3ga_util.h