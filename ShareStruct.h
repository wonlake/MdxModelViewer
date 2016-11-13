#pragma once

#include <Ogre.h>
#include <iostream>

//顶点
struct tagVERTEX
{
	float Position[3];
};
//法线
struct tagVERTEXNORMAL
{
	float Normal[3];
};
//纹理坐标
struct tagTEXCOORD
{
	float UV[2];
};
//面
struct tagFACE
{
	unsigned short VertexIndex[3];
};
//平移
struct tagTRANSLATION
{
	unsigned int dwTime;
	float fTrans[3];
	float fInTan[3];
	float fOutTan[3];
};
//旋转
struct tagROTATION
{
	unsigned int dwTime;
	float fRot[4];
	float fInTan[4];
	float fOutTan[4];
};
//缩放
struct tagSCALING
{
	unsigned int dwTime;
	float fScale[3];
	float fInTan[3];
	float fOutTan[3];
};

//骨骼矩阵
struct tagBONEMATRIX
{
	unsigned int objectID;
	unsigned int parentID;
	unsigned int translationType;
	unsigned int rotationType;
	unsigned int scalingType;
	unsigned int translationGlobalSeqID;
	unsigned int rotationGlobalSeqID;
	unsigned int scalingGlobalSeqID;
	float pivotPoint[3];
	std::vector<tagTRANSLATION> translation;
	std::vector<tagROTATION> rotation;
	std::vector<tagSCALING> scaling;
	Ogre::Matrix4 matrix;
};
//时间
struct TIME
{ 
	unsigned int start;
	unsigned int end;
	unsigned int time;
};
//全局序列
struct tagGLOBALSEQ
{
	unsigned int dwDuration;
	unsigned int dwTime;
};
//动画名及时间段
struct tagANIMATION
{
	char  szActionName[80];
	DWORD dwStartTime;
	DWORD dwEndTime;
	float fMoveSpeed;
	DWORD dwFlags;
	float fRarity;
	float fSyncPoint;
	float fRadius;
	float fMin[3];
	float fMax[3];
};
//几何体Alpha及时间
struct tagALPHATIME
{
	unsigned int time;
	float alpha;
	float inTan;
	float outTan;
};
struct tagTEXTUREIDTIME
{
	unsigned int time;
	unsigned int textureID;
	unsigned int inTan;
	unsigned int outTan;
};
struct tagCOLORTIME
{
	unsigned int time;
	float color[3];
	float inTan[3];
	float outTan[3];
};
//几何体动画
struct tagGEOSETANIMATION
{
	unsigned int dwFlags;
	unsigned int dwGeosetID;
	float fAlpha;
	float fColor[3];
	unsigned int alphaType;
	unsigned int colorType;
	unsigned int alphaGlobalSeqID;
	unsigned int colorGlobalSeqID;
	std::vector< tagALPHATIME > alpha;
	std::vector< tagCOLORTIME > color;
};
//纹理动画
struct tagTEXTUREANIMATION
{
	unsigned int translationType;
	unsigned int rotationType;
	unsigned int scalingType;
	unsigned int translationGlobalSeqID;
	unsigned int rotationGlobalSeqID;
	unsigned int scalingGlobalSeqID;
	float fTrans[3];
	float fRot[4];
	float fScaling[3];
	std::vector<tagTRANSLATION> translation;
	std::vector<tagROTATION> rotation;
	std::vector<tagSCALING> scaling;
};
//Pass
struct tagLAYER
{
	unsigned int FilterMode;
	unsigned int ShadingFlags;
	unsigned int TextureID;
	unsigned int TextureAnimationID;
	unsigned int dwCoordID;
	unsigned int dwAlphaType;
	unsigned int dwTextureIDType;
	unsigned int dwAlphaGlobalSeqID;
	unsigned int dwTextureIDGlobalSeqID;
	float		 fAlpha;
	std::vector<tagALPHATIME>	  alpha;
	std::vector<tagTEXTUREIDTIME> textureID;
};

//纹理定义
struct IMAGEINFO
{
	unsigned int  bpp;			   //图像深度
	unsigned int  width;		   //图像宽度
	unsigned int  height;		   //图像高度
	unsigned int  type;            //图像类型(GL_RGB,GL_RGBA)
	unsigned char *imageData;      //图像数据
};