#include "MathHelper.h"

MathHelper::MathHelper(void)
{
}

MathHelper::~MathHelper(void)
{
}

VOID MathHelper::GetInterpolatedValue( Ogre::Vector4& Vector, int Time,int InterpolationType,
										 tagTRANSLATION *p1,tagTRANSLATION *p2)
{
	float Factor;
	float InverseFactor;
	int TempTime;
	TempTime = Time;

	Factor = static_cast<float>(TempTime - p1->dwTime) / static_cast<float>(p2->dwTime - p1->dwTime);
	InverseFactor = (1.0f - Factor);

	switch(InterpolationType)
	{
	case 0:
		{
			Vector.x= p1->fTrans[0];
			Vector.y= p1->fTrans[1];
			Vector.z= p1->fTrans[2];
			Vector.w=1;
			break;
		}

	case 1:
		{
			Vector.x = (InverseFactor * p1->fTrans[0]) + (Factor * p2->fTrans[0]);
			Vector.y = (InverseFactor * p1->fTrans[1]) + (Factor * p2->fTrans[1]);
			Vector.z = (InverseFactor * p1->fTrans[2]) + (Factor * p2->fTrans[2]);
			break;
		}

	case 2:
		{
			float Factor1;
			float Factor2;
			float Factor3;
			float Factor4;
			float FactorTimesTwo;

			FactorTimesTwo = Factor * Factor;

			Factor1 = FactorTimesTwo * (2.0f * Factor - 3.0f) + 1;
			Factor2 = FactorTimesTwo * (Factor - 2.0f) + Factor;
			Factor3 = FactorTimesTwo * (Factor - 1.0f);
			Factor4 = FactorTimesTwo * (3.0f - 2.0f * Factor);

			Vector.x = (Factor1 * p1->fTrans[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fTrans[0]);
			Vector.y = (Factor1 * p1->fTrans[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fTrans[1]);
			Vector.z = (Factor1 * p1->fTrans[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fTrans[2]);

			break;
		}

	case 3:
		{
			FLOAT Factor1;
			FLOAT Factor2;
			FLOAT Factor3;
			FLOAT Factor4;
			FLOAT FactorTimesTwo;
			FLOAT InverseFactorTimesTwo;

			FactorTimesTwo = Factor * Factor;
			InverseFactorTimesTwo = InverseFactor * InverseFactor;

			Factor1 = InverseFactorTimesTwo * InverseFactor;
			Factor2 = 3.0f * Factor * InverseFactorTimesTwo;
			Factor3 = 3.0f * FactorTimesTwo * InverseFactor;
			Factor4 = FactorTimesTwo * Factor;

			Vector.x = (Factor1 * p1->fTrans[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fTrans[0]);
			Vector.y = (Factor1 * p1->fTrans[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fTrans[1]);
			Vector.z = (Factor1 * p1->fTrans[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fTrans[2]);

			break;
		}
	}
}

VOID MathHelper::GetQuaternionValue( Ogre::Quaternion& Vector, int Time,int InterpolationType,
									   tagROTATION *p1,tagROTATION *p2)
{
	float Factor;
	int TempTime;
	TempTime = Time;

	Factor = static_cast<float>(TempTime- p1->dwTime) / static_cast<float>(p2->dwTime - p1->dwTime);

	switch(InterpolationType)
	{
	case 0:
		{
			Vector.x = p1->fRot[0];
			Vector.y = p1->fRot[1];
			Vector.z = p1->fRot[2];
			Vector.w = p1->fRot[3];
			break;
		}

	case 1:
	case 2:
	case 3:
		{
			Ogre::Quaternion n1,n2;
			n1.x = p1->fRot[0];
			n1.y = p1->fRot[1];
			n1.z = p1->fRot[2];
			n1.w = p1->fRot[3];

			n2.x = p2->fRot[0];
			n2.y = p2->fRot[1];
			n2.z = p2->fRot[2];
			n2.w = p2->fRot[3];

			Vector = Ogre::Quaternion::Slerp( Factor, n1, n2, true );

			break;
		}

	//case 2:
	//case 3:
	//	{
	//		Ogre::Quaternion n1,n2,n1_inTan,n2_outTan;
	//		n1.x=p1->fRot[0];
	//		n1.y=p1->fRot[1];
	//		n1.z=p1->fRot[2];
	//		n1.w=p1->fRot[3];
	//		n1_inTan.x=p1->fInTan[0];
	//		n1_inTan.y=p1->fInTan[1];
	//		n1_inTan.z=p1->fInTan[2];
	//		n1_inTan.w=p1->fInTan[3];

	//		n2.x=p2->fRot[0];
	//		n2.y=p2->fRot[1];
	//		n2.z=p2->fRot[2];
	//		n2.w=p2->fRot[3];
	//		n2_outTan.x=p2->fOutTan[0];
	//		n2_outTan.y=p2->fOutTan[1];
	//		n2_outTan.z=p2->fOutTan[2];
	//		n2_outTan.w=p2->fOutTan[3];

	//		Vector = Ogre::Quaternion::Squad( Factor, n1, n1_inTan, n2, n2_outTan, true);

	//		break;
	//	}
	}
}
VOID MathHelper::GetInterpolatedValue( Ogre::Vector4& Vector, int Time,int InterpolationType,
										 tagSCALING *p1,tagSCALING *p2)
{
	float Factor;
	float InverseFactor;
	int TempTime;
	TempTime = Time;

	Factor = static_cast<float>(TempTime - p1->dwTime) / static_cast<float>(p2->dwTime - p1->dwTime);
	InverseFactor = (1.0f - Factor);

	switch(InterpolationType)
	{
	case 0:
		{
			Vector.x=p1->fScale[0];
			Vector.y=p1->fScale[1];
			Vector.z=p1->fScale[2];
			Vector.w=1;
			break;
		}

	case 1:
		{
			Vector.x = (InverseFactor * p1->fScale[0]) + (Factor * p2->fScale[0]);
			Vector.y = (InverseFactor * p1->fScale[1]) + (Factor * p2->fScale[1]);
			Vector.z = (InverseFactor * p1->fScale[2]) + (Factor * p2->fScale[2]);
			break;
		}

	case 2:
		{
			float Factor1;
			float Factor2;
			float Factor3;
			float Factor4;
			float FactorTimesTwo;

			FactorTimesTwo = Factor * Factor;

			Factor1 = FactorTimesTwo * (2.0f * Factor - 3.0f) + 1;
			Factor2 = FactorTimesTwo * (Factor - 2.0f) + Factor;
			Factor3 = FactorTimesTwo * (Factor - 1.0f);
			Factor4 = FactorTimesTwo * (3.0f - 2.0f * Factor);

			Vector.x = (Factor1 * p1->fScale[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fScale[0]);
			Vector.y = (Factor1 * p1->fScale[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fScale[1]);
			Vector.z = (Factor1 * p1->fScale[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fScale[2]);

			break;
		}

	case 3:
		{
			FLOAT Factor1;
			FLOAT Factor2;
			FLOAT Factor3;
			FLOAT Factor4;
			FLOAT FactorTimesTwo;
			FLOAT InverseFactorTimesTwo;

			FactorTimesTwo = Factor * Factor;
			InverseFactorTimesTwo = InverseFactor * InverseFactor;

			Factor1 = InverseFactorTimesTwo * InverseFactor;
			Factor2 = 3.0f * Factor * InverseFactorTimesTwo;
			Factor3 = 3.0f * FactorTimesTwo * InverseFactor;
			Factor4 = FactorTimesTwo * Factor;

			Vector.x = (Factor1 * p1->fScale[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fScale[0]);
			Vector.y = (Factor1 * p1->fScale[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fScale[1]);
			Vector.z = (Factor1 * p1->fScale[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fScale[2]);

			break;
		}
	}
}

//VOID MathHelper::GetInterpolatedValue(D3DXVECTOR4& Vector, int Time,int InterpolationType,
//										 tagTRANSLATION *p1, tagTRANSLATION *p2)
//{
//	float Factor;
//	float InverseFactor;
//	int TempTime;
//	TempTime = Time;
//
//	Factor = static_cast<float>(TempTime - p1->dwTime) / static_cast<float>(p2->dwTime - p1->dwTime);
//	InverseFactor = (1.0f - Factor);
//
//	switch(InterpolationType)
//	{
//	case 0:
//		{
//			Vector.x= p1->fTrans[0];
//			Vector.y= p1->fTrans[1];
//			Vector.z= p1->fTrans[2];
//			Vector.w=1;
//			break;
//		}
//
//	case 1:
//		{
//			Vector.x = (InverseFactor * p1->fTrans[0]) + (Factor * p2->fTrans[0]);
//			Vector.y = (InverseFactor * p1->fTrans[1]) + (Factor * p2->fTrans[1]);
//			Vector.z = (InverseFactor * p1->fTrans[2]) + (Factor * p2->fTrans[2]);
//			break;
//		}
//
//	case 2:
//		{
//			float Factor1;
//			float Factor2;
//			float Factor3;
//			float Factor4;
//			float FactorTimesTwo;
//
//			FactorTimesTwo = Factor * Factor;
//
//			Factor1 = FactorTimesTwo * (2.0f * Factor - 3.0f) + 1;
//			Factor2 = FactorTimesTwo * (Factor - 2.0f) + Factor;
//			Factor3 = FactorTimesTwo * (Factor - 1.0f);
//			Factor4 = FactorTimesTwo * (3.0f - 2.0f * Factor);
//
//			Vector.x = (Factor1 * p1->fTrans[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fTrans[0]);
//			Vector.y = (Factor1 * p1->fTrans[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fTrans[1]);
//			Vector.z = (Factor1 * p1->fTrans[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fTrans[2]);
//
//			break;
//		}
//
//	case 3:
//		{
//			FLOAT Factor1;
//			FLOAT Factor2;
//			FLOAT Factor3;
//			FLOAT Factor4;
//			FLOAT FactorTimesTwo;
//			FLOAT InverseFactorTimesTwo;
//
//			FactorTimesTwo = Factor * Factor;
//			InverseFactorTimesTwo = InverseFactor * InverseFactor;
//
//			Factor1 = InverseFactorTimesTwo * InverseFactor;
//			Factor2 = 3.0f * Factor * InverseFactorTimesTwo;
//			Factor3 = 3.0f * FactorTimesTwo * InverseFactor;
//			Factor4 = FactorTimesTwo * Factor;
//
//			Vector.x = (Factor1 * p1->fTrans[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fTrans[0]);
//			Vector.y = (Factor1 * p1->fTrans[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fTrans[1]);
//			Vector.z = (Factor1 * p1->fTrans[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fTrans[2]);
//
//			break;
//		}
//	}
//}
//
//VOID MathHelper::GetQuaternionValue(D3DXVECTOR4& Vector, int Time,int InterpolationType,
//									   tagROTATION *p1,tagROTATION *p2)
//{
//	float Factor;
//	int TempTime;
//	TempTime = Time;
//
//	Factor = static_cast<float>(TempTime- p1->dwTime) / static_cast<float>(p2->dwTime - p1->dwTime);
//
//	switch(InterpolationType)
//	{
//	case 0:
//		{
//			Vector.x=p1->fRot[0];
//			Vector.y=p1->fRot[1];
//			Vector.z=p1->fRot[2];
//			Vector.w=p1->fRot[3];
//			break;
//		}
//
//	case 1:
//		{
//			D3DXQUATERNION* Quaternion;
//			D3DXQUATERNION* Quaternion1;
//			D3DXQUATERNION* Quaternion2;
//
//			D3DXVECTOR4 n1,n2;
//			n1.x = p1->fRot[0];
//			n1.y = p1->fRot[1];
//			n1.z = p1->fRot[2];
//			n1.w = p1->fRot[3];
//
//			n2.x = p2->fRot[0];
//			n2.y = p2->fRot[1];
//			n2.z = p2->fRot[2];
//			n2.w = p2->fRot[3];
//
//			Quaternion = reinterpret_cast<D3DXQUATERNION*>(&Vector);
//			Quaternion1 = reinterpret_cast<D3DXQUATERNION*>(&n1);
//			Quaternion2 = reinterpret_cast<D3DXQUATERNION*>(&n2);
//
//			D3DXQuaternionSlerp(Quaternion, Quaternion1, Quaternion2, Factor);
//
//			break;
//		}
//
//	case 2:
//	case 3:
//		{
//			D3DXQUATERNION* Quaternion;
//			D3DXQUATERNION* Quaternion1;
//			D3DXQUATERNION* Quaternion2;
//			D3DXQUATERNION* Quaternion3;
//			D3DXQUATERNION* Quaternion4;
//			D3DXQUATERNION QuaternionA;
//			D3DXQUATERNION QuaternionB;
//			D3DXQUATERNION QuaternionC;
//
//			D3DXVECTOR4 n1,n2,n1_inTan,n2_outTan;
//			n1.x=p1->fRot[0];
//			n1.y=p1->fRot[1];
//			n1.z=p1->fRot[2];
//			n1.w=p1->fRot[3];
//			n1_inTan.x=p1->fInTan[0];
//			n1_inTan.y=p1->fInTan[1];
//			n1_inTan.z=p1->fInTan[2];
//			n1_inTan.w=p1->fInTan[3];
//
//			n2.x=p2->fRot[0];
//			n2.y=p2->fRot[1];
//			n2.z=p2->fRot[2];
//			n2.w=p2->fRot[3];
//			n2_outTan.x=p2->fOutTan[0];
//			n2_outTan.y=p2->fOutTan[1];
//			n2_outTan.z=p2->fOutTan[2];
//			n2_outTan.w=p2->fOutTan[3];
//
//			Quaternion = reinterpret_cast<D3DXQUATERNION*>(&Vector);
//			Quaternion1 = reinterpret_cast<D3DXQUATERNION*>(&n1);
//			Quaternion2 = reinterpret_cast<D3DXQUATERNION*>(&n1_inTan);
//			Quaternion3 = reinterpret_cast<D3DXQUATERNION*>(&n2);
//			Quaternion4 = reinterpret_cast<D3DXQUATERNION*>(&n2_outTan);
//
//			D3DXQuaternionSquadSetup(&QuaternionA, &QuaternionB, &QuaternionC, Quaternion1, Quaternion2, Quaternion3, Quaternion4);
//			D3DXQuaternionSquad(Quaternion, Quaternion1, &QuaternionA, &QuaternionB, &QuaternionC, Factor);
//
//			break;
//		}
//	}
//}
//VOID MathHelper::GetInterpolatedValue(D3DXVECTOR4& Vector, int Time,int InterpolationType,
//										 tagSCALING *p1,tagSCALING *p2)
//{
//	float Factor;
//	float InverseFactor;
//	int TempTime;
//	TempTime = Time;
//
//	Factor = static_cast<float>(TempTime - p1->dwTime) / static_cast<float>(p2->dwTime - p1->dwTime);
//	InverseFactor = (1.0f - Factor);
//
//	switch(InterpolationType)
//	{
//	case 0:
//		{
//			Vector.x=p1->fScale[0];
//			Vector.y=p1->fScale[1];
//			Vector.z=p1->fScale[2];
//			Vector.w=1;
//			break;
//		}
//
//	case 1:
//		{
//			Vector.x = (InverseFactor * p1->fScale[0]) + (Factor * p2->fScale[0]);
//			Vector.y = (InverseFactor * p1->fScale[1]) + (Factor * p2->fScale[1]);
//			Vector.z = (InverseFactor * p1->fScale[2]) + (Factor * p2->fScale[2]);
//			break;
//		}
//
//	case 2:
//		{
//			float Factor1;
//			float Factor2;
//			float Factor3;
//			float Factor4;
//			float FactorTimesTwo;
//
//			FactorTimesTwo = Factor * Factor;
//
//			Factor1 = FactorTimesTwo * (2.0f * Factor - 3.0f) + 1;
//			Factor2 = FactorTimesTwo * (Factor - 2.0f) + Factor;
//			Factor3 = FactorTimesTwo * (Factor - 1.0f);
//			Factor4 = FactorTimesTwo * (3.0f - 2.0f * Factor);
//
//			Vector.x = (Factor1 * p1->fScale[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fScale[0]);
//			Vector.y = (Factor1 * p1->fScale[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fScale[1]);
//			Vector.z = (Factor1 * p1->fScale[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fScale[2]);
//
//			break;
//		}
//
//	case 3:
//		{
//			FLOAT Factor1;
//			FLOAT Factor2;
//			FLOAT Factor3;
//			FLOAT Factor4;
//			FLOAT FactorTimesTwo;
//			FLOAT InverseFactorTimesTwo;
//
//			FactorTimesTwo = Factor * Factor;
//			InverseFactorTimesTwo = InverseFactor * InverseFactor;
//
//			Factor1 = InverseFactorTimesTwo * InverseFactor;
//			Factor2 = 3.0f * Factor * InverseFactorTimesTwo;
//			Factor3 = 3.0f * FactorTimesTwo * InverseFactor;
//			Factor4 = FactorTimesTwo * Factor;
//
//			Vector.x = (Factor1 * p1->fScale[0]) + (Factor2 * p1->fOutTan[0]) + (Factor3 * p2->fInTan[0]) + (Factor4 * p2->fScale[0]);
//			Vector.y = (Factor1 * p1->fScale[1]) + (Factor2 * p1->fOutTan[1]) + (Factor3 * p2->fInTan[1]) + (Factor4 * p2->fScale[1]);
//			Vector.z = (Factor1 * p1->fScale[2]) + (Factor2 * p1->fOutTan[2]) + (Factor3 * p2->fInTan[2]) + (Factor4 * p2->fScale[2]);
//
//			break;
//		}
//	}
//}



