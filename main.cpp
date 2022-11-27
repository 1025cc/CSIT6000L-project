#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif
// --------------------------------
#include <vecmath.h>
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
///TODO: include more headers if necessary
#include "TimeStepper.hpp"
#include "ClothSystem.h"
#include "Mesh.h"
using namespace std;

// Globals here.
namespace
{

	ClothSystem *system;
	TimeStepper * timeStepper;
	bool move = false;
	Mesh mesh;
	//These are variables to change the light
	//fixed increment/decrement
	float posChange = 0.5f;
	//first value of light position. default:1.0
	float pos1 = 0.0f;
	//second value of light positon. default:1.0
	float pos2 = 7.0f;
	int systemState[] = {0,0,0,0};
	// initialize your particle systems
  ///TODO: read argv here. set timestepper , step size etc
	void initSystem(int argc, char * argv[])
	{

    // seed the random number generator with the current time
    srand( time( NULL ) );
    system = new ClothSystem(20,20,0);
    timeStepper = new RKF45();
  }

	// Take a step forward for the particle shower
	///TODO: Optional. modify this function to display various particle systems
	///and switch between different timeSteppers
	void stepSystem()
	{
		///TODO The stepsize should change according to commandline arguments
		const float h = 0.04f;
		if (timeStepper != 0) {
			timeStepper->takeStep(system, h);
		}
	}

	void drawRoom() {
		GLfloat floorColor[] = { 131.0 / 255.0, 175.0 / 255.0, 155.0 / 255.0, 1.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, floorColor);
		glPushMatrix();
		glTranslatef(0.0f, -15.0f, 0.0f);
		glScaled(30.0f, 0.01f, 30.0f);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0.0f, 15.0f, 0.0f);
		glScaled(30.0f, 0.01f, 30.0f);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef(10.0f, -14.0f, 0.0f);
		glScaled(15.f, 0.01f, 30.0f);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef(-10.0f, -14.0f, 0.0f);
		glScaled(14.3f, 0.01f, 30.0f);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef(0.f, -14.f, 9.5f);
		glScaled(30.f, 0.01f, 14.f);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
		glTranslatef(0.f, -14.f, -10.f);
		glScaled(30.f, 0.01f, 10.f);
		glutSolidCube(1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		glTranslatef(0.0f, -15.0f, 0.0f);
		glScaled(30.0f, 0.01f, 30.0f);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		glTranslatef(0.0f, 15.0f, 0.0f);
		glScaled(30.0f, 0.01f, 30.0f);
		glutSolidCube(1);
		glPopMatrix();
	}
	// Draw the current particle positions
	void drawSystem()
	{

		// Base material colors (they don't change)
		GLfloat particleColor[] = { 0.4f, 0.7f, 1.0f, 1.0f };

		GLfloat windowColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, particleColor);
		system->draw();
		drawRoom();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, windowColor);

		// Define specular color and shininess
		GLfloat specColor[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat shininess[] = { 100.0 };

		// Note that the specular color and shininess can stay constant
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

		glPushMatrix();
		glRotatef(-90, 0, 1, 0);
		glTranslatef(-14, 1.4, 0.17);
		glScaled(0.13, 0.1, 0.13);
		mesh.draw();
		glPopMatrix();



	}



	//-------------------------------------------------------------------


	// This is the camera
	Camera camera;

	// These are state variables for the UI
	bool g_mousePressed = false;

	// Declarations of functions whose implementations occur later.
	void arcballRotation(int endX, int endY);
	void keyboardFunc(unsigned char key, int x, int y);
	void specialFunc(int key, int x, int y);
	void mouseFunc(int button, int state, int x, int y);
	void motionFunc(int x, int y);
	void reshapeFunc(int w, int h);
	void drawScene(void);
	void initRendering();

	// This function is called whenever a "Normal" key press is
	// received.
	void keyboardFunc(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27: // Escape key
			exit(0);
			break;
		case ' ':
		{
			Matrix4f eye = Matrix4f::identity();
			camera.SetRotation(eye);
			camera.SetCenter(Vector3f::ZERO);
			break;
		}
		case 's':
			move = !move;
			cout << "change move" << endl;
			break;
		case 'r':
			systemState[0] = 0;
			system = new ClothSystem(20, 20, systemState);
			cout << "reset" << endl;
			break;
		case 'f':
			systemState[0] = 1;
			system = new ClothSystem(20, 20, systemState);
			cout << "cloth fall" << endl;
			break;
		case 'g':
			systemState[0] = 2;
			system = new ClothSystem(20, 20, systemState);
			cout << "add friction" << endl;
			break;
		case '1':
			systemState[3]++;
			system = new ClothSystem(20, 20, systemState);
			cout << "add z" << endl;
			break;
		case '2':
			systemState[3]--;
			system = new ClothSystem(20, 20, systemState);
			cout << "minus z" << endl;
			break;
		case '3':
			systemState[2]++;
			system = new ClothSystem(20, 20, systemState);
			cout << "add y" << endl;
			break;
		case '4':
			systemState[2]--;
			system = new ClothSystem(20, 20, systemState);
			cout << "minus y" << endl;
			break;
		case '5':
			systemState[1]++;
			system = new ClothSystem(20, 20, systemState);
			cout << "add x" << endl;
			break;
		case '6':
			systemState[1]--;
			system = new ClothSystem(20, 20, systemState);
			cout << "minus x" << endl;
			break;
		default:
			cout << "Unhandled key press " << key << "." << endl;
		}

		glutPostRedisplay();
	}

    // This function is called whenever a "Special" key press is
    // received.  Right now, it's handling the arrow keys.
    void specialFunc( int key, int x, int y )
    {
        switch ( key )
        {
            case GLUT_KEY_UP:
                // add code to change light position
                pos2 += posChange;
                cout << "Press up arrow to move the light up." << endl;
                break;
            case GLUT_KEY_DOWN:
                // add code to change light position
                pos2 -= posChange;
                cout << "Press down arrow to move the light down." << endl;
                break;
            case GLUT_KEY_LEFT:
                // add code to change light position
                pos1 -= posChange;
                cout << "Press left arrow to move the light to the left." << endl;
                break;
            case GLUT_KEY_RIGHT:
                // add code to change light position
                pos1 += posChange;
                cout << "Press right arrow to move the light to the right." << endl;
                break;
        }

        // this will refresh the screen so that the user sees the light position
        glutPostRedisplay();
    }

    //  Called when mouse button is pressed.
    void mouseFunc(int button, int state, int x, int y)
    {
        if (state == GLUT_DOWN)
        {
            g_mousePressed = true;
            
            switch (button)
            {
            case GLUT_LEFT_BUTTON:
                camera.MouseClick(Camera::LEFT, x, y);
                break;
            case GLUT_MIDDLE_BUTTON:
                camera.MouseClick(Camera::MIDDLE, x, y);
                break;
            case GLUT_RIGHT_BUTTON:
                camera.MouseClick(Camera::RIGHT, x,y);
            default:
                break;
            }                       
        }
        else
        {
            camera.MouseRelease(x,y);
            g_mousePressed = false;
        }
        glutPostRedisplay();
    }

    // Called when mouse is moved while button pressed.
    void motionFunc(int x, int y)
    {
        camera.MouseDrag(x,y);        
    
        glutPostRedisplay();
    }

    // Called when the window is resized
    // w, h - width and height of the window in pixels.
    void reshapeFunc(int w, int h)
    {
        camera.SetDimensions(w,h);

        camera.SetViewport(0,0,w,h);
        camera.ApplyViewport();

        // Set up a perspective view, with square aspect ratio
        glMatrixMode(GL_PROJECTION);

        camera.SetPerspective(50);
        glLoadMatrixf( camera.projectionMatrix() );
    }

    // Initialize OpenGL's rendering modes
    void initRendering()
    {
        glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
        glEnable(GL_LIGHTING);     // Enable lighting calculations
        glEnable(GL_LIGHT0);       // Turn on light #0.

        glEnable(GL_NORMALIZE);

        // Setup polygon drawing
        glShadeModel(GL_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

        // Clear to black
        glClearColor(0,0,0,1);
    }

    // This function is responsible for displaying the object.
    void drawScene(void)
    {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode( GL_MODELVIEW );  
        glLoadIdentity();

        // Light color (RGBA)
        GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
        // Light position
        GLfloat Lt0pos[] = {pos1, pos2, -20.f, 1.0f};

        glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
        glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

        glLoadMatrixf( camera.viewMatrix() );
        // THIS IS WHERE THE DRAW CODE GOES.

        drawSystem();

        // This draws the coordinate axes when you're rotating, to
        // keep yourself oriented.
        if( g_mousePressed )
        {
            glPushMatrix();
            Vector3f eye = camera.GetCenter();
            glTranslatef( eye[0], eye[1], eye[2] );

            // Save current state of OpenGL
            glPushAttrib(GL_ALL_ATTRIB_BITS);

            // This is to draw the axes when the mouse button is down
            glDisable(GL_LIGHTING);
            glLineWidth(3);
            glPushMatrix();
            glScaled(5.0,5.0,5.0);
            glBegin(GL_LINES);
            glColor4f(1,0.5,0.5,1); glVertex3f(0,0,0); glVertex3f(1,0,0);
            glColor4f(0.5,1,0.5,1); glVertex3f(0,0,0); glVertex3f(0,1,0);
            glColor4f(0.5,0.5,1,1); glVertex3f(0,0,0); glVertex3f(0,0,1);

            glColor4f(0.5,0.5,0.5,1);
            glVertex3f(0,0,0); glVertex3f(-1,0,0);
            glVertex3f(0,0,0); glVertex3f(0,-1,0);
            glVertex3f(0,0,0); glVertex3f(0,0,-1);

            glEnd();
            glPopMatrix();

            glPopAttrib();
            glPopMatrix();
        }
                 
        // Dump the image to the screen.
        glutSwapBuffers();
    }

    void timerFunc(int t)
    {
        if(move){
            system->move();
        }
        stepSystem();

        glutPostRedisplay();

        glutTimerFunc(t, &timerFunc, t);
    }

    

    

}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char* argv[] )
{
    glutInit( &argc, argv );

    // We're going to animate it, so double buffer 
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 800, 800 );
    
    camera.SetDimensions( 600, 600 );

    camera.SetDistance( 30 );
    camera.SetCenter( Vector3f::ZERO );
    
    glutCreateWindow("Assignment 4");

    // Initialize OpenGL parameters.
    initRendering();

    // Setup particle system
    initSystem(argc,argv);
    mesh.load("window.obj");
    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

    // Set up callback functions for mouse
    glutMouseFunc(mouseFunc);
    glutMotionFunc(motionFunc);

    // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Trigger timerFunc every 20 msec
    glutTimerFunc(20, timerFunc, 20);

        
    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop();

    return 0;	// This line is never reached.
}
