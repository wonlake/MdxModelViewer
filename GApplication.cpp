#include "GApplication.h"
#include "GFrameListener.h"

GApplication::GApplication() :
	m_pFrameListener(NULL),
	m_pRoot(NULL),
	m_bStatsOn(true),
	m_DebugOverlay(NULL)
{
}

GApplication::~GApplication()
{
	if( m_pFrameListener )
	{
		delete m_pFrameListener;
	}

	if( m_pRoot )
	{
		OGRE_DELETE m_pRoot;
	}

	PostDestorySystem();
}

void GApplication::PostDestorySystem()
{

}

bool GApplication::Setup( HWND hWnd )
{
#ifdef _DEBUG
	m_pRoot = OGRE_NEW Root("Plugins_d.cfg");
#else
	m_pRoot = OGRE_NEW Root("Plugins.cfg");
#endif

	SetupResources();

	bool bOK = Configure( hWnd );

	if( !bOK )
		return false;

	ChooseSceneManager();

	CreateCamera();
	CreateViewports();
	TextureManager::getSingletonPtr()->setDefaultNumMipmaps(5);

	CreateResourceListener();
	LoadResources();
	CreateScene( hWnd );
	CreateOverlay();
	CreateFrameListener();

	return true;
}	

bool GApplication::Configure( HWND hWnd )
{
	const RenderSystemList& renderSystemList = Root::getSingleton().getAvailableRenderers();
	RenderSystemList::const_iterator it = renderSystemList.begin();
	RenderSystem* mRenderSystem = NULL;
	while( it != renderSystemList.end() )
	{
		if( -1 != (*it)->getName().find("Direct3D9") )
		{
			mRenderSystem = *it;
			break;
		}
		++it;
	}
	//mRenderSystem->setConfigOption( "Anti aliasing", "None" );
	//mRenderSystem->setConfigOption( "Floating-point mode", "Fastest" );
	mRenderSystem->setConfigOption( "Full Screen", "No" );
	//mRenderSystem->setConfigOption( "Rendering Device", "NVIDIA GeForce FX 5200" );
	mRenderSystem->setConfigOption( "VSync", "No" );
	mRenderSystem->setConfigOption( "Video Mode", "800 x 600 @ 32-bit colour" );
	mRenderSystem->setConfigOption( "sRGB Gamma Conversion", "No" );
	m_pRoot->setRenderSystem( mRenderSystem );
	m_pRoot->initialise( false );
	NameValuePairList miscParams;
	miscParams["externalWindowHandle"] = StringConverter::toString( (size_t)hWnd );
	m_pRenderWindow = m_pRoot->createRenderWindow( "Direct3D App", 800, 600, false, &miscParams );
	return true;
}

void GApplication::ChooseSceneManager()
{
	m_pSceneManager = m_pRoot->createSceneManager( 0xFFFF, "GameSceneManager" );
}

void GApplication::CreateCamera()
{
	m_pCamera = m_pSceneManager->createCamera( "GameCamera" );
	m_pCamera->setPosition( Vector3(0, 0, 500) );
	m_pCamera->lookAt( Vector3(0, 0, -100) );
	m_pCamera->setNearClipDistance( 5 );
}

void GApplication::CreateFrameListener(void)
{
	m_pFrameListener = new GFrameListener( m_pRenderWindow, m_pCamera, this );
	m_pRoot->addFrameListener( m_pFrameListener );
}

void GApplication::DestroyScene()
{
}

void GApplication::CreateViewports()
{
	Viewport* pViewport = m_pRenderWindow->addViewport(m_pCamera);
	pViewport->setBackgroundColour( ColourValue(0,0,0) );

	m_pCamera->setAspectRatio(
		Real(pViewport->getActualWidth()) / Real(pViewport->getActualHeight()));
}

void GApplication::SetupResources()
{
	
}

void GApplication::CreateResourceListener()
{
}

void GApplication::LoadResources()
{
	ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
}

void GApplication::Render()
{
	m_pRoot->renderOneFrame();
}

void GApplication::Update( const FrameEvent& evt )
{
}

void GApplication::UpdateCamera( const FrameEvent& evt )
{
}

bool GApplication::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return false;
}

void GApplication::CreateOverlay()
{
	m_DebugOverlay = OverlayManager::getSingletonPtr()->getByName( "Core/DebugOverlay" );

	ShowDebugOverlay(true);
}

void GApplication::ShowDebugOverlay( bool bShow )
{
	if( m_DebugOverlay )
	{
		if( bShow )
			m_DebugOverlay->show();
		else
			m_DebugOverlay->hide();
	}
}

void GApplication::UpdateStats(void)
{
	static DisplayString strCurrentFPS   = L"当前帧速率:";
	static DisplayString strAverageFPS   = L"平均帧速率:";
	static DisplayString strBestFPS	     = L"最高帧速率:";
	static DisplayString strWorstFPS     = L"最低帧速率:";
	static DisplayString strNumTriangles = L"三角形数:";
	static DisplayString strNumBatches   = L"批次数:";

	OverlayElement* elemAverageFPS = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
	OverlayElement* elemCurrentFPS = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
	OverlayElement* elemBestFPS    = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
	OverlayElement* elemWorstFPS   = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

	const RenderTarget::FrameStats& stats = m_pRenderWindow->getStatistics();
	elemAverageFPS->setCaption( strAverageFPS + StringConverter::toString( stats.avgFPS ) );
	elemCurrentFPS->setCaption( strCurrentFPS + StringConverter::toString( stats.lastFPS) );
	elemBestFPS->setCaption( strBestFPS + StringConverter::toString( stats.bestFPS ) + " " +
		StringConverter::toString( stats.bestFrameTime) + " ms" );
	elemWorstFPS->setCaption( strWorstFPS + StringConverter::toString( stats.worstFPS ) + " " +
		StringConverter::toString( stats.worstFrameTime) + " ms" );

	OverlayElement* elemTriangles = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
	elemTriangles->setCaption( strNumTriangles + StringConverter::toString(stats.triangleCount) );


	OverlayElement* elemBatches = OverlayManager::getSingleton().getOverlayElement("Core/NumBatches");
	elemBatches->setCaption( strNumBatches + StringConverter::toString(stats.batchCount) );

	OverlayElement* elemDebug = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
	elemDebug->setCaption( "" );
}