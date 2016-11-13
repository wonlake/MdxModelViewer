#include "MainApplication.h"

#define SetWindowRect( window, x, y, width, height ) \
{	\
	window->setPosition( \
	CEGUI::UVector2( CEGUI::UDim(0.0f, x),	\
	CEGUI::UDim(0.0f, y) )	\
	);	\
	window->setSize(	\
	CEGUI::UVector2( CEGUI::UDim(0.0f, width),	\
	CEGUI::UDim(0.0f, height) )	\
	);	\
}

Entity* HeroMoonPriestessEntity;
SceneNode* mApplicationNode;

CMdxModel* pModel = NULL;
BOOL     g_bInit  = FALSE;

std::map<std::string, std::string> mModelMaps;

CEGUI::MouseButton convertMouseButtonToCegui(int buttonID)
{
	switch (buttonID)
	{
	case 0: 
		return CEGUI::LeftButton;
	case 1: 
		return CEGUI::RightButton;
	case 2:	
		return CEGUI::MiddleButton;
	case 3: 
		return CEGUI::X1Button;
	default: 
		return CEGUI::LeftButton;
	}
}

class MyListItem : public CEGUI::ListboxTextItem
{
public:
	MyListItem(const CEGUI::String& text, CEGUI::uint id) : CEGUI::ListboxTextItem(text, id)
	{
		setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
	}
};

MainApplication::MainApplication(void)
{
	pModel = &mModel[0];
}


MainApplication::~MainApplication(void)
{
	CEGUI::OgreRenderer::destroySystem();
}

void MainApplication::SetupResources()
{
	ConfigFile cf;
	cf.load("mdxmodelviewer.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;

			ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}

void MainApplication::CreateScene( HWND hWnd )
{		
	RECT rc = { 0 };
	GetClientRect( hWnd, &rc );
	m_PlayerCamera.Init( rc.right - rc.left, rc.bottom - rc.top );
	m_PlayerCamera.SetWheelUnit( 100.0f );

	m_pSceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	Light* l = m_pSceneManager->createLight("MainLight");
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(-Vector3::UNIT_Y);

	m_pSceneManager->setSkyBox(true, "Examples/CloudyNoonSkyBox" );

	CMdxModel* pM = mModel;
	pM->SetSceneManager( m_pSceneManager );

	mModelMaps["units\\NightElf\\HeroMoonPriestess\\HeroMoonPriestess.mdx"] =
		"HeroMoonPriestess";
	mModelMaps["units\\NightElf\\HeroDemonHunter\\HeroDemonHunter.mdx"] = 
		"HeroDemonHunter";
	mModelMaps["units\\NightElf\\HeroKeeperOfTheGrove\\HeroKeeperOfTheGrove.mdx"] = 
		"HeroKeeperOfTheGrove";

	mModelMaps["units\\Orc\\HeroFarseer\\HeroFarseer.mdx"] = 
		"HeroFarseer";
	mModelMaps["units\\Orc\\HeroBladeMaster\\HeroBladeMaster.mdx"] = 
		"HeroBladeMaster";
	mModelMaps["units\\Orc\\HeroTaurenChieftain\\HeroTaurenChieftain.mdx"] = 
		"HeroTaurenChieftain";
	mModelMaps["units\\Orc\\Grunt\\Grunt.mdx"] = 
		"Grunt";

	mModelMaps["units\\Human\\HeroArchMage\\HeroArchMage.mdx"] = 
		"HeroArchMage";
	mModelMaps["units\\Human\\HeroPaladin\\HeroPaladin.mdx"] = 
		"HeroPaladin";
	mModelMaps["units\\Human\\HeroMountainKing\\HeroMountainKing.mdx"] = 
		"HeroMountainKing";
	mModelMaps["units\\Human\\Jaina\\Jaina.mdx"] = 
		"Jaina";

	mModelMaps["units\\Undead\\HeroDeathKnight\\HeroDeathKnight.mdx"] = 
		"HeroDeathKnight";		
	mModelMaps["units\\Undead\\HeroLich\\HeroLich.mdx"] = 
		"HeroLich";		
	mModelMaps["units\\Undead\\HeroDreadLord\\HeroDreadLord.mdx"] =
		"HeroDreadLord";
	mModelMaps["units\\Undead\\Ghoul\\Ghoul.mdx"] = "Ghoul";

	mApplicationNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();

	mApplicationNode->scale( 2, 2, 2 );
	mApplicationNode->pitch(Radian(-3.14 / 2) - Radian(-3.14 / 20) );
	mApplicationNode->roll(Radian(-3.14 / 2));

	for( int i = 1; i < NUM_MODEL; i++ )
	{
		SceneNode* HeroMoonPriestessNode =
			m_pSceneManager->getRootSceneNode()->createChildSceneNode();

		HeroMoonPriestessNode->attachObject( mModel[i].GetEntity());
		HeroMoonPriestessNode->scale( 2, 2, 2 );
		HeroMoonPriestessNode->pitch(Radian(-3.14 / 2));
		HeroMoonPriestessNode->roll(Radian(-3.14 / 2));
		HeroMoonPriestessNode->translate( 200 * (i%2), 300 * (i/2) - 50, -100 );
	}

	CreateGUI();
}

void MainApplication::CreateGUI()
{
	CEGUI::OgreRenderer& mGUIRender = 
		CEGUI::OgreRenderer::bootstrapSystem(
		*(m_pRoot->getRenderTarget("Direct3D App")) );

	CEGUI::System& mGUISystem = CEGUI::System::getSingleton();

	CEGUI::SchemeManager::getSingleton().create(
		(CEGUI::utf8*)"TaharezLookSkin.scheme");
	mGUISystem.setDefaultMouseCursor(
		(CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	mGUISystem.setDefaultFont((CEGUI::utf8*)"simkai");

	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	CEGUI::Window* sheet = wmgr.createWindow(
		"DefaultGUISheet", "Root" ); 

	mGUISystem.setGUISheet(sheet);

	CEGUI::FrameWindow* frame = 
		(CEGUI::FrameWindow*)wmgr.createWindow(
		"TaharezLook/FrameWindow", "Root/Frame");
	SetWindowRect( frame, 0, 0, 200, 450 );
	frame->setProperty( "Alpha", "0.0" );
	frame->setProperty( "SizingEnabled", "False" );
	frame->setProperty( "TitlebarEnabled", "False" );
	sheet->addChildWindow( frame );

	CEGUI::ImagesetManager::getSingleton().createFromImageFile("MyImage", "72.bmp");
	mImage = wmgr.createWindow( 
		"TaharezLook/StaticImage", "Root/Frame/StaticImage");
	SetWindowRect( mImage, 10, 10, 50, 50 );
	mImage->setProperty( "InheritsAlpha", "False" );
	frame->addChildWindow( mImage );

	mQuitButton = (CEGUI::PushButton*)wmgr.createWindow(
		"TaharezLook/Button", "Root/Frame/QuitButton" );
	SetWindowRect( mQuitButton, 80, 20, 100, 26 );
	mQuitButton->setText( "ChangeModel" );
	mQuitButton->setProperty( "InheritsAlpha", "False" );
	frame->addChildWindow( mQuitButton );

	mEditBox = (CEGUI::MultiLineEditbox*)wmgr.createWindow(
		"TaharezLook/MultiLineEditbox", "Root/Frame/MultiEditbox" );
	SetWindowRect( mEditBox, 5, 80, 155, 100 );
	mEditBox->setProperty( "InheritsAlpha", "False" );
	mEditBox->setProperty( "Alpha", "0.5" );
	frame->addChildWindow( mEditBox );

	mComboBox = (CEGUI::Combobox*)wmgr.createWindow(
		"TaharezLook/Combobox", "Root/Frame/ComboBox" );
	SetWindowRect( mComboBox, 5, 200, 155, 240 );
	mComboBox->setProperty( "InheritsAlpha", "False" );
	mComboBox->setProperty( "Alpha", "1.0" );
	frame->addChildWindow( mComboBox );

	setupEventHandlers();
}

void MainApplication::setupEventHandlers(void)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	mQuitButton = (CEGUI::PushButton*)wmgr.getWindow((CEGUI::utf8*)"Root/Frame/QuitButton");
	mQuitButton->subscribeEvent(
		CEGUI::PushButton::EventClicked, 
		CEGUI::Event::Subscriber(&MainApplication::handleQuit, this));
	mEditBox = (CEGUI::MultiLineEditbox*)wmgr.getWindow((CEGUI::utf8*)"Root/Frame/MultiEditbox");

	CEGUI::ImagesetManager::getSingleton().createFromImageFile("MyImage", "72.bmp");
	mImage = wmgr.getWindow((CEGUI::utf8*)"Root/Frame/StaticImage");
	mImage->setProperty( "Image", "set:MyImage image:full_image");

	mComboBox = (CEGUI::Combobox*)wmgr.getWindow((CEGUI::utf8*)"Root/Frame/ComboBox");
	mComboBox->subscribeEvent(
		CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&MainApplication::handleModelComboChanged, this));
	std::map< std::string, std::string >::iterator it = mModelMaps.begin();
	std::map< std::string, std::string >::iterator it_end = mModelMaps.end();

	int count = 0;
	while( it != it_end )
	{			
		mComboBox->addItem( new MyListItem( it->second.c_str(), count) );
		++it;
		++count;
	}
	if (mComboBox->getItemCount() > 0)
	{
		mComboBox->setItemSelectState((size_t)0, true);

		CEGUI::Editbox* eb;
		eb = (CEGUI::Editbox*)wmgr.getWindow( mComboBox->getName() + "__auto_editbox__");
		eb->setText( mModelMaps.begin()->second.c_str() );
		handleModelComboChanged( CEGUI::WindowEventArgs(mComboBox) );
	}
}

bool MainApplication::handleModelComboChanged( const CEGUI::EventArgs& e )
{
	using namespace CEGUI;

	CEGUI::ListboxItem* item = ((Combobox*)((const WindowEventArgs&)e).window)->getSelectedItem();
	int id = item->getID();
	std::map< std::string, std::string >::iterator it= mModelMaps.begin();
	int i = 0;
	while( i < id )
	{
		++it;
		++i;
	}
	mEditBox->setText( it->first.c_str() );
	changeModel( it->first.c_str() );

	return true;
}

bool MainApplication::handleColourChanged(const CEGUI::EventArgs& e)
{
	return true;
}

bool MainApplication::handleQuit(const CEGUI::EventArgs& e)
{
	const char* text = mEditBox->getText().c_str();
	changeModel( text );

	return true;
}

bool MainApplication::changeModel( const char* text )
{		
	int length = strlen( text );
	char* filename = new char[length + 1];
	memcpy( filename, text, length  );

	for( int i = length - 3; i > 1 ; i-- )
	{
		if( (*(filename + i) == 'm' ||
			*(filename + i) == 'M') &&
			(*(filename + i + 1) == 'd' ||
			*(filename + i + 1) == 'D') &&
			(*(filename + i + 2) == 'x' ||
			*(filename + i + 2) == 'X') )
		{
			*(filename + i + 3) = 0;
			break;
		}
	}

	if( !mModel[0].IsExist( filename ) )
	{
		delete []filename;
		MessageBox( NULL, TEXT("文件不存在"), NULL, MB_OK );
		return false;
	}
	if( mModel[0].GetEntity() != NULL )
		mApplicationNode->detachObject( mModel[0].GetEntity());
	mModel[0].Clear();
	mModel[0].CreateEntityFromFile( filename );
	delete []filename;

	mApplicationNode->attachObject( mModel[0].GetEntity());
	FrameEvent evt = { 0.0f, 0.0f };
	mModel[0].Update( evt );
	return true;
}

bool MainApplication::handleMouseEnters(const CEGUI::EventArgs& e)
{
	return true;
}

bool MainApplication::handleMouseLeaves(const CEGUI::EventArgs& e)
{
	return true;
}

void MainApplication::Update( const FrameEvent& evt )
{
	if( pModel )
		pModel->Update( evt );
}

void MainApplication::UpdateCamera( const FrameEvent& evt )
{
	XMMATRIX matView;
	static XMMATRIX matTrans = XMMatrixTranslation( 0.0f, -200.0f, 800.0f );
	static XMMATRIX matR = XMMatrixScaling( 1.0f, 1.0f, -1.0f );

	m_PlayerCamera.GetViewMatrix( &matView );
	matView = XMMatrixTranspose( matR * matView * matTrans * matR );
	m_pCamera->setCustomViewMatrix( true, *(Ogre::Matrix4*)(&matView) );
}

bool MainApplication::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static BOOL  bShowCursor   = TRUE;
	static DWORD CaptionHeight = 0;
	static POINT ptCursor;
	static RECT  winRect;
	static bool  bInput = false;

	m_PlayerCamera.HandleMessage( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_SIZE:
		CaptionHeight = GetSystemMetrics( SM_CYCAPTION );
		GetWindowRect( hWnd, &winRect );
		winRect.top += CaptionHeight;
		break;

	case WM_MOVE:
		GetWindowRect( hWnd, &winRect );
		winRect.top += CaptionHeight;
		break;

	case WM_CHAR:
		if( bInput )
			Win32ImmHelper::ChnInjectChar((CEGUI::utf32)wParam);
		break;

	case WM_SETFOCUS:
		CEGUI::System::getSingleton().signalRedraw();
		break;

	case WM_MOUSEMOVE:
		{
			POINT pt = { LOWORD(lParam), HIWORD(lParam) };
			CEGUI::System::getSingleton().injectMousePosition( pt.x, pt.y );
			break;
		}

	case WM_LBUTTONDOWN:
		CEGUI::System::getSingleton().injectMouseButtonDown( 
			convertMouseButtonToCegui(0) );
		break;

	case WM_LBUTTONUP:
		CEGUI::System::getSingleton().injectMouseButtonUp( 
			convertMouseButtonToCegui(0) );
		break;

	case WM_KEYDOWN:
		{
			if( wParam == VK_ESCAPE )
			{
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				break;
			}

			if( wParam == VK_NUMLOCK )
			{
				bInput = !bInput;
				break;
			}
			if( bInput )
			{
				Win32ImmHelper::IMEFollow( hWnd );
				UINT vk = (UINT)ImmGetVirtualKey( hWnd );
				if( vk == wParam )
					break;
				CEGUI::System::getSingleton().injectKeyDown( 
					(CEGUI::utf32)Win32ImmHelper::VirtualKeyToScanCode(wParam, lParam) );
				break;
			}
			switch(wParam)
			{
			case 'C':
				{
					int num = pModel->GetNumAnimations();
					static int index=0;
					if(index == num)
						index = 0;
					pModel->SetAnimationByID(index);
					index++;
					break;
				}
			case 'W':
				{
					pModel->SetAnimationByName("Walk");
					break;
				}
			case 'A':
				{
					pModel->SetAnimationByName("Attack - 1");
					break;
				}
			case 'S':
				{
					pModel->SetAnimationByName("Spell");
					break;
				}
			case 'D':
				{
					pModel->SetAnimationByName("Death");
					break;
				}
			case 'T':
				{
					pModel->SetAnimationByName("Stand");
					break;
				}
			}
			break;
		}

	case WM_KEYUP:
		{
			CEGUI::System::getSingleton().injectKeyUp( 
				(CEGUI::utf32)Win32ImmHelper::VirtualKeyToScanCode(wParam, lParam) );
			break;
		}

	default:
		{
			GetCursorPos( &ptCursor );
			if( PtInRect( &winRect, ptCursor) )
			{
				if( bShowCursor )
				{
					ShowCursor(FALSE);
					bShowCursor = FALSE;
				}
			}
			else
			{
				if( !bShowCursor )
				{
					ShowCursor(TRUE);
					bShowCursor = TRUE;
				}

			}
			return false;
		}
	}

	return true;
}