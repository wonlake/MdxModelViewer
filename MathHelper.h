#pragma once
#include "ShareStruct.h"
class MathHelper
{
public:
	MathHelper(void);
	~MathHelper(void);

	static VOID GetInterpolatedValue( Ogre::Vector4& Vector, int Time,int InterpolationType,
		tagTRANSLATION *p1,tagTRANSLATION *p2);
	static VOID GetQuaternionValue( Ogre::Quaternion& Vector, int Time,int InterpolationType,
		tagROTATION *p1,tagROTATION *p2);
	static VOID GetInterpolatedValue( Ogre::Vector4& Vector, int Time,int InterpolationType,
		tagSCALING *p1,tagSCALING *p2);
};
