////////////////////////////////////////////////////////////////////////////////




// ---------- rtvsD3dWinLite.h ----------

/*!

\file rtvsD3dWinLite.h
\brief declare D3D tainted API WINdows LITE prototypes and framework
\author Gareth Edwards

*/


#ifndef _rtvsD3dWinLite_
#define _rtvsD3dWinLite_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




// ---------- include application header ----------

#include "../header/rtvsD3dApp.h"




////////////////////////////////////////////////////////////////////////////////




// ---- macros ----

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


// ---- globals ----

HWND                    g_hWnd          = NULL;
LPDIRECT3D9             g_pD3D          = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice    = NULL;
D3DPRESENT_PARAMETERS   g_d3dpp;
bool					g_bHandleWindowResize = true;


// ---- windows ----

int WINAPI			WinMain(
						HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPSTR lpCmdLine,
						int nCmdShow);

LRESULT CALLBACK	WindowProc(
						HWND hWnd,
						UINT msg,
						WPARAM wParam,
						LPARAM lParam);


// ---- framework ----

void		cleanup(void);
HRESULT		cleanupDX(void);
void		display(void);
void		setup(void);
HRESULT		setupDX(void);



// ---- application ----

rtvsD3dApp*	g_app;




////////////////////////////////////////////////////////////////////////////////




#endif // _rtvsD3dWinLite_
