#pragma once

#include <Ogre.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

using namespace Ogre;

class GApplication;
class GFrameListener: public FrameListener, public WindowEventListener
{
public:
	GFrameListener( RenderWindow* win, Camera* cam, GApplication* pApp );

	virtual ~GFrameListener();

	virtual void windowClosed( RenderWindow* win );
	
	virtual bool frameStarted( const FrameEvent& evt );

	virtual bool frameRenderingQueued( const FrameEvent& evt );	    

	bool frameEnded( const FrameEvent& evt );

public:
	RenderWindow*	m_RenderWindow;
	GApplication*	m_pApp;
};


