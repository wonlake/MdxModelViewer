#include "StdAfx.h"
#include "OgreWindow.h"
#include "TerrainScene.h"
#include "MdxModel.h"

COgreWindow::COgreWindow(void)
{
	m_pFrameListener	= NULL;
	m_pRoot				= NULL;
	m_pCamera			= NULL;
	m_pRenderWindow		= NULL;
	m_pSceneManager		= NULL;

	m_dwWidth			= 800;
	m_dwHeight			= 600;
}

COgreWindow::~COgreWindow(void)
{
	std::list<CTerrainScene*>::iterator scene_begin = m_vecScenes.begin();
	std::list<CTerrainScene*>::iterator scene_end = m_vecScenes.end();

	while( scene_begin != scene_end )
	{
		delete *scene_begin;
		++scene_begin;
	}
	m_vecScenes.clear();

	std::list<CMdxModel*>::iterator model_begin = m_vecMdxModels.begin();
	std::list<CMdxModel*>::iterator model_end = m_vecMdxModels.end();

	while( model_begin != model_end )
	{
		delete *model_begin;
		++model_begin;
	}
	m_vecMdxModels.clear();

	if( m_pFrameListener )
	{
		delete m_pFrameListener;
	}
	if ( m_pRoot )
		OGRE_DELETE m_pRoot;
}

bool COgreWindow::Setup( HWND hWnd, DWORD dwWidth, DWORD dwHeight )
{
	m_dwWidth  = dwWidth;
	m_dwHeight = dwHeight;

#ifdef _DEBUG
	m_pRoot = OGRE_NEW Root( "Plugins_d.cfg" );
#else
	m_pRoot = OGRE_NEW Root( "Plugins.cfg" );
#endif
	SetupResources();
	Configure( hWnd );
	ChooseSceneManager();
	CreateCamera();
	CreateViewports();

	TextureManager::getSingleton().setDefaultNumMipmaps( 5 );

	CreateResourceListener();
	LoadResources();

	CreateScene();
	CreateFrameListener();

	m_Camera.Init( dwWidth, dwHeight );

	return true;
}

bool COgreWindow::Configure( HWND hWnd )
{
	const RenderSystemList& listRenderSystem = Root::getSingleton().getAvailableRenderers();
	RenderSystemList::const_iterator it = listRenderSystem.begin();
	RenderSystem* m_pRenderSystem = NULL;
	while( it != listRenderSystem.end() )
	{
		if( -1 != (*it)->getName().find("Direct3D9") )
		{
			m_pRenderSystem = *it;
			break;
		}
		++it;
	}
	m_pRenderSystem->setConfigOption( "FSAA", "8" );
	m_pRenderSystem->setConfigOption( "Full Screen", "No" );
	m_pRenderSystem->setConfigOption( "VSync", "No" );
	m_pRenderSystem->setConfigOption( "sRGB Gamma Conversion", "No" );
	char videomode[256];
	sprintf( videomode, "%d x %d @ 32-bit colour", m_dwWidth, m_dwHeight );
	m_pRenderSystem->setConfigOption( "Video Mode", videomode );

	m_pRoot->setRenderSystem( m_pRenderSystem );
	m_pRoot->initialise( false );

	NameValuePairList miscParams;
	miscParams["externalWindowHandle"] = StringConverter::toString( (size_t)hWnd );

	m_pRenderWindow = m_pRoot->createRenderWindow( "MeshViewer", 
		m_dwWidth, m_dwHeight, false, &miscParams );

	return true;
}

void COgreWindow::ChooseSceneManager(void)
{
	m_pSceneManager = m_pRoot->createSceneManager( ST_GENERIC, "MeshViewerInstance" );
}

void COgreWindow::CreateCamera(void)
{
	m_pCamera = m_pSceneManager->createCamera( "PlayerCamera" );

	m_pCamera->setPosition( Vector3(0, 0, 500) );
	m_pCamera->lookAt( Vector3(0, 0, -300) );
	m_pCamera->setNearClipDistance( 5 );
}

void COgreWindow::CreateFrameListener()
{
	m_pFrameListener= new CCustomFrameListener( this, m_pRenderWindow,
		m_pCamera );
	m_pRoot->addFrameListener( m_pFrameListener );
}

void COgreWindow::CreateScene()
{
	m_pSceneManager->setAmbientLight( ColourValue(1.0, 1.0, 1.0) );

	Light* l = m_pSceneManager->createLight("MainLight");
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(-Vector3::UNIT_Z);

	m_pSceneManager->setSkyBox(true, "Examples/CloudyNoonSkyBox" );
	SceneNode* pModelNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();

	pModelNode->setPosition( 0.0f, -1000.0f, -6000.0f );

	CTerrainScene* pTerrainScene = new CTerrainScene();
	pTerrainScene->SetSceneManager(m_pSceneManager);
	pTerrainScene->LoadScene("_pingpan_1.Scene", pModelNode);

	m_vecScenes.push_back(pTerrainScene);

	CMdxModel* pMdxModel = new CMdxModel();
	pMdxModel->SetSceneManager( m_pSceneManager );

	char* filename = "units\\NightElf\\HeroMoonPriestess\\HeroMoonPriestess.mdx";
	if( !pMdxModel->IsExist( filename ) )
	{
		MessageBox( NULL, TEXT("文件不存在"), NULL, MB_OK );
		return;
	}
	pMdxModel->CreateEntityFromFile( filename );
	SceneNode* pMdxModelNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	pMdxModelNode->attachObject( pMdxModel->GetEntity());

	pMdxModelNode->pitch(Radian(-3.14 / 2) - Radian(-3.14 / 20) );
	pMdxModelNode->roll(Radian(-3.14 / 2));
	pMdxModelNode->translate( 0.0f, -1000.0f, -6500.0f );

	m_vecMdxModels.push_back(pMdxModel);

/*	Entity* pEntity = CreateSphereEntity();
	SceneNode* pPlanetNode  = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	pPlanetNode->attachObject( pEntity );	*/	

	//Plane reflectionPlane;
	//Entity* pPlaneEnt;
	//reflectionPlane.normal = Vector3::UNIT_Z;
	//reflectionPlane.d = 100;
	//MeshManager::getSingleton().createPlane("ReflectPlane",
	//	ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
	//	reflectionPlane,
	//	512,512,1,1,true,1,1,1,Vector3::UNIT_Y);
	//pPlaneEnt = m_pSceneManager->createEntity( "plane", "ReflectPlane" );
	//pPlaneEnt->setMaterialName("MyGod");
	//SceneNode* pNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	//pNode->attachObject(pPlaneEnt);
	//pNode->translate( 0, 200, 0 );

	//MaterialPtr pMaterial = MaterialManager::getSingleton().getByName( "MyGod" );
	//GpuProgramParametersSharedPtr pPixelShader = 
	//	pMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	//pPixelShader->setIgnoreMissingParams( true );
	//float TexSize[] = { 512, 512 };
	//pPixelShader->setNamedConstant( "TexSize", TexSize, 2, 1 );

	//BillboardSet* bbset = mSceneMgr->createBillboardSet("BBSet", 1);
	//Billboard* bb = bbset->createBillboard(Vector3(0, 0, 0));
	//
	//FloatRect coords(0.0, 0.0, 1.0, 1.0);
	//bb->setTexcoordRect(coords);
	//bb->setDimensions( 200, 200 );
	//SceneNode* pBBNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	//pBBNode->attachObject( bbset );
	//bbset->setMaterialName( "MyGod" );
	//pBBNode->scale( 2, 2, 2 );		
	//pBBNode->translate( -100, 0, 0 );
}

void COgreWindow::DestroyScene()
{

}

void COgreWindow::CreateViewports()
{
	Viewport* pViewport = m_pRenderWindow->addViewport( m_pCamera );
	pViewport->setBackgroundColour( ColourValue(0, 0, 0) );

	m_pCamera->setAspectRatio(
		Real(pViewport->getActualWidth()) / Real(pViewport->getActualHeight()));
}

void COgreWindow::SetupResources()
{
	ConfigFile cf;
	cf.load("resources.cfg");

	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i = settings->begin();
		for ( ; i != settings->end(); ++i )
		{
			typeName = i->first;
			archName = i->second;

			ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}

void COgreWindow::CreateResourceListener()
{

}

void COgreWindow::LoadResources()
{
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void COgreWindow::Render()
{
	m_pRoot->renderOneFrame();
}

void COgreWindow::Update(const FrameEvent& evt)
{
	AnimationStateIterator it = m_pSceneManager->getAnimationStateIterator();
	while( it.hasMoreElements() )
	{
		it.getNext()->addTime(evt.timeSinceLastFrame);
	}

	std::list<AnimationState*>::iterator it_begin = m_AnimationStates.begin();
	std::list<AnimationState*>::iterator it_end = m_AnimationStates.end();

	while( it_begin != it_end )
	{
		(*it_begin)->addTime(evt.timeSinceLastFrame);
		++it_begin;
	}
	
	std::list<CTerrainScene*>::iterator scene_begin = m_vecScenes.begin();
	std::list<CTerrainScene*>::iterator scene_end = m_vecScenes.end();

	while( scene_begin != scene_end )
	{
		(*scene_begin)->Update(evt);
		++scene_begin;
	}

	std::list<CMdxModel*>::iterator model_begin = m_vecMdxModels.begin();
	std::list<CMdxModel*>::iterator model_end = m_vecMdxModels.end();

	while( model_begin != model_end )
	{
		(*model_begin)->Update(evt);
		++model_begin;
	}
}

bool COgreWindow::HandleMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_KEYDOWN:
		{
			std::list<CMdxModel*>::iterator model_begin = m_vecMdxModels.begin();
			std::list<CMdxModel*>::iterator model_end = m_vecMdxModels.end();

			while( model_begin != model_end )
			{
				switch(wParam)
				{
				case 'C':
					{
						int num = (*model_begin)->GetNumAnimations();
						static int index=0;
						if(index == num)
							index = 0;
						(*model_begin)->SetAnimationByID(index);
						index++;
						break;
					}
				case 'W':
					{
						(*model_begin)->SetAnimationByName("Walk");
						break;
					}
				case 'A':
					{
						(*model_begin)->SetAnimationByName("Attack - 1");
						break;
					}
				case 'S':
					{
						(*model_begin)->SetAnimationByName("Spell");
						break;
					}
				case 'D':
					{
						(*model_begin)->SetAnimationByName("Death");
						break;
					}
				case 'T':
					{
						(*model_begin)->SetAnimationByName("Stand");
						break;
					}
				default:
					return false;
			}
				++model_begin;
			}
			break;
		}
	default:
		return false;
	}
	return true;
}