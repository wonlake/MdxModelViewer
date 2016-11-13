#include "GApplication.h"
#include "GFrameListener.h"

GFrameListener::GFrameListener( RenderWindow* win, 
	Camera* cam, GApplication* pApp ) :
	m_RenderWindow(win),
	m_pApp(pApp)
{
	WindowEventUtilities::addWindowEventListener( m_RenderWindow, this );
}
	
void GFrameListener::windowClosed( RenderWindow* win )
{
}

GFrameListener::~GFrameListener()
{
	WindowEventUtilities::removeWindowEventListener( m_RenderWindow, this );
	windowClosed( m_RenderWindow );
}

bool GFrameListener::frameStarted( const FrameEvent& evt )
{
	if( m_pApp )
	{
		m_pApp->UpdateCamera( evt );
	}

	return true;
}

bool GFrameListener::frameRenderingQueued( const FrameEvent& evt )
{
	if( m_RenderWindow->isClosed() ) 
		return false;

	if( m_pApp )
	{
		m_pApp->Update( evt );
	}

	return true;
}	    

bool GFrameListener::frameEnded( const FrameEvent& evt )
{
	if( m_pApp )
	{
		m_pApp->UpdateStats();
	}
	return true;
}