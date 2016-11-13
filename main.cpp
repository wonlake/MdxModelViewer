#include "MainApplication.h"

#pragma comment( lib, "Imm32.lib" )

#ifdef _DEBUG
#pragma comment( lib, "OgreMain_d.lib" )
#pragma comment( lib, "CEGUIOgreRenderer_d.lib" )
#pragma comment( lib, "ceguibase_d.lib" )
#else
#pragma comment( lib, "OgreMain.lib" )
#pragma comment( lib, "CEGUIOgreRenderer.lib" )
#pragma comment( lib, "ceguibase.lib" )
#endif

MainApplication app;

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	app.WndProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_CREATE:
		{
			DWORD dwThread;

			app.Setup( hWnd );

			break;
		}
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	WNDCLASS wc;
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hCursor		 = (HCURSOR)LoadCursor( NULL, IDC_ARROW );
	wc.hIcon		 = (HICON)LoadIcon( hInstance, IDI_APPLICATION );
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpfnWndProc	 = (WNDPROC)WndProc;
	wc.lpszClassName = TEXT("Direct3D App");
	wc.lpszMenuName  = NULL;

	if( !RegisterClass( &wc ) )
	{
		MessageBox( NULL, TEXT("注册窗口失败！"), NULL, MB_OK );
		return NULL;
	}

	HWND hWnd = CreateWindow( TEXT("Direct3D App"), 
		TEXT("魔兽模型查看器"), 
		WS_OVERLAPPEDWINDOW,
		150, 100, 800, 600,
		NULL, NULL, hInstance, NULL );

	if( !hWnd )
	{
		MessageBox( NULL, TEXT("创建窗口失败！"), NULL, MB_OK );
		return NULL;
	}

	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );

	MSG msg;
	ZeroMemory( &msg, sizeof(MSG) );
	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			app.Render();
		}
	}
	return 0;
}

