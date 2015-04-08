#ifndef _RenderFrameWORK_H_
#define _RenderFrameWORK_H_

#include <string>
#include <GL/glut.h>

#include "Math3D/math.h"
#include "glutManipulators.h"

using namespace xglm;

class RenderFrame;

class GlutApp
{
public:
	std::string   _theAppTitle;
	int           _theWindowWidth;
	int			  _theWindowHeight;
	int           _theWindowPositionX;
	int			  _theWindowPositionY;
	bool          _isAnimating;
	bool          _isFullScreen;
	RenderFrame * _renderFrame;

public:
	GlutApp(const char title[], int winWidth, int winHeight, int posX, int posY);
	void initGlut(int argc, char *argv[]);

	void         activate();
	RenderFrame* getRenderFrame() const;
	void         setRenderFrame(RenderFrame* theFrame);
	void         initRenderFrame(int argc, char * argv[]);
	void         selectObject (int x, int y);
	const char * getTitle() const;
	void         setTitle(const char t[]);
	//
	// call backs
	//
	static void onDisplay ();
	static void onIdle ();
	// pre:  key has been pressed while mouse is within glut window
	// post: scene is updated and re-rendered
	static void onAlphaNumericKeyPress (unsigned char key, int x, int y);
	// pre:  arrow or function key has been pressed while mouse is within glut window
	// post: scene is updated and re-rendered
	static void onSpecialKeyPress (int key, int x, int y);
	// pre:  mouse is dragged (i.e., moved while button is pressed) within glut window
	// post: scene is updated and re-rendered
	static void onMouseMotion (int x, int y);
	// pre:  mouse has been moved while within glut window
	// post: scene is updated and re-rendered
	static void onPassiveMouseMotion (int x, int y);
	// pre:  mouse button has been pressed while within glut window
	// post: scene is updated and re-rendered
	static void onMouseButtonPress (int button, int state, int x, int y);
	// pre:  glut window has been resized
	// post: resets cameras location and aspect to match window
	static void onReshape (GLsizei width, GLsizei height);
	// pre:  glut window has just been iconified or restored
	// post: if window is visible, animate model, otherwise don't bother
	static void onVisible (int state);
	//
	// utilities
	//
	static GlutApp* getApp();
	static void     computeFPS ();

private:
	static GlutApp* g_theActiveApp;

};

// perspective projection
class Perspective
{
public:
	GLfloat _fovy;   // field of view in Y direction
	GLfloat _aspect; // aspect ratio ( fovx / fovy )
	GLfloat _znear;  // near plane
	GLfloat _zfar;   // far plane
public:
	Perspective();
	Perspective( GLfloat fovy, GLfloat aspect, GLfloat znear,  GLfloat zfar);
	void setProj() const;
};

class RenderFrame
{
public:
	// Constructor
	RenderFrame();

	virtual void initScene(int argc, char * argv[]) = 0;
	virtual void getSceneBBox(Vec3f & bbmin, Vec3f& bbmax) = 0;
	virtual void drawScene() = 0;

	// setup perspective projection
	virtual void setPerspective (GLenum mode, int x, int y);
	// Initialize all parameters needed for this program.
	// This includes for example light parameters and texture parameters
	virtual void initGL (int argc, char * argv[]);

	// Display is called whenever the contents of the window need
	// to be redrawn. The contents of the window may need to be redrawn
	// when the window is initially opened, when the window is popped and
	// window damage is exposed, and when glutPostRedisplay() is
	// explicitly called.
	virtual void display ();

	// Animate will be executed if no other events are pending. This is
	// for example a good spot to calculate the frame rate .
	virtual void animate ();

	// AlphaNumericKeys is called when a key that generates an ASCII
	// character is pressed. the 'key' callback parameter is the generated
	// ASCII value. The 'x' and 'y' callback parameters indicate the location
	// of the mouse (in window-relative coordinates) when the key
	// was pressed.
	virtual void alphaNumericKeys (unsigned char key, int x, int y);

	// SpecialKeus is called when a key that DOES NOT generate an ASCII
	// cahracter is pressed. i.e.: F1-F12, Page-up, Page-down etc.
	virtual void specialKeys (int key, int x, int y);

	// MouseMotion is called when the mouse pointer moves within the
	// window while one of the mouse buttons are pressed. The 'x' and 'y'
	// callback parameters indicate the location(in window-relative
	// coordinates) or the mouse when the event occurred.
	virtual void mouseMotion (int x, int y);

	// PassiveMouseMotion is called when the mouse pointer moves within
	// the window and NO mouse button is pressed. The 'x' and 'y'
	// callback parameters indicate the location(in window-relative
	// coordinates) or the mouse when the event occurred.
	virtual void passiveMouseMotion (int x, int y);

	// MouseButton is called when a mouse button is pressed or released.
	// The 'button' callback parameter is GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON,
	// or GLUT_RIGHT_BUTTON. The 'state' callback parameter is either GLUT_UP
	// or GLUT_DOWN, depending on whether the mouse has been released or
	// pressed. The 'x' and 'y' callback parameters indicate the location
	// (in window-relative coordinates) or the mouse when the event occurred.
	virtual void mouseButtons (int button, int state, int x, int y);

	// SelectObject is called when one or more objects have been clicked on.
	// selectInfo format for numSelected objects is:
	//   [num names given object], [zfront], [zback], [name_0, name_1, ...]
	virtual void selectObject (GLuint numSelected, GLuint selectInfo[]);

	// ReportError is called when OpenGL detects an error while drawing
	// the current scene.  The last such error is given as errorCode.
	// Valid error codes are: GL_INVALID_ENUM, GL_INVALID_VALUE,
	// GL_INVALID_OPERATION, GL_STACK_OVERFLOW, GL_STACK_UNDERFLOW,
	// and GL_OUT_OF_MEMORY.
	virtual void reportError (GLenum errorCode);

	// Reshape is called whenever the window is ersized or moved.
	// It should call glViewport(), so that the display is clipped to
	// the new size, and it redefines the projeciton matrix so that the
	// aspect ratio of the projected image matches the viewport,
	// avoiding ratio distortion.
	virtual void reshape(GLsizei width, GLsizei height);

public:
	Perspective _pproj;     // perspective projection
	Vec4f   _sceneCenter;   // c: center of the scene (in world)
	Vec4f   _sceneScaling;  // S: scaling imposed on the scene
	Mat4f   _projection;    // the projection matrix
	Mat4f   _modelview;     // M: model view matrix
	Mat4f   _userRotate;    // A: arcball rotation, rotation center is: c*M
	//
	// The total transformation for a point p on the scene is:
	//
	//           p*[-c]*S[c]*M*[-(c*M)]A[(c*M)]
	//
	//  where [t] denotes a translation by vector t
	//
	//
	Trackball _trackball;
};


#endif
