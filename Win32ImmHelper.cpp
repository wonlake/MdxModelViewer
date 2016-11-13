#include "Win32ImmHelper.h"

Win32ImmHelper::Win32ImmHelper(void)
{
}

Win32ImmHelper::~Win32ImmHelper(void)
{
}

bool Win32ImmHelper::getFocusedInputBoxCoord( POINT& point, float& height )
{
	CEGUI::WindowManager::WindowIterator wit = 
		CEGUI::WindowManager::getSingleton().getIterator();
	while( !wit.isAtEnd() )
	{
		const CEGUI::Window* widget = (*wit)->getActiveChild();
		if( widget!= NULL )
		{
			CEGUI::String windowType = widget->getType();
			size_t pos = windowType.find("Editbox");
			if( pos != 0xFFFFFFFF )
			{
				CEGUI::UVector2 winPos = widget->getPosition();
				height = widget->getPixelSize().d_height;
				CEGUI::Vector2 winPos1 = CEGUI::CoordConverter::windowToScreen(
					*widget, winPos );
				point.x = winPos1.d_x;
				point.y = winPos1.d_y;
				return true;
			}
		}
		++wit;
	}
	return false;
}

bool Win32ImmHelper::IMEFollow( HWND hWnd )
{
	if( !ImmIsIME( GetKeyboardLayout( 0 ) ) )
		return false;
	bool result;
	POINT point;
	float height;
	result = getFocusedInputBoxCoord( point, height );
	if( !result )
		return false;
	RECT rect;
	GetClientRect( hWnd, &rect );
	point.x += rect.left;
	point.y += rect.top;
	HIMC hImc = ImmGetContext( hWnd );
	if( hImc == NULL )
		return false;
	COMPOSITIONFORM form;
	ImmGetCompositionWindow( hImc, &form );
	form.ptCurrentPos.x = 200;
	form.ptCurrentPos.y = 0;
	ImmSetCompositionWindow( hImc, &form );
	return true;
}
UINT Win32ImmHelper::VirtualKeyToScanCode(WPARAM wParam, LPARAM lParam)
{
	if(HIWORD(lParam) & 0x0F00)
	{
		UINT scancode = MapVirtualKey(wParam, 0);
		return scancode | 0x80;
	}
	else
	{
		return HIWORD(lParam) & 0x00FF;
	}
}

bool Win32ImmHelper::ChnInjectChar(CEGUI::utf32 code_point)
{
#ifndef UNICODE
	static char s_tempChar[3] = "";
	static wchar_t s_tempWchar[2] = L"";
	static bool s_flag = false;
	unsigned char uch = (unsigned char)code_point;
	if( uch >= 0xA1 )
	{
		if( !s_flag )
		{
			s_tempChar[0] = (char)uch; //第一个字节
			s_flag = true;
			return true;
		}
		else if( uch >= 0xA1 )
		{
			s_tempChar[1] = (char)uch; //第二个字节
			s_flag = false;
			MultiByteToWideChar( 0, 0, s_tempChar, 2, s_tempWchar, 1); //转成宽字节
			s_tempWchar[1] = L'\0';
			CEGUI::utf32 code = (CEGUI::utf32)s_tempWchar[0];
			return CEGUI::System::getSingleton().injectChar( code );
		}
		else
		{
			return CEGUI::System::getSingleton().injectChar(code_point);
		}
	}
	else
	{
		s_flag = false;
		return CEGUI::System::getSingleton().injectChar(code_point);
	}
#else
	return CEGUI::System::getSingleton().injectChar(code_point );
#endif
}