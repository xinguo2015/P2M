
#include <string.h>
#include <stdio.h>
#include "glutApp.h"

#ifdef _WIN
#pragma warning(disable:4996)
#endif

GlutApp * GlutApp::g_theActiveApp = 0;

GlutApp* GlutApp::getApp()
{
	return g_theActiveApp;
}

RenderFrame* GlutApp::getRenderFrame() const
{
	return _renderFrame;
}

GlutApp::GlutApp(const char title[], int winWidth, int winHeight, int posX, int posY)
{
	_theAppTitle        = title;
	_theWindowWidth     = winWidth;
	_theWindowHeight    = winHeight;
	_theWindowPositionX = posX;
	_theWindowPositionY = posY;
	_isAnimating        = false;
	_isFullScreen       = false;
	_renderFrame       = NULL;
}

void GlutApp::activate()
{
	g_theActiveApp = this;
}

void GlutApp::setRenderFrame(RenderFrame* framwork)
{
	_renderFrame = framwork;
}

void GlutApp::initRenderFrame (int argc, char * argv[])
{
	getApp()->getRenderFrame()->initGL(argc, argv);
}



const char * GlutApp::getTitle() const
{
	return _theAppTitle.c_str();
}

void GlutApp::setTitle(const char t[])
{
	_theAppTitle = t;
}

void GlutApp::computeFPS ()
{
	static int frameCount = 0;
	static int lastFrameTime = 0;

	frameCount++;
	int currentTime = glutGet(GLenum(GLUT_ELAPSED_TIME));
	if (currentTime - lastFrameTime > 1000)
	{
		const char * t = getApp()->getTitle();
		char * s = new char[strlen(t) + 50];
		sprintf(s, "%s [ FPS: %4.2f ]", t,
			frameCount * 1000.0 / (currentTime - lastFrameTime));
		glutSetWindowTitle(s);
		lastFrameTime = currentTime;
		frameCount = 0;
		delete[] s;
	}
}


//////////////////////////////////////////////////////////////////
// Callback Functions
// These functions are registered with the glut window and called
// when certain events occur.
//

void GlutApp::onDisplay ()
	// pre:  glut window needs to be refreshed
	// post: model is drawn
{
	// clears requested bits (color and depth) in glut window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw entire scene into cleared window
	getApp()->getRenderFrame()->display();
	// check for any errors when rendering
	GLenum errorCode = glGetError();
	if (errorCode == GL_NO_ERROR)
	{
		// double-buffering - swap the back and front buffers
		glFlush();
		glutSwapBuffers();
		computeFPS();
	}
	else
	{
		getApp()->getRenderFrame()->reportError(errorCode);
	}

}

void GlutApp::onIdle ()
	// pre:  glut window is not doing anything else
	// post: scene is updated and re-rendered if necessary
{
	static int oldTime = 0;

	if( getApp()->_isAnimating )
	{
		int currentTime = glutGet(GLenum(GLUT_ELAPSED_TIME));
		// ensures constant non-framerate dependant animation
		// In particular, the frame rate will never exceed
		// 30 fps.  If the machine cannot handle 30 fps,
		// it may be adviseable to adjust this parameter
		// and the size of the animation steps.
		if (currentTime - oldTime > 40)
		{
			// animate the scene
			getApp()->getRenderFrame()->animate();
			oldTime = currentTime;
		}
	}

	// notify window it has to be repainted
	glutPostRedisplay();
}

void GlutApp::onAlphaNumericKeyPress (unsigned char key, int x, int y)
	// pre:  key has been pressed while mouse is within glut window
	// post: scene is updated and re-rendered
{
	// overwrite defaults
	getApp()->getRenderFrame()->alphaNumericKeys(key, x, y);

	// please do not change function of these keys
	switch (key)
	{
		// toggle animation running
	case 'p':
		getApp()->_isAnimating = ! getApp()->_isAnimating;
		break;

		// quit!
	case 'Q':
	case 'q':
	case 27:
		exit(0);
	}

	// notify window that it has to be re-rendered
	glutPostRedisplay();
}

void GlutApp::onSpecialKeyPress (int key, int x, int y)
	// pre:  arrow or function key has been pressed while mouse is within glut window
	// post: scene is updated and re-rendered
{
	// overwrite defaults
	getApp()->getRenderFrame()->specialKeys(key, x, y);

	// please do not change function of these keys
	switch (key)
	{
		// toggle full screen mode
	case GLUT_KEY_F1:
		getApp()->_isFullScreen = ! getApp()->_isFullScreen;
		if ( getApp()->_isFullScreen )
		{
			getApp()->_theWindowPositionX = glutGet(GLenum(GLUT_WINDOW_X));
			getApp()->_theWindowPositionY = glutGet(GLenum(GLUT_WINDOW_Y));
			glutFullScreen();
		}
		else
		{
			glutReshapeWindow(getApp()->_theWindowWidth, getApp()->_theWindowHeight);
			glutPositionWindow(getApp()->_theWindowPositionX, getApp()->_theWindowPositionY);
		}
		break;
	}

	// notify window that it has to be re-rendered
	glutPostRedisplay();
}

void GlutApp::onMouseMotion (int x, int y)
	// pre:  mouse is dragged (i.e., moved while button is pressed) within glut window
	// post: scene is updated and re-rendered
{
	// overwrite defaults
	getApp()->getRenderFrame()->mouseMotion(x, y);
	// notify window that it has to be re-rendered
	glutPostRedisplay();
}

void GlutApp::onPassiveMouseMotion (int x, int y)
	// pre:  mouse has been moved while within glut window
	// post: scene is updated and re-rendered
{
	// overwrite defaults
	getApp()->getRenderFrame()->passiveMouseMotion(x, y);
	// notify window that it has to be re-rendered
	glutPostRedisplay();
}

void GlutApp::onMouseButtonPress (int button, int state, int x, int y)
	// pre:  mouse button has been pressed while within glut window
	// post: scene is updated and re-rendered
{
	// overwrite defaults
	getApp()->getRenderFrame()->mouseButtons(button, state, x, y);
	// notify window that it has to be re-rendered
	glutPostRedisplay();
}

void GlutApp::onReshape (GLsizei width, GLsizei height)
	// pre:  glut window has been resized
	// post: resets cameras location and aspect to match window
{
	// set viewable area to entire window
	glViewport(0, 0, width, height);
	if (! getApp()->_isFullScreen)
	{
		getApp()->_theWindowWidth = width;
		getApp()->_theWindowHeight = height;
	}
	getApp()->getRenderFrame()->reshape(width, height);
	getApp()->getRenderFrame()->setPerspective(GL_RENDER, 0, 0);
}

void GlutApp::onVisible (int state)
	// pre:  glut window has just been iconified or restored
	// post: if window is visible, animate model, otherwise don't bother
{
	if (state == GLUT_VISIBLE)
	{
		// tell glut to animate model again
		glutIdleFunc(onIdle);
	}
	else
	{
		glutIdleFunc(NULL);
	}
}

void GlutApp::initGlut( int argc, char *argv[] )
{
    // initialize glut
    glutInit(&argc, argv);
    // request initial window size and position on the screen
    glutInitWindowSize(_theWindowWidth, _theWindowHeight);
    glutInitWindowPosition(_theWindowPositionX, _theWindowPositionY);
    // request full color with double buffering and depth-based rendering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    // create window whose title is the name of the executable
	glutCreateWindow( _theAppTitle.c_str() );
	//
	// call back functions
	//
    // tell glut how to display model
    glutDisplayFunc(GlutApp::onDisplay);
    // tell glutwhat to do when it would otherwise be idle
    glutIdleFunc(GlutApp::onIdle);
    // tell glut how to respond to changes in window size
    glutReshapeFunc(GlutApp::onReshape);
    // tell glut how to handle changes in window visibility
    glutVisibilityFunc(GlutApp::onVisible);
    // tell glut how to handle key presses
    glutKeyboardFunc(GlutApp::onAlphaNumericKeyPress);
    glutSpecialFunc(GlutApp::onSpecialKeyPress);
    // tell glut how to handle the mouse
    glutMouseFunc(GlutApp::onMouseButtonPress);
    glutMotionFunc(GlutApp::onMouseMotion);
    glutPassiveMotionFunc(GlutApp::onMouseMotion);
}

Perspective::Perspective()
{
	_fovy   = 45;
	_aspect = 1;
	_znear  = 0.01f;
	_zfar   = 100;
}
Perspective::Perspective( GLfloat fovy, GLfloat aspect, GLfloat znear,  GLfloat zfar)
{
	_fovy   = fovy;
	_aspect = aspect;
	_znear  = znear;
	_zfar   = zfar;
}
void Perspective::setProj() const
{
	gluPerspective(_fovy, _aspect, _znear, _zfar);
}

RenderFrame::RenderFrame ()
{
}

void RenderFrame::reshape (GLsizei width, GLsizei height)
{
	_pproj._aspect = width/float(height);
	_trackball.reshape(width, height);
}

void RenderFrame::initGL (int argc, char * argv[])
{
	initScene(argc,argv);

	// store some scene parameters
	Vec3f bbx[2];
	getSceneBBox(bbx[0], bbx[1]);
	_sceneCenter = Vec4f((bbx[0]+bbx[1])*0.5f, 1.0f);
	// global scaling around center
	_sceneScaling = 1.0f;
	// setup model view
	float sceneSize = length(bbx[1]-bbx[0])/2;
	Vec3f eye = Vec3f(_sceneCenter) + sceneSize*2.f*Vec3f(0,0,1);
	_modelview.translation(-eye);
	// adjust near/far plane
	_pproj._znear = sceneSize/10;
	_pproj._zfar = sceneSize*10;
	// setup track ball
	Vec3f coRot ( _modelview*_sceneCenter );
	_trackball.setCenterOfRotation(coRot);

	// seting opengl
	glClearColor(0, 0, 0, 0.0);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
}

void RenderFrame::setPerspective (GLenum mode, int x, int y)
{
	// get info about viewport (x, y, w, h)
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// set camera to view viewport area
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// check for selection
	if (mode == GL_SELECT) {
		// create 5x5 pixel picking region near cursor location
		gluPickMatrix(GLfloat(x), GLfloat(viewport[3] - y), 5.0, 5.0, viewport);
	}
	// view scene in perspective
	_pproj.setProj();
}

void RenderFrame::display ()
{
	// p*[-c]*S[c]*M*[-(c*M)]A[(c*M)]
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); {
		glLoadIdentity();
		_trackball.applyTransform();
		// model view matrix
		glMultMatrixf( (float*)_modelview );
		// scene scaling with center fixed
		glTranslatef(_sceneCenter[0], _sceneCenter[1], _sceneCenter[2]);
		glScalef(_sceneScaling[0], _sceneScaling[1], _sceneScaling[2]);
		glTranslatef(-_sceneCenter[0], -_sceneCenter[1], -_sceneCenter[2]);
		// drawing....
		drawScene();
	} glPopMatrix();
}

void RenderFrame::animate ()
{
}

void RenderFrame::alphaNumericKeys (unsigned char key, int x, int y)
{
  GLint params[2];

  switch (key)
  {
    case 'f':
      glGetIntegerv(GL_SHADE_MODEL, params);
      if (params[0] == GL_FLAT)
        glShadeModel(GL_SMOOTH);
      else
        glShadeModel(GL_FLAT);
      break;

    case 'w':
      glGetIntegerv(GL_POLYGON_MODE, params);
      if (params[0] == GL_FILL)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
	case 's':
		_sceneScaling *= 0.95;
		break;
	case 'S':
		_sceneScaling *= 1.05;
		break;
  }
}

void RenderFrame::specialKeys (int key, int x, int y)
{
}

void RenderFrame::mouseMotion (int x, int y)
{
	_trackball.motion(x, y);
}

void RenderFrame::passiveMouseMotion (int x, int y)
{
}

void RenderFrame::mouseButtons (int button, int state, int x, int y)
{
	_trackball.mouse(
		Manipulator::convertButtonFromGlut(button),
		Manipulator::convertModifierFromGlut(glutGetModifiers()),
		state == GLUT_DOWN, x, y);
}

void RenderFrame::selectObject (GLuint numSelected, GLuint selectInfo[])
{
}

void RenderFrame::reportError (GLenum errorCode)
{
	printf("Error[%d]:\n", errorCode);
}
