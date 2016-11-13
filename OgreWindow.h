#pragma once
#include <Ogre.h>
#include "CustomFrameListener.h"
#include "Camera.h"
#include <vector>

using namespace Ogre;

class CTerrainScene;
class CMdxModel;

class COgreWindow
{
public:
	COgreWindow(void);
	~COgreWindow(void);

public:
	CTrackBallCamera		m_Camera;
	CCustomFrameListener*	m_pFrameListener;
	Root*					m_pRoot;
	Camera*					m_pCamera;
	RenderWindow*			m_pRenderWindow;
	String					m_strResourcePath;
	SceneManager*			m_pSceneManager;
	DWORD					m_dwWidth;
	DWORD					m_dwHeight;
	
	std::list<AnimationState*> m_AnimationStates;
	std::list<CTerrainScene*> m_vecScenes;
	std::list<CMdxModel*> m_vecMdxModels;

public:
	bool Setup( HWND hWnd, DWORD dwWidth, DWORD dwHeight );

	bool Configure( HWND hWnd );

	void ChooseSceneManager(void);

	void CreateCamera(void);
	
	void CreateFrameListener();

	void CreateScene();

	void DestroyScene();

	void CreateViewports();
	
	void SetupResources();

	void CreateResourceListener();

	void LoadResources();

	void Render();

	void Update(const FrameEvent& evt);

	bool HandleMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
};

