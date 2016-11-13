#pragma once
#include "GFrameListener.h"
#include "GApplication.h"

#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>
#include <CEGUI/elements/CEGUICombobox.h>
#include <CEGUI/elements/CEGUIComboDropList.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIScrollbar.h>
#include <CEGUI/elements/CEGUIMultiLineEditbox.h>
#include <CEGUI/elements/CEGUIFrameWindow.h>
#include <CEGUI/elements/CEGUIEditbox.h>
#include <CEGUIOgreRenderer/ceguiogrerenderer.h>
#include <CEGUIOgreRenderer/ceguiogreresourceprovider.h>

#include "Win32ImmHelper.h"
#include "MdxHeader.h"
#include "MdxModel.h"
#include "Camera.h"

#define NUM_MODEL 1

class MainApplication : public GApplication
{
public:
	CEGUI::PushButton* mQuitButton;
	CEGUI::MultiLineEditbox* mEditBox;
	CEGUI::Window* mImage;
	CEGUI::Combobox* mComboBox;

	CMdxModel mModel[NUM_MODEL];

	CTrackBallCamera m_PlayerCamera;

public:
	MainApplication(void);
	~MainApplication(void);

	void SetupResources();

	void CreateScene( HWND hWnd );

	void CreateGUI();

	void setupEventHandlers(void);

	bool handleModelComboChanged( const CEGUI::EventArgs& e );

	bool handleColourChanged(const CEGUI::EventArgs& e);

	bool handleQuit(const CEGUI::EventArgs& e);

	bool changeModel( const char* text );

	bool handleMouseEnters(const CEGUI::EventArgs& e);

	bool handleMouseLeaves(const CEGUI::EventArgs& e);

	void Update( const FrameEvent& evt );

	void UpdateCamera( const FrameEvent& evt );

	bool WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

