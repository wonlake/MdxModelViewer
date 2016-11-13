//
// Copyright (C) Mei Jun 2011
//

#pragma once

#include <windows.h>
#include <xnamath.h>

class CTrackBallCamera
{
public:
	CTrackBallCamera();
	virtual ~CTrackBallCamera();
private:
	VOID Trackball_Prov( int x, int y, float pos[3] );
public:

	VOID Init( int width, int height );
	VOID GetViewMatrix( XMMATRIX* pViewMatrix );
	VOID OnRotate( int x, int y );
	VOID OnTranslate( int x, int y );
	VOID OnMouseMove( int x, int y );
	VOID OnMouseWheel( int delta );
	VOID BeginRotate( int x, int y );
	VOID EndRotate();
	VOID BeginMove( int x, int y );
	VOID EndMove();

	BOOL HandleMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void Reset();
	void SetWheelUnit( float fUnit );

	void GetProjectMatrix( XMMATRIX* pMatProj );
	void SetNearFarPlane( float fNear, float fFar );
private:
	int				  m_width;
	int				  m_height;

	float			  m_lastPos[3];
	float			  m_curPos[3];

	float			  m_Axis[4];
	float			  m_Angle;

	BOOL			  m_bRotating;
	BOOL			  m_bTranslating;
	BOOL			  m_bUpdating;

	float			  m_X;
	float			  m_Y;
	float			  m_Z;

	float			  m_fWheelTranslateUnit;

	float			  m_matRot[16];
	float			  m_matTrans[16];
	float			  m_matLastRot[16];	
	float			  m_matProject[16];
};