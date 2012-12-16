///////////////////////////////////////////////////////////////////////////////
//  This file is built using code ripped from rtvsD3dApp.h and rtvsD3dApp.h, by
//  Gareth Edwards, as well as code written my me, Willem van der Merwe.

#ifndef _rtvs_
#define _rtvs_

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

///////////////////////////////////////
struct Vertex {
  float x, y, z;
};

struct Config {
  byte generation;
  float angle;
  std::string axiom;
  std::map<char, std::string> rules;
};

///////////////////////////////////////
class RTVS {

public:
    
  LPDIRECT3DVERTEXBUFFER9	pVertexBuffer;
	D3DMATERIAL9 lineMtrl;
	DWORD currentKeyClicked;
	DWORD	previousKeyClicked;
	float	fSpinX;
	float	fSpinY;

  Vertex start_point;
  float orientation;
  float node_length;
  int line_count;
  std::string realised;
  std::string cfg_loaded;
  Config cfg;

  std::vector<Vertex> vertices;

  RTVS(){
    //initialize
    ZeroMemory( &pVertexBuffer, sizeof(pVertexBuffer) );
    ZeroMemory( &lineMtrl, sizeof(lineMtrl) );
    ZeroMemory( &fSpinX, sizeof(fSpinX) );
    ZeroMemory( &fSpinY, sizeof(fSpinY) );

    previousKeyClicked = 1;
    currentKeyClicked = 1;

    readCfg("cfgs/1.txt");

    start_point.x = 0;
    start_point.y = 0;
    start_point.z = 0;
    
    orientation = 0;
    node_length = 1;
    line_count = 0;
  };

  bool setup() {
    // setup a material for the lines
	  lineMtrl.Emissive.r = 1.0f;
	  lineMtrl.Emissive.g = 1.0f;
	  lineMtrl.Emissive.b = 1.0f;

    return true;
  }

  bool readCfg(char * file) {
    std::ifstream infilestream;
    std::string line;
    size_t found;

    cfg.angle = 0;
    cfg.axiom = "f";
    cfg.rules.clear();
    cfg.generation = 1;

    infilestream.open(file);
    while(infilestream)
    {
      //read line
      std::getline(infilestream, line);
      //parse line
      if(line[0] == 'n') {
        cfg.generation = atoi(line.substr(2, line.length() - 2).c_str());
      } else if(line[0] == 'd') {
        cfg.angle = atof(line.substr(2, line.length() - 2).c_str());
      } else {
        found = line.find("->");
        if(found != std::string::npos) {
          cfg.rules[line[0]] = line.substr(found + 2, line.length() - found - 2);
        } else {
          cfg.axiom = line;
        }
      }
    }
    infilestream.close();

    return(0);
  }

  std::string produce(const std::string &input, const char * production_rule, char replace) {
    std::string output = "";
    for (unsigned int i = 0; i < input.length(); i++) {
      if(input[i] == replace) {
        output += production_rule;
      } else {
        output += input.at(i);
      }
    }
    return output;
  }
 
  void generateTree() {
    line_count = 0;
    vertices.clear();
    // create iterator for rules in cfg and loop over to produce
    std::map<char,std::string> rules = cfg.rules;
    std::map<char,std::string>::iterator it;
    realised = cfg.axiom;

    for(int i = 1; i < cfg.generation; i++) {
      for ( it=rules.begin(); it != rules.end(); it++ ) {
        realised = produce(realised, cfg.rules[(*it).first].c_str(), (*it).first);  
      }
    }
    
    generate(&realised[0], realised.length(), start_point, orientation);
  }

  void bufferTree(LPDIRECT3DDEVICE9 pd3dDevice) {
    pd3dDevice->CreateVertexBuffer( vertices.size()*sizeof(Vertex),
                                        D3DUSAGE_WRITEONLY,
                                        D3DFVF_XYZ,
                                        //D3DPOOL_MANAGED, // does not have to be properly Released before calling IDirect3DDevice9::Reset
                                        D3DPOOL_DEFAULT,   // must be Released properly before calling IDirect3DDevice9::Reset
                                        &pVertexBuffer, NULL );

    void* pVertices = NULL;
	  pVertexBuffer->Lock( 0, sizeof(vertices.data()), (void**)&pVertices, 0 );
	  memcpy( pVertices, vertices.data(), vertices.size() * sizeof(Vertex) );
	  pVertexBuffer->Unlock();
  }

  void generate(char base[], int base_length, Vertex origin, float orientation) {
    float gen_orientation = orientation;
    Vertex gen_origin = origin;
    bool branching = false;
    int branch_start, sub_branch;

    for (int i = 0; i < base_length; i++) {
      if(!branching) {
        if(base[i] == 0x66) { // f
          gen_origin = addLine(gen_origin, gen_orientation);
          line_count++;
        } else if((int)base[i] == 43) { // + = 43
          gen_orientation += cfg.angle;
        } else if((int)base[i] == 45) { // - = 45
          gen_orientation -= cfg.angle;
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
            generate(branch, i - branch_start, gen_origin, gen_orientation);
          } else {
            sub_branch--;
          }
        }
      }
    }
  }

  Vertex addLine(Vertex start_point, float angle) {
	  Vertex node = {0, node_length, 0};
	  Vertex new_node = {node.x, node.y, node.z};
	  new_node = rotateOnZ(new_node, angle);
	  Vertex end_point = {start_point.x + new_node.x, start_point.y + new_node.y, start_point.z + new_node.z};
    //add vertices to vertex vector
    vertices.push_back(start_point);
    vertices.push_back(end_point);
	  return end_point;
  }

  Vertex rotateOnZ (Vertex vertex, float angle) {
    Vertex rotated = {0, 0, 0};
    float cosAngle = cosf(angle*(3.14159265f/180));
    float sinAngle = sinf(angle*(3.14159265f/180));
    rotated.x = vertex.x * cosAngle + vertex.y * sinAngle;
    rotated.y -= vertex.x * sinAngle - vertex.y * cosAngle;
    return rotated;
  }

  bool setupDX(LPDIRECT3DDEVICE9 pd3dDevice) {
    // ---- turn lighting ON ----
	  pd3dDevice->SetRenderState( D3DRS_LIGHTING , TRUE);

    // ---- ambient light ----
	  pd3dDevice->SetRenderState( D3DRS_AMBIENT,D3DCOLOR_COLORVALUE( 0.3, 0.3, 0.3, 1.0));

    // set up stuff for l-system
    generateTree();
    bufferTree(pd3dDevice);

    return true;
  }
  
  bool display(LPDIRECT3DDEVICE9 pd3dDevice) {
    // clear backbuffers
    pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
      D3DCOLOR_COLORVALUE(0.35f,0.53f,0.7f,1.0f), 1.0f, 0);

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
    pd3dDevice->SetRenderState(D3DRS_CLIPPING, FALSE);

	  // locate
	  D3DXMatrixTranslation( &matTranslation, 0, -20, 50 );
	  matWorld = matRotation * matTranslation;
	  pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    
	  // IF lines THEN
	  if (lines)
	  {
		  // set draw parameters
		  //pd3dDevice->SetTexture( 0, 0 );
		  pd3dDevice->SetStreamSource( 0, pVertexBuffer, 0, sizeof(Vertex) );
		  pd3dDevice->SetFVF(D3DFVF_XYZ);
		  pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		  pd3dDevice->SetMaterial( &lineMtrl );

		  // update key clicked
		  //updateKeyboard(pd3dDevice); // <= also go modify that .. for hotkeys (it's further down in this file)

	    pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, line_count );
	  }

	  return true;
  }

  bool updateKeyboard(LPDIRECT3DDEVICE9 pd3dDevice) {
    currentKeyClicked = 0;
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
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000f)
      currentKeyClicked = 25;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000f)
      currentKeyClicked = 27;
    else if (GetAsyncKeyState(VK_UP) & 0x8000f)
      currentKeyClicked = 26;
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000f)
      currentKeyClicked = 28;

    if(currentKeyClicked == 0) return true;

    bool reload = false;
    bool regen = false;

    if(currentKeyClicked != previousKeyClicked) {
      char * file;
      previousKeyClicked = currentKeyClicked;
      switch(currentKeyClicked) {
        case 1: file = "cfgs/1.txt"; reload = true; break;
        case 2: file = "cfgs/2.txt"; reload = true; break;
        case 3: file = "cfgs/3.txt"; reload = true; break;
        case 4: file = "cfgs/4.txt"; reload = true; break;
        case 5: file = "cfgs/5.txt"; reload = true; break;
        case 6: file = "cfgs/6.txt"; reload = true; break;
        case 7: file = "cfgs/7.txt"; reload = true; break;
        case 8: file = "cfgs/8.txt"; reload = true; break;
      }

      if(reload) readCfg(file);
    }

    switch(currentKeyClicked) {
      case 25:
          if(cfg.angle > 0.1) {
            cfg.angle -= 0.1;
            regen = true;
          }
        break;
      case 27:
          if(cfg.angle < 359.9) {
            cfg.angle += 0.1;
            regen = true;
          }
        break;
      case 26:
          if(cfg.generation < 10) {
            cfg.generation += 1;
            regen = true;
          }
        break;
      case 28:
          if(cfg.generation >= 2) {
            cfg.generation -= 1;
            regen = true;
          }
        break;
    }
      
    if(reload || regen) {
      generateTree();
      bufferTree(pd3dDevice);
    } 

	  // ok
	  return true;
  }

  void getSpin(float* spinX, float* spinY) {
    *spinX = fSpinX;
	  *spinY = fSpinY;
  }

  void setSpin(float spinX, float spinY) {
    fSpinX = spinX;
	  fSpinY = spinY;
  }

};

#endif //_rtvs_