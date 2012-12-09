////////////////////////////////////////////////////////////////////////////////

// ---------- lSysApp.h ----------
#ifndef _lSysApp_
#define _lSysApp_

// ---------- include ----------
#include <Vector>
#include "../header/rtvsD3dApp.h"

////////////////////////////////////////////////////////////////////////////////

// ---------- lSysApp class interface ----------

class lSysApp : public rtvsD3dApp
{

public:

  char *axiom;
  char *F;
  float angle;
  Vertex start_point;
  int generations;
  int generation;
  float orientation;
  float node_length;
  int axiom_length; // will have to be clever about these..
  int f_length;
  int line_count;
  std::vector<Vertex> vertices;

	// ---- methods ----

	// constructor/destructor
	lSysApp (int);

  // lsystem .. must be before setupDX
  void bufferTree	(LPDIRECT3DDEVICE9);

  // overwrites
  bool display	(LPDIRECT3DDEVICE9);
  bool setupDX	(LPDIRECT3DDEVICE9);

  // lsystem
  //Vertex draw (Vertex, float);
  void generateTree	();
  Vertex addLine (Vertex, float);
  void generate(char*, int, int, Vertex, float);
  Vertex rotateOnZ (Vertex, float);

};

////////////////////////////////////////////////////////////////////////////////

#endif // _lSysApp_




