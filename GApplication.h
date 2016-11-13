#pragma once

#include <Ogre.h>

using namespace Ogre;

class GFrameListener;
class GApplication
{
public:
	GApplication();
	virtual ~GApplication();

protected:
	Root*				m_pRoot;
	Camera*				m_pCamera;
	SceneManager*		m_pSceneManager;
	GFrameListener*		m_pFrameListener;
	RenderWindow*		m_pRenderWindow;
	String				m_pResourcePath;
	bool				m_bStatsOn;
	Overlay*			m_DebugOverlay;

	SceneNode*			m_pModelNode;

public:
	virtual bool Setup( HWND hWnd );	

	virtual bool Configure( HWND hWnd );

	virtual void ChooseSceneManager();

	virtual void CreateCamera();

	virtual void CreateFrameListener(void);

	virtual void CreateScene( HWND hWnd ) = 0;

	virtual void CreateOverlay();

	virtual void ShowDebugOverlay( bool bShow );

	virtual void DestroyScene();

	virtual void CreateViewports();

	virtual void SetupResources();

	virtual void CreateResourceListener();

	virtual void LoadResources();

	virtual void Render();

	virtual void Update( const FrameEvent& evt );

	virtual void UpdateStats();

	virtual void UpdateCamera( const FrameEvent& evt );

	virtual bool WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	virtual void PostDestorySystem();
};
