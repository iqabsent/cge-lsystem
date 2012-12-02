////////////////////////////////////////////////////////////////////////////////




// ---------- rtvsD3dApp.cpp ----------

/*!

\file rtvsD3dApp.cpp
\brief Implementation of the rtvsD3dApp class
\author Gareth Edwards

*/




// ---------- include ----------

#include "../header/rtvsD3dApp.h"




////////////////////////////////////////////////////////////////////////////////




// ---------- constructor ----------

/*!

\brief constructor
\author Gareth Edwards

\param int (id of this instance)


*/

rtvsD3dApp::rtvsD3dApp (int id)
{

	// initialise
    ZeroMemory( this, sizeof(rtvsD3dApp) );

	// store id
	_id = id;

	// key clicked
	currentKeyClicked = 1;

}




////////////////////////////////////////////////////////////////////////////////




// ---------- framework ----------




// ---------- framework : cleanup ----------

/*!

\brief framework : cleanup
\author Gareth Edwards

\return bool (TRUE if ok)

*/

bool rtvsD3dApp::cleanup ()
{

	// ok
	return true;

}




// ---------- framework : cleanup dx ----------

/*!

\brief framework : cleanup dx
\author Gareth Edwards

\param LPDIRECT3DDEVICE9	(device)

\return bool (TRUE if ok)

*/

bool rtvsD3dApp::cleanupDX (LPDIRECT3DDEVICE9 pd3dDevice)
{

     // ---- invalidate the font object ----

    if( pFont != NULL )
    {
        int nNewRefCount = pFont->Release();

        if( nNewRefCount > 0 )
        {
            static char strError[256];
            sprintf_s ( strError, 256,
				"The font object failed to cleanup properly.\n"
                "Release() returned a reference count of %d",
				nNewRefCount );
            MessageBox( NULL, strError, "ERROR", MB_OK | MB_ICONEXCLAMATION );
        }

        pFont = NULL;
    }


    // ---- invalidate the vertex buffer object ----

    if( pVertexBuffer != NULL )
    {
        int nNewRefCount = pVertexBuffer->Release();

        if( nNewRefCount > 0 )
        {
            static char strError[256];
            sprintf_s ( strError, 256,
				"The vertex buffer object failed to cleanup properly.\n"
                "Release() returned a reference count of %d",
				nNewRefCount );
            MessageBox( NULL, strError, "ERROR", MB_OK | MB_ICONEXCLAMATION );
        }
        pVertexBuffer = NULL;
    }


	// ok
	return true;

}




// ---------- framework : display ----------

/*!

\brief framework : display
\author Gareth Edwards

\param LPDIRECT3DDEVICE9	(device)

\return bool (TRUE if ok)

*/

bool rtvsD3dApp::display (LPDIRECT3DDEVICE9 pd3dDevice)
{

 	// clear backbuffers
    pd3dDevice->Clear( 0,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_COLORVALUE(0.35f,0.53f,0.7f,1.0f),
		1.0f,
		0);

   // local matrices
    D3DXMATRIX matView;
    D3DXMATRIX matRotation;
    D3DXMATRIX matTranslation;
    D3DXMATRIX matWorld;


	// display flag
	bool lines = true;


	// view matrix
    D3DXMatrixIdentity( &matView );
    pd3dDevice->SetTransform( D3DTS_VIEW, &matView );


	// rotation matrix
	D3DXMatrixRotationYawPitchRoll	( &matRotation, D3DXToRadian(fSpinX), D3DXToRadian(fSpinY), 0 );


	// set render states
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	// locate
	D3DXMatrixTranslation( &matTranslation, 0, 0, 20 );
	matWorld = matRotation * matTranslation;
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );


	// IF lines THEN
	if (lines)
	{

		// set draw parameters
		pd3dDevice->SetTexture( 0, 0 );
		pd3dDevice->SetStreamSource( 0, pVertexBuffer, 0, sizeof(Vertex) );
		pd3dDevice->SetFVF( Vertex::FVF_Flags );
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		pd3dDevice->SetMaterial( &lineMtrl );

		// update key clicked
		updateKeyboard(); // <= also go modify that .. for hotkeys (it's further down in this file)
//MODIFY START
		// select angle of increment from a look up table
		// where the array index == current key clicked
		float angle[8] = { 60, 30, 20, 15, 10, 5, 2, 1 };
		int angIncr = (int)angle[currentKeyClicked];

		// draw a line rotating around the z axis
		Vertex s, e;
		float rdn = 3.141592f / 180.0f;
		for (int a=0; a<360; a+=angIncr)
		{
			// update start and end vertex
			float ang = float(a)*rdn;
			float cosAng = cos(ang);
			float sinAng = sin(ang);
			s.z = 0;
			s.x = 2  * cosAng;
			s.y = 2  * sinAng;
			e.z = 0;
			e.x = 8 * cosAng;
			e.y = 8 * sinAng;

			// update vertex buffer
			updateVertexBuffer(s, e);
//MODIFY END
			// draw a single line
			pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 1 );
		}
	}



	// ok
	return true;

}




// ---------- framework : setup ----------

/*!

\brief framework : setup
\author Gareth Edwards

\return bool (TRUE if ok)

*/

bool rtvsD3dApp::setup ()
{

    // setup a material for the lines
    ZeroMemory( &lineMtrl, sizeof(D3DMATERIAL9) );
	lineMtrl.Emissive.r = 1.0f;
	lineMtrl.Emissive.g = 1.0f;
	lineMtrl.Emissive.b = 1.0f;

    // setup directional sun light
	ZeroMemory( &sunLight, sizeof(D3DLIGHT9) );
	sunLight.Type = D3DLIGHT_DIRECTIONAL;
    sunLight.Direction = D3DXVECTOR3( -1.0f, -1.0f,  1.0f );
    sunLight.Diffuse.r = 1.0f;
    sunLight.Diffuse.g = 1.0f;
    sunLight.Diffuse.b = 1.0f;
    sunLight.Diffuse.a = 1.0f;
    sunLight.Specular.r = 1.0f;
    sunLight.Specular.g = 1.0f;
    sunLight.Specular.b = 1.0f;
    sunLight.Specular.a = 1.0f;

    // setup directional back light
	ZeroMemory( &backLight, sizeof(D3DLIGHT9) );
	backLight.Type = D3DLIGHT_DIRECTIONAL;
    backLight.Direction = D3DXVECTOR3( 1.0f,  0.0f,  0.0f );
    backLight.Diffuse.r = 0.0f;
    backLight.Diffuse.g = 0.2f;
    backLight.Diffuse.b = 0.5f;
    backLight.Diffuse.a = 1.0f;
    backLight.Specular.r = 0.3f;
    backLight.Specular.g = 0.3f;
    backLight.Specular.b = 0.3f;
    backLight.Specular.a = 1.0f;

	// ok
	return true;

}




// ---------- framework : setup dx ----------

/*!

\brief framework : setup dx
\author Gareth Edwards

\param LPDIRECT3DDEVICE9	(device)

\return bool (TRUE if ok)

*/

bool rtvsD3dApp::setupDX (LPDIRECT3DDEVICE9 pd3dDevice)
{

	// ---- turn lighting ON ----
	pd3dDevice->SetRenderState( D3DRS_LIGHTING , TRUE);


	// ---- ambient light ----
	pd3dDevice->SetRenderState( D3DRS_AMBIENT,D3DCOLOR_COLORVALUE( 0.3, 0.3, 0.3, 1.0));


	// ---- sun light ----
    pd3dDevice->SetLight( 0, &sunLight );
    pd3dDevice->LightEnable( 0, TRUE );


	// ---- back light ----
	pd3dDevice->SetLight( 1, &backLight );
    pd3dDevice->LightEnable( 1, TRUE );


	// ---- FONT ----

	fontCol = D3DCOLOR_COLORVALUE(1,1,1,1);
	D3DXCreateFont(
		pd3dDevice,
		30,								// height in pixels
		0,								// width in pixels (0 for default)
		400,							// thickness, 0-1000 OR FW_THIN (100), FW_NORMAL (400), FW_BOLD (700), FW_HEAVY (900)
		0,								// number of MipMaps to create. 0 creates a full chain - no scaling use 1
		false,							// 0/1 - true/false, do you want Italics
		DEFAULT_CHARSET,				// character Set - (Arabic, Greek, etc)
		OUT_DEFAULT_PRECIS,				// how precisely the output must match the font
		ANTIALIASED_QUALITY,			// ANTIALIASED_QUALITY, DEFAULT_QUALITY, DRAFT_QUALITY, and PROOF_QUALITY
		DEFAULT_PITCH | FF_DONTCARE,	// font pitch 
		"Arial",						// name of the required font or "" for system best match
		&pFont);


	// ---- LINE ----


	// ---- initialise vertex data ----
 	Vertex vertices[] =
	{
		//    x      y      z
		{  0.0f,  0.0f,  0.0f  },
		{  0.0f,  0.0f,  0.0f  },
	};

	// ---- create vertex buffer ----
	int numVertices = sizeof(vertices) / ( sizeof(float) * 3 );
	int numLines = numVertices / 2;
	pd3dDevice->CreateVertexBuffer( numVertices*sizeof(Vertex),
                                      D3DUSAGE_WRITEONLY,
                                      Vertex::FVF_Flags,
                                      //D3DPOOL_MANAGED, // does not have to be properly Released before calling IDirect3DDevice9::Reset
                                      D3DPOOL_DEFAULT,   // must be Released properly before calling IDirect3DDevice9::Reset
                                      &pVertexBuffer, NULL );

	// ---- block copy into vector vertex buffer ----
	void* pVertices = NULL;
	pVertexBuffer->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 );
	memcpy( pVertices, vertices, sizeof(vertices) );
	pVertexBuffer->Unlock();


	// ok
	return true;

}




////////////////////////////////////////////////////////////////////////////////




// ---------- update vertex buffer ----------

/*!

\brief line - update vertex buffer
\author Gareth Edwards

\param 

\return bool (TRUE)

*/

DWORD rtvsD3dApp::updateVertexBuffer(Vertex s, Vertex e)
{

	// ---- update vector vertex buffer ----
	Vertex *pVertices = NULL;
	pVertexBuffer->Lock( 0, sizeof(Vertex) * 6, (void**)&pVertices, 0 );
	pVertices->x = (float)s.x;
	pVertices->y = (float)s.y;
	pVertices->z = (float)s.z;
	pVertices++;
	pVertices->x = (float)e.x;
	pVertices->y = (float)e.y;
	pVertices->z = (float)e.z;
	pVertexBuffer->Unlock();

	// ok
	return 1;
}



// ---------- update keyboard ----------

/*!

\brief update keyboard
\author Gareth Edwards

\param rtvsD3dSdk::stage* (pointer to RTVS Stage)

\return bool (TRUE if key updated)

*/

bool rtvsD3dApp::updateKeyboard ()
{

	// get key clicked
	if(GetAsyncKeyState('1') & 0x8000f)
		currentKeyClicked = 1;
	else if(GetAsyncKeyState('2') & 0x8000f)
		currentKeyClicked = 2;
	else if(GetAsyncKeyState('3') & 0x8000f)
		currentKeyClicked = 3;
	else if(GetAsyncKeyState('4') & 0x8000f)
		currentKeyClicked = 4;
	else if(GetAsyncKeyState('5') & 0x8000f)
		currentKeyClicked = 5;
	else if(GetAsyncKeyState('6') & 0x8000f)
		currentKeyClicked = 6;
	else if(GetAsyncKeyState('7') & 0x8000f)
		currentKeyClicked = 7;
	else if(GetAsyncKeyState('8') & 0x8000f)
		currentKeyClicked = 8;

	// ok
	return true;
}


	
	
////////////////////////////////////////////////////////////////////////////////




// ---------- getSpin ----------

/*!

\brief get x and y spin 
\author Gareth Edwards
\param float* ([out] spin around x axis)
\param float* ([out] spin around y axis)

*/

void rtvsD3dApp::getSpin (float* spinX, float* spinY)
{
	*spinX = fSpinX;
	*spinY = fSpinY;
}




// ---------- setSpin ----------

/*!

\brief set x and y spin 
\author Gareth Edwards
\param float (spin around x axis)
\param float (spin around y axis)

*/

void rtvsD3dApp::setSpin (float spinX, float spinY)
{
	fSpinX = spinX;
	fSpinY = spinY;
}




////////////////////////////////////////////////////////////////////////////////
