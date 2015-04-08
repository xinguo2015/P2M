
#include <GL/glut.h>       // includes glu and gl correctly

#include "glutApp.h"
#include "TriMesh.h"
#include "MeshIO.h"
#include "MeshRender.h"

using namespace xglm;

class MyRenderFrame : public RenderFrame
{
public:
	TriMesh      _mesh;
	RenderOption _renderOptions;
public:
	virtual void initScene(int argc, char * argv[])
	{
		if( argc<2 ) {
			printf("File name is not provided\n");
			return;
		}
		if( ! MeshIO<TriMesh>::createFromFile(&_mesh, argv[1]) )
			return;
		// option for showing bounding box
		//_renderOptions.getOption(RenderOption::BBox) = true;
	}

	virtual void getSceneBBox(Vec3f & bbmin, Vec3f& bbmax)
	{
		if( ! _mesh._bbox.IsValid() )
			_mesh.needBBox();
		bbmin = _mesh._bbox._min;
		bbmax = _mesh._bbox._max;
	}

	virtual void drawScene(void)
	{
		glColor4f(1.0, 1.0, 1.0, 1.0);
		MeshRender<TriMesh>::draw( &_mesh, _renderOptions );
	}


};

//////////////////////////////////////////////////////////////////
// Main Function
//
int main (int argc, char *argv[])
{
	GlutApp myApp(argv[0], 600, 600, 100, 100);

	myApp.initGlut(argc, argv);
	myApp.activate();
    myApp.setRenderFrame(new MyRenderFrame);
	myApp.initRenderFrame(argc,argv);
    // give control over to glut
	// to handle rendering and interaction
    glutMainLoop();
    return 0;
}
