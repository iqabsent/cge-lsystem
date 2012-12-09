////////////////////////////////////////////////////////////////////////////////

// ---------- lSysApp.cpp ----------

/*!

\file lSysApp.cpp
\brief Implementation of the lSysApp class
\author void

*/


// ---------- include ----------

#include "../header/lSysApp.h"


////////////////////////////////////////////////////////////////////////////////


// ---------- constructor ----------

/*!

\brief constructor
\author Gareth Edwards

\param int (id of this instance)


*/
lSysApp::lSysApp (int id) : rtvsD3dApp( id ){
  axiom = "f";
  F = "f[-f][+f]";
  angle = 30;
  start_point.x = 0;
  start_point.y = 0;
  start_point.z = 0;
  generations = 2;
  generation = 0;
  orientation = 0;
  node_length = 2;
  axiom_length = 1; // will have to be clever about these..
  f_length = 9;
  line_count = 0;
}


////////////////////////////////////////////////////////////////////////////////

// ---------- framework : display ----------

/*!

\brief framework : display
\author Gareth Edwards

\param LPDIRECT3DDEVICE9	(device)

\return bool (TRUE if ok)

*/

bool lSysApp::display (LPDIRECT3DDEVICE9 pd3dDevice)
{

	pd3dDeviceCache = pd3dDevice;

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

	  pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, line_count );

//MODIFY END
	}


	// ok
	return true;

}

void lSysApp::generateTree() {
  //create vertices
  line_count = 0;
  generate(axiom, axiom_length, generation, start_point, orientation);
}

void lSysApp::bufferTree(LPDIRECT3DDEVICE9 pd3dDevice) {
  pd3dDevice->CreateVertexBuffer( vertices.size()*sizeof(Vertex),
                                      D3DUSAGE_WRITEONLY,
                                      Vertex::FVF_Flags,
                                      //D3DPOOL_MANAGED, // does not have to be properly Released before calling IDirect3DDevice9::Reset
                                      D3DPOOL_DEFAULT,   // must be Released properly before calling IDirect3DDevice9::Reset
                                      &pVertexBuffer, NULL );

  void* pVertices = NULL;
	pVertexBuffer->Lock( 0, sizeof(vertices.data()), (void**)&pVertices, 0 );
	memcpy( pVertices, vertices.data(), vertices.size() * sizeof(Vertex) );
	pVertexBuffer->Unlock();
}

//void lSysApp::generate(char base[], int base_length, int generation, Vertex origin, float orientation) {
  
//}


//THIS METHOD DOES NOT WORK
void lSysApp::generate(char base[], int base_length, int generation, Vertex origin, float orientation) {
  float gen_orientation = orientation;
  Vertex gen_origin = origin;
  bool branching = false;
  int branch_start, sub_branch;

  int gen_generation = generation + 1;

  for (int i = 0; i < base_length; i++) {
    if(!branching) {
      if(base[i] == 0x66) { // f
        if(generation != generations) {
          generate(F, f_length, gen_generation, gen_origin, gen_orientation);
        } else {
          gen_origin = addLine(gen_origin, gen_orientation);
          line_count++;
        }
      } else if((int)base[i] == 43) { // + = 43
        gen_orientation += angle;
      } else if((int)base[i] == 45) { // - = 45
        gen_orientation -= angle;
      } else if((int)base[i] == 91) { // [ = 91
        branching = true;
        branch_start = i + 1; // store start
        sub_branch = 0;       // keep track of sub-branches
      }
    } else {
      if((int)base[i] == 91) { // [ = 91
        sub_branch++; // another sub-branch starts
      } else if((int)base[i] == 93) { // found a branch end .. ] = 93
        if(!sub_branch) { // we are not in a sub-branch
          branching = false;  // stop branching
          char *branch = &base[branch_start]; // use branch as new base and generate
          generate(branch, i - branch_start, generation, gen_origin, gen_orientation);
        } else {
          sub_branch--;
        }
      }
    }
  }
}
//*/

Vertex lSysApp::addLine(Vertex start_point, float angle) {
	if (pd3dDeviceCache == NULL) return start_point;
	Vertex node = {0, node_length, 0};
	Vertex new_node = {node.x, node.y, node.z};
	new_node = rotateOnZ(new_node, angle);
	Vertex end_point = {start_point.x + new_node.x, start_point.y + new_node.y, start_point.z + new_node.z};
  //add vertices to vertex vector
  vertices.push_back(start_point);
  vertices.push_back(end_point);
	//updateVertexBuffer(start_point, end_point);
	//pd3dDeviceCache->DrawPrimitive( D3DPT_LINELIST, 0, 1 );
	return end_point;
}

Vertex lSysApp::rotateOnZ (Vertex vertex, float angle) {
  Vertex rotated = {0, 0, 0};
  float cosAngle = cosf(angle*(3.14159265f/180));
  float sinAngle = sinf(angle*(3.14159265f/180));
  rotated.x = vertex.x * cosAngle + vertex.y * sinAngle;
  rotated.y -= vertex.x * sinAngle - vertex.y * cosAngle;
  return rotated;
}


// ---------- framework : setup dx ----------

/*!

\brief framework : setup dx
\author Gareth Edwards

\param LPDIRECT3DDEVICE9	(device)

\return bool (TRUE if ok)

*/

bool lSysApp::setupDX (LPDIRECT3DDEVICE9 pd3dDevice)
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


  // generate tree
  generateTree();
  bufferTree(pd3dDevice);

	// ok
	return true;

}