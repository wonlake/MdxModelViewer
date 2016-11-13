//
// Copyright (C) Mei Jun 2011
//

#include "Camera.h"

CTrackBallCamera::CTrackBallCamera()
{
	m_width  = 640;
	m_height = 480;

	memset( m_Axis, 0, sizeof(XMVECTOR) );
	m_Axis[1] = 1.0f;
	m_Angle = 0.0f;

	m_bRotating = FALSE;
	m_bTranslating = FALSE;
	m_bUpdating = FALSE;

	m_X = 0.0f;
	m_Y = 0.0f;
	m_Z = 0.0f;

	m_fWheelTranslateUnit = 1.0f;

	static XMMATRIX matIdentity = XMMatrixIdentity();
	memcpy( m_matRot, &matIdentity, sizeof(XMMATRIX) );
	memcpy( m_matTrans, &matIdentity, sizeof(XMMATRIX) );
	memcpy( m_matLastRot, &matIdentity, sizeof(XMMATRIX) );

	SetNearFarPlane( 0.1f, 1000.0f );
}

CTrackBallCamera::~CTrackBallCamera()
{
}

VOID CTrackBallCamera::GetViewMatrix( XMMATRIX* pViewMatrix )
{
	static XMVECTOR vZero = XMVectorZero();
	static XMMATRIX matIdentity = XMMatrixIdentity();

	XMMATRIX matTrans = XMMatrixTranslationFromVector( XMVectorSet(m_X, m_Y, m_Z, 0.0f) );
	memcpy( m_matTrans, &matTrans, sizeof(XMMATRIX) );

	if( m_bRotating )
	{
		XMVECTOR vAxis = vZero;
		memcpy( &vAxis, m_Axis, sizeof(XMVECTOR) );
		XMMATRIX matRot = XMMatrixRotationAxis( vAxis, m_Angle * XM_PI / 180.0f );
		memcpy( m_matRot, &matRot, sizeof(XMMATRIX) );
		XMMATRIX matLastRot = matIdentity;
		memcpy( &matLastRot, m_matLastRot, sizeof(XMMATRIX) );
		*pViewMatrix = matLastRot * matRot * matTrans;
	}
	else
	{
		XMMATRIX matLastRot = matIdentity;
		memcpy( &matLastRot, m_matLastRot, sizeof(XMMATRIX) );
		XMMATRIX matRot = matIdentity;
		memcpy( &matRot, m_matRot, sizeof(XMMATRIX) );

		if( m_bUpdating && !m_bRotating )
		{			
			matLastRot *= matRot;
			memcpy( m_matLastRot, &matLastRot, sizeof(XMMATRIX) );
			m_bUpdating = FALSE;
			m_Angle = 0.0f;
		}

		*pViewMatrix = matLastRot * matTrans;
	}	
}

void CTrackBallCamera::Reset()
{
	memset( m_Axis, 0, sizeof(XMVECTOR) );
	m_Axis[1] = 1.0f;
	m_Angle = 0.0f;

	m_bRotating = FALSE;
	m_bTranslating = FALSE;
	m_bUpdating = FALSE;

	m_X = 0.0f;
	m_Y = 0.0f;
	m_Z = 0.0f;

	static XMMATRIX matIdentity = XMMatrixIdentity();
	memcpy( m_matRot, &matIdentity, sizeof(XMMATRIX) );
	memcpy( m_matTrans, &matIdentity, sizeof(XMMATRIX) );
	memcpy( m_matLastRot, &matIdentity, sizeof(XMMATRIX) );
}

void CTrackBallCamera::SetWheelUnit( float fUnit )
{
	m_fWheelTranslateUnit = fUnit;
}

VOID CTrackBallCamera::Init( int width, int height )
{
	m_width  = width;
	m_height = height;

	m_bRotating = FALSE;
	m_bTranslating = FALSE;
	m_bUpdating = FALSE;

	SetNearFarPlane( 0.1f, 1000.0f );
}

VOID CTrackBallCamera::Trackball_Prov( int x, int y, float pos[3] )
{
	FLOAT distance;
	FLOAT unit;

	pos[0] = ( 2.0f * x - m_width ) / m_width;
	pos[1] = ( m_height - 2.0f * y ) / m_height;

	distance = (FLOAT)sqrt( pos[0] * pos[0] + pos[1] * pos[1] );
	pos[2] = (FLOAT)cos( (XM_PI / 2.0f) * ( (distance < 1.0f ) ? distance : 1.0f ) );

	unit = 1.0f / (FLOAT)sqrt( pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2] );

	pos[0] *= unit;
	pos[1] *= unit;
	pos[2] *= unit;
}

VOID CTrackBallCamera::OnRotate( int x, int y )
{
	if( m_bRotating )
	{
		FLOAT dx, dy, dz;

		y = m_height - y;
		Trackball_Prov( x, y, m_curPos );


		dx = m_curPos[0] - m_lastPos[0];
		dy = m_curPos[1] - m_lastPos[1];
		dz = m_curPos[2] - m_lastPos[2];

		if( sqrt(dx * dx + dy * dy + dz * dz ) < 0.0001f )
		{
			m_Angle = 0.0f;
			return;
		}

		m_Angle = 90.0f * sqrt(dx * dx + dy * dy + dz * dz);
		
		XMVECTOR axis = XMVectorSet( m_lastPos[1] * m_curPos[2] - m_lastPos[2] * m_curPos[1],
			m_lastPos[0] * m_curPos[2] - m_lastPos[2] * m_curPos[0],
			m_lastPos[1] * m_curPos[0] - m_lastPos[0] * m_curPos[1],
			0.0f );
		if( XMVectorGetX(axis) != 0.0f &&
		    XMVectorGetY(axis) != 0.0f &&
		    XMVectorGetZ(axis) != 0.0f )
		    memcpy( m_Axis, &axis, sizeof(XMVECTOR) );
	}
}

VOID CTrackBallCamera::OnTranslate( int x, int y )
{
	if( m_bTranslating )
	{
		m_curPos[0] = x;
		m_curPos[1] = y;

		m_X += ( m_curPos[0] - m_lastPos[0] ) * 8.0f / m_width * m_fWheelTranslateUnit;
		m_Y += ( m_lastPos[1] - m_curPos[1] ) * 8.0f / m_height * m_fWheelTranslateUnit;

		m_lastPos[0] = m_curPos[0];
		m_lastPos[1] = m_curPos[1];
	}		
}

VOID CTrackBallCamera::OnMouseMove( int x, int y )
{
	OnRotate( x, y );
	OnTranslate( x, y );
}

VOID CTrackBallCamera::OnMouseWheel( int delta )
{
	m_Z += delta / 200.0f * m_fWheelTranslateUnit;
}

VOID CTrackBallCamera::BeginRotate( int x, int y )
{		
	m_bRotating = TRUE;
	y = m_height - y;
	Trackball_Prov( x, y, m_lastPos );
	m_bUpdating = TRUE;
}

VOID CTrackBallCamera::EndRotate()
{
	m_bRotating = FALSE;
	m_lastPos[0] = m_curPos[0];
	m_lastPos[1] = m_curPos[1];
	m_lastPos[2] = m_curPos[2];
}

VOID CTrackBallCamera::BeginMove( int x, int y )
{		
	m_bTranslating = TRUE;
	m_lastPos[0] = x;
	m_lastPos[1] = y;
}

VOID CTrackBallCamera::EndMove()
{
	m_bTranslating = FALSE;
}

BOOL CTrackBallCamera::HandleMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_CAPTURECHANGED:
		if( ( HWND )lParam != hWnd )
		{			
			EndRotate();
			EndMove();
			ReleaseCapture();
		}
		return TRUE;

	case WM_LBUTTONDOWN:
		{
			SetCapture( hWnd );
			BeginMove( LOWORD(lParam), HIWORD(lParam) );
			return TRUE;
		}

	case WM_LBUTTONUP:
		{
			EndMove();
			ReleaseCapture();
			return TRUE;
		}

	case WM_RBUTTONDOWN:
		{
			SetCapture( hWnd );				
			BeginRotate( LOWORD(lParam), HIWORD(lParam) );
			return TRUE;
		}

	case WM_RBUTTONUP:
		{
			EndRotate();
			ReleaseCapture();			
			return TRUE;
		}

	case WM_MOUSEMOVE:
		{					
			WORD x = LOWORD( lParam );
			WORD y = HIWORD( lParam );			
			
			if( x > m_width || y > m_height )
			{				
				EndRotate();
				EndMove();
				ReleaseCapture();
			}
			OnMouseMove( x, y );			
			return TRUE;
		}

	case WM_MOUSEWHEEL:
		{
			short Delta = (short)HIWORD( wParam );
			OnMouseWheel( Delta );
			return TRUE;
		}
	}
	return FALSE;
}

void CTrackBallCamera::GetProjectMatrix( XMMATRIX* pMatProj )
{
	*pMatProj = m_matProject;
}

void CTrackBallCamera::SetNearFarPlane( float fNear, float fFar )
{
	XMMATRIX proj = XMMatrixPerspectiveFovLH( XM_PI / 4.0f, 
		(float)m_width / (float)m_height, 
		fNear, fFar );

	memcpy( m_matProject, &proj, sizeof(XMMATRIX) );
}
