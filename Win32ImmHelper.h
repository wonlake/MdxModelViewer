#pragma once

#include <CEGUI/CEGUICoordConverter.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <windows.h>

class Win32ImmHelper
{
public:
	Win32ImmHelper(void);
	~Win32ImmHelper(void);
	static bool getFocusedInputBoxCoord( POINT& point, float& height );
	static bool IMEFollow( HWND hWnd );
	static UINT VirtualKeyToScanCode(WPARAM wParam, LPARAM lParam);
	static bool ChnInjectChar(CEGUI::utf32 code_point);
};
