/***********************************************************
             CSC418, Winter 2016
 
                 penguin.cpp
                 author: Mike Pratscher
                 based on code by: Eron Steger, J. Radulovich

		Main source file for assignment 2
		Uses OpenGL, GLUT and GLUI libraries
  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design. In particular, see lines marked 'README'.
		
		Be sure to also look over keyframe.h and vector.h.
		While no changes are necessary to these files, looking
		them over will allow you to better understand their
		functionality and capabilites.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <glui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "keyframe.h"
#include "timer.h"
#include "vector.h"


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

const float SPINNER_SPEED = 0.1;

const float SPINNER_SPEED2 = 2;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;				// Glut window ID (for display)
int Win[2];					// window (x,y) size

GLUI* glui_joints;			// Glui window with joint controls
GLUI* glui_keyframe;		// Glui window with keyframe controls
GLUI* glui_render;			// Glui window for render style

char msg[256];				// String used for status message
GLUI_StaticText* status;	// Status message ("Status: <msg>")


// ---------------- ANIMATION VARIABLES ---------------------

// Camera settings
bool updateCamZPos = false;
int  lastX = 0;
int  lastY = 0;
const float ZOOM_SCALE = 0.01;

GLdouble camXPos =  0.0;
GLdouble camYPos =  -4.0;
GLdouble camZPos = -50.5;

const GLdouble CAMERA_FOVY = 60.0;
const GLdouble NEAR_CLIP   = 0.1;
const GLdouble FAR_CLIP    = 1000.0;

// Render settings
enum { WIREFRAME, SOLID, OUTLINED, SHINY, MATTE };// README: the different render styles
int renderStyle = WIREFRAME;			// README: the selected render style

// Animation settings
int animate_mode = 0;			// 0 = no anim, 1 = animate

// Keyframe settings
const char filenameKF[] = "keyframes.txt";	// file for loading / saving keyframes

Keyframe* keyframes;			// list of keyframes

int maxValidKeyframe   = 0;		// index of max VALID keyframe (in keyframe list)
const int KEYFRAME_MIN = 0;
const int KEYFRAME_MAX = 32;	// README: specifies the max number of keyframes

// Frame settings
char filenameF[128];			// storage for frame filename

int frameNumber = 0;			// current frame being dumped
int frameToFile = 0;			// flag for dumping frames to file

const float DUMP_FRAME_PER_SEC = 24.0;		// frame rate for dumped frames
const float DUMP_SEC_PER_FRAME = 1.0 / DUMP_FRAME_PER_SEC;

float camHeight;
float camRadius = 50;
float camAngle = 0;

// Joint settings

// README: This is the key data structure for
// updating keyframes in the keyframe list and
// for driving the animation.
//   i) When updating a keyframe, use the values
//      in this data structure to update the
//      appropriate keyframe in the keyframe list.
//  ii) When calculating the interpolated pose,
//      the resulting pose vector is placed into
//      this data structure. (This code is already
//      in place - see the animate() function)
// iii) When drawing the scene, use the values in
//      this data structure (which are set in the
//      animate() function as described above) to
//      specify the appropriate transformations.
Keyframe* joint_ui_data;

// README: To change the range of a particular DOF,
// simply change the appropriate min/max values below
const float ROOT_TRANSLATE_X_MIN = -5.0;
const float ROOT_TRANSLATE_X_MAX =  5.0;
const float ROOT_TRANSLATE_Y_MIN = -5.0;
const float ROOT_TRANSLATE_Y_MAX =  5.0;
const float ROOT_TRANSLATE_Z_MIN = -5.0;
const float ROOT_TRANSLATE_Z_MAX =  5.0;
const float ROOT_ROTATE_X_MIN    = -180.0;
const float ROOT_ROTATE_X_MAX    =  180.0;
const float ROOT_ROTATE_Y_MIN    = -180.0;
const float ROOT_ROTATE_Y_MAX    =  180.0;
const float ROOT_ROTATE_Z_MIN    = -180.0;
const float ROOT_ROTATE_Z_MAX    =  180.0;
const GLfloat orange[] 		  = {1.0,170.0/255.0,0.0};
const GLfloat white[] 		  = {1.0,1.0,1.0,1.0};
const GLfloat black[] 		  = {0.0,0.0,0.0,0.0};
const GLfloat red[]		  = {1.0,0.0,0.0,1.0};
const GLfloat yellow[]		  = {1,1,0,1};
const GLfloat green[]		  = {0,1,0,1};
const GLfloat brown[]		  = {148/255,75/255,0,1};
const GLfloat net[]		  = {1,1,1,0.4};
const GLfloat spec[] 		  = {0.2,0.2,0.2,1.0};
float LIGHT_HEIGHT		  = 1;
float LIGHT_RAD			  = 2;
float LIGHT_THETA		  = 0;
const float speed		  = 1.5;
const float swingSpeed		  = 2;
bool Swing1_p1			  = false;
bool Swing2_p1			  = false;
bool Swing3_p1			  = false;
bool Swing1_p2			  = false;
bool Swing2_p2			  = false;
bool Swing3_p2			  = false;
float p1_x			  = 0.0;
float p1_y			  = 0.0;
float p2_x			  = 0.0;
float p2_y			  = 0.0;
float ball_x			  = 0.0;
float ball_y			  = 0.0;
float ball_z			  = 0.0;
float start_velocity = 0.3;
float velocity_y = 0.3;
float velocity_z = 0.1;
float velocity_x = 0.0;
float acceleration = 0.01;
int p1Score = 0;
int p2Score = 0;
GLfloat direction[] = {1.0f, 1.0f, 1.0f, 0.0f};
GLfloat position[] = {5.0f, 3.0f, 8.0f, 1.0f};
GLfloat spotDirection[] = {0.0f, 3.0f, 3.0f};
GLfloat diffuseRGBA[] = {0.5f, 1.0f, 1.0f, 1.0f};
GLfloat specularRGBA[] = {1.0f, 1.0f, 1.0f, 1.0f};



// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initDS();
void initGlut(int argc, char** argv);
//void initGlui();
void initGl();


// Callbacks for handling events in glut
void reshape(int w, int h);
void animate();
void display(void);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void update(int data);


// Functions to help draw the object
Vector getInterpolatedJointDOFS(float time);
void drawCube();
void drawPoly();
void drawFoot();
void drawPlayer(int c);
void drawCourt();
void drawWorld();
void drawNet();
void drawBall();
void drawOne();
void pressKey(unsigned char key, int x, int y);


// Image functions
void writeFrame(char* filename, bool pgm, bool frontBuffer);


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 640x480 window by default...\n");
        Win[0] = 1280;
        Win[1] = 960;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize data structs, glut, glui, and opengl
	initDS();
    initGlut(argc, argv);
    //initGlui();
    initGl();
    glutKeyboardFunc(pressKey);
    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


void update(int data)
{
  glutTimerFunc(3, update, -1);

  
  if (Swing1_p1){
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1) < 40){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_YAW_p1,joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1)+swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p1) > -40){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_ROLL_p1,joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p1)-swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p1) < 50){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_X_p1,joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p1)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1) >= 40 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p1) <= -40 && joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p1) >= 50)
      {	  
	  Swing1_p1 = false;
	  Swing2_p1 = true;
      }
    }
    if (Swing2_p1){
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1) > -40){
	joint_ui_data->setDOF(Keyframe::R_SHOULDER_YAW_p1,joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1)-3*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p1) > 0){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_X_p1,joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p1)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p1) > -30){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Z_p1,joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p1)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p1) < 40){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Y_p1,joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p1)+2*swingSpeed);
      } 
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p1) < 30){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_PITCH_p1,joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p1)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p1) >= 30 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p1) >= 40 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p1) <= -30 && joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p1) <= 0 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1) <= -40){
	  Swing2_p1 = false;
	  Swing3_p1 = true;
      }
    }
    if (Swing3_p1)
    {
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1) < 0){
	joint_ui_data->setDOF(Keyframe::R_SHOULDER_YAW_p1,joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1)+3*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p1) > 0){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_PITCH_p1,joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p1)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p1) > 0){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Y_p1,joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p1)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p1) < 0){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Z_p1,joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p1)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p1) < 0){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_ROLL_p1,joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p1)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p1) >= 0 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p1) >= 0 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p1) <= 0 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p1) <= 0 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1) >= 0)
      {
	Swing3_p1 = false;
      }
  
    }
        if (Swing1_p2){
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2) < 40){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_YAW_p2,joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2)+swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p2) > -40){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_ROLL_p2,joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p2)-swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p2) < 50){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_X_p2,joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p2)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2) >= 40 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p2) <= -40 && joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p2) >= 50)
      {	  
	  Swing1_p2 = false;
	  Swing2_p2 = true;
      }
    }
    if (Swing2_p2){
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2) > -40){
	joint_ui_data->setDOF(Keyframe::R_SHOULDER_YAW_p2,joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2)-3*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p2) > 0){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_X_p2,joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p2)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p2) > -30){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Z_p2,joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p2)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p2) < 40){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Y_p2,joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p2)+2*swingSpeed);
      } 
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p2) < 30){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_PITCH_p2,joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p2)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p2) >= 30 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p2) >= 40 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p2) <= -30 && joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p2) <= 0 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2) <= -40){
	  Swing2_p2 = false;
	  Swing3_p2 = true;
      }
    }
    if (Swing3_p2)
    {
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2) < 0){
	joint_ui_data->setDOF(Keyframe::R_SHOULDER_YAW_p2,joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2)+3*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p2) > 0){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_PITCH_p2,joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p2)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p2) > 0){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Y_p2,joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p2)-2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p2) < 0){
	  joint_ui_data->setDOF(Keyframe::R_ELBOW_Z_p2,joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p2)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p2) < 0){
	  joint_ui_data->setDOF(Keyframe::R_SHOULDER_ROLL_p2,joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p2)+2*swingSpeed);
      }
      if (joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p2) >= 0 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p2) >= 0 && joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p2) <= 0 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p2) <= 0 && joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2) >= 0)
      {
	Swing3_p2 = false;
      }
    }
  glutPostRedisplay();
}
// Create / initialize global data structures
void initDS()
{
	keyframes = new Keyframe[KEYFRAME_MAX];
	for( int i = 0; i < KEYFRAME_MAX; i++ )
		keyframes[i].setID(i);

	joint_ui_data  = new Keyframe();
}


// Initialize glut and create a window with the specified caption 
void initGlut(int argc, char** argv)
{
	// Init GLUT
	glutInit(&argc, argv);

    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(argv[0]);

    // Setup callback functions to handle events
    glutReshapeFunc(reshape);	// Call reshape whenever window resized
    glutDisplayFunc(display);	// Call display whenever new frame needed
        glutTimerFunc(2,update,-1);
	glutMouseFunc(mouse);		// Call mouse whenever mouse button pressed
	glutMotionFunc(motion);		// Call motion whenever mouse moves while button pressed
}



// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}




// Handles the window being resized by updating the viewport
// and projection matrices
void reshape(int w, int h)
{
	// Update internal variables and OpenGL viewport
	Win[0] = w;
	Win[1] = h;
	glViewport(0, 0, (GLsizei)Win[0], (GLsizei)Win[1]);

    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(CAMERA_FOVY, (GLdouble)Win[0]/(GLdouble)Win[1], NEAR_CLIP, FAR_CLIP);
}



// display callback
//
// README: This gets called by the event handler
// to draw the scene, so this is where you need
// to build your scene -- make your changes and
// additions here. All rendering happens in this
// function. For Assignment 2, updates to the
// joint DOFs (joint_ui_data) happen in the
// animate() function.
void display(void)
{
  
      // Draw our hinged object
    
    // Clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_RESCALE_NORMAL);	
    //glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseRGBA);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularRGBA);
    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
      
	if (p1Score == 10 || p2Score == 10)
	{
	  exit(1);
	}
	// Specify camera transformation
//	glTranslatef(camXPos, camYPos, camZPos);
	glTranslatef(camXPos, camAngle, camZPos);
	glTranslatef(camRadius*cos(camAngle),0,camRadius*sin(camAngle));
	glTranslatef(0,5,-30);
	
	for (int s=1;s<=p1Score;s++)
	{ 
	  glPushMatrix();
	  glTranslatef(-50 + s*3, 20, 0.0);
	  glScalef(0.5,3,1);
	  drawCube();
	  glPopMatrix();
	}
	for (int s=1;s<=p2Score;s++)
	{ 
	  glPushMatrix();
	  glTranslatef(50 - s*3, 20, 0.0);
	  glScalef(0.5,3,1);
	  drawCube();
	  glPopMatrix();
	}
	
	glRotatef(25,1,0,0);
	//glRotatef(30,0,1,0);

	// Get the time for the current animation step, if necessary
	glMaterialfv(GL_FRONT,GL_DIFFUSE,black);
	glMaterialfv(GL_FRONT,GL_AMBIENT,black);		  
	glMaterialfv(GL_FRONT,GL_SPECULAR,black);
	
	
	glTranslatef(0.0, -7.0, -5.0);
	glPushMatrix();
	  glScalef(2,2,2);
	  glTranslatef(p1_x,0,-18);
	  drawPlayer(0);
	glPopMatrix();
	glPushMatrix();
	  glScalef(2,2,2);
	  glTranslatef(p2_x,0,+18);
	  glRotatef(180,0,1,0);
	  drawPlayer(1);
	glPopMatrix();
	drawWorld();
	
	drawBall();
	
	ball_z += velocity_z;
	ball_y += velocity_y;
	ball_x += velocity_x;
	velocity_y -= acceleration;
	if (ball_y <= -1)
	{
	  ball_y = -1;
	  velocity_y = start_velocity;
	}
	if (ball_z >= 10)
	{
	  ball_z = 0.0;
	  velocity_z = -velocity_z;
	  velocity_x = 0;
	  ball_x = 0;
	  p2Score += 1;
	}
	if (ball_z <= -10)
	{
	  ball_z = 0.0;
	  velocity_x = 0;
	  ball_x = 0;
	  velocity_z = -velocity_z;
	  p1Score += 1;
	}
	if (ball_z > -10 && ball_z < -9 && ball_x > p1_x-3 && ball_x < p1_x + 3 &&(Swing1_p1 || Swing2_p1 || Swing3_p1))
	{
	  ball_z = -9;
	  velocity_z = -velocity_z;
	  velocity_x = (rand() % 10 - 5)/10.0;
	}
	if (ball_z > 9 && ball_z < 10 && ball_x > p2_x-3 && ball_x < p2_x + 3 && (Swing1_p2 || Swing2_p2 || Swing3_p2))
	{
	  ball_z = 9;
	  velocity_z = -velocity_z;
	  velocity_x = (rand() % 10 - 5)/10.0;
	}
	
	if (ball_x > 10)
	{
	  ball_x = 10;
	  velocity_x = -velocity_x;
	}
	if (ball_x < -10)
	{
	  ball_x = -10;
	  velocity_x = -velocity_x;
	}
	
    glDisable(GL_LIGHTING);

    // Execute any GL functions that are in the queue just to be safe
    glFlush();

    glutSwapBuffers();
    
}

// Handles mouse button pressed / released events
void mouse(int button, int state, int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( button == GLUT_RIGHT_BUTTON )
	{
		if( state == GLUT_DOWN )
		{
			lastX = x;
			lastY = y;
			updateCamZPos = true;
		}
		else
		{
			updateCamZPos = false;
		}
	}
}

void pressKey(unsigned char key, int x, int y){
    if(key=='s' && Swing1_p1 == false && Swing2_p1 == false && Swing3_p1 == false)
    {
      Swing1_p1 = true;
    }
    if(key=='k' && Swing1_p2 == false && Swing2_p2 == false && Swing3_p2 == false)
    {
      Swing1_p2 = true;
    }
    if (key=='a')
    {
      if (p1_x > -15)
      {
	p1_x -= speed;
      }
    }
    if (key=='d')
    {
      if (p1_x < 15)
      {
	p1_x += speed;
      }
    }
    
    if (key=='j')
    {
      if (p2_x > -15)
      {
	p2_x -= speed;
      }
    }
    if (key=='l')
    {
      if (p2_x < 15)
      {
	p2_x += speed;
      }
    }
    if (key=='x')
    {
      camAngle += 1;
    }
    switch (key)
    {
      case 27:
	exit (0);
	break;
    }
}
// Handles mouse motion events while a button is pressed
void motion(int x, int y)
{
	// If the RMB is pressed and dragged then zoom in / out
	if( updateCamZPos )
	{
		// Update camera z position
		camZPos += (x - lastX) * ZOOM_SCALE;
		lastX = x;

		// Redraw the scene from updated camera position
		glutSetWindow(windowID);
		glutPostRedisplay();
	}
}


// Draw a unit cube, centered at the current location
// README: Helper code for drawing a cube

void drawBall()
{	
	glMaterialfv(GL_FRONT,GL_DIFFUSE,black);
	glMaterialfv(GL_FRONT,GL_AMBIENT,black);		  
	glMaterialfv(GL_FRONT,GL_SPECULAR,black);
	glTranslatef(ball_x, ball_y, ball_z);
	glBegin(GL_QUADS);
		// draw front face
		glNormal3d(0, 0, 1);
		glVertex3f(-0.2,  1.8   , 0.2);
		glVertex3f( 0.2,  1.8   , 0.2);
		glVertex3f( 0.2,  2.2   , 0.2);
		glVertex3f(-0.2,  2.2   , 0.2);

		// draw back face
		glNormal3d(0, 0, -1);
		glVertex3f( 0.2,  1.8   , -0.2);
		glVertex3f(-0.2,  1.8   , -0.2);
		glVertex3f(-0.2,  2.2   , -0.2);
		glVertex3f( 0.2,  2.2   , -0.2);

		// draw left face
		glNormal3d(-1, 0, 0);
		glVertex3f(-0.2,  1.8   , -0.2);
		glVertex3f(-0.2,  1.8   ,  0.2);
		glVertex3f(-0.2,  2.2   ,  0.2);
		glVertex3f(-0.2,  2.2   , -0.2);

		// draw right face
		glNormal3d(1, 0, 0);
		glVertex3f( 0.2,  1.8   ,  0.2);
		glVertex3f( 0.2  ,  1.8   , -0.2);
		glVertex3f( 0.2  ,  2.2   , -0.2);
		glVertex3f( 0.2  ,  2.2   ,  0.2);

		// draw top
		glNormal3d(0, 1, 0);
		glVertex3f(-0.2  ,  2.2   ,  0.2);
		glVertex3f( 0.2  ,  2.2   ,  0.2);
		glVertex3f( 0.2  ,  2.2   , -0.2);
		glVertex3f(-0.2  ,  2.2   , -0.2);
	glEnd();
}

void drawNet()
	{
	for (int i = 0;i<5;i++){
	  glBegin(GL_QUADS);
		  glNormal3f(0, 0, 1);
		  glVertex3f(10,  i, 0.0);
		  glVertex3f( -10, i, 0.0);
		  glVertex3f( -10,  i+0.5, 0.0);
		  glVertex3f(10,  i+0.5, 0.0);
	  glEnd();
	}
	for (float i = -10.00; i <=10.00;i+=0.5)
	{
	  glBegin(GL_QUADS);
		  glNormal3f(0,0,1);
		  glVertex3f(i,0,0);
		  glVertex3f(i+0.25,0.0,0);
		  glVertex3f(i+0.25,4.5,0);
		  glVertex3f(i,4.5,0);
	  glEnd();
	}
	
}
void drawWorld()
{
  
  glMaterialfv(GL_FRONT,GL_DIFFUSE,green);
  glMaterialfv(GL_FRONT,GL_AMBIENT,green);		  
  glMaterialfv(GL_FRONT,GL_SPECULAR,green);
  drawCourt();
  glMaterialfv(GL_FRONT,GL_DIFFUSE,net);
  glMaterialfv(GL_FRONT,GL_AMBIENT,net);		  
  glMaterialfv(GL_FRONT,GL_SPECULAR,net);
  drawNet();
}
void drawPlayer(int c)
{
  glTranslatef(joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_X),
			  joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Y),
			  joint_ui_data->getDOF(Keyframe::ROOT_TRANSLATE_Z));
  glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_X),
			  1,0,0);
  glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Y)+180.0,
			  0,1,0);
  glRotatef(joint_ui_data->getDOF(Keyframe::ROOT_ROTATE_Z),
			  0,0,1);	
  //Drawing body
  glPushMatrix();
    glMaterialfv(GL_FRONT,GL_DIFFUSE,orange);
    glMaterialfv(GL_FRONT,GL_AMBIENT,orange);		  
    glMaterialfv(GL_FRONT,GL_SPECULAR,orange);
    glScalef(1.0,2.0,0.5);
    drawCube();
    //Drawing Head
      glPushMatrix();
      glMaterialfv(GL_FRONT,GL_DIFFUSE,red);
      glMaterialfv(GL_FRONT,GL_AMBIENT,red);		  
      glMaterialfv(GL_FRONT,GL_SPECULAR,red);
      glScalef(0.5,1/4.0,1.0);
      glTranslatef(0.0,5.0,0.0);
      drawCube();
      glPopMatrix();
    //Drawing Right Arm
      glPushMatrix();
	glScalef(1/1.0,1/2.0,1/0.5);
	glColor3f(255/255.0,153/255.0,51/255.00);
	glTranslatef(1,2,0);
	if (c == 0)
	{
	  glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p1),1,0,0);
			    glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p1),0,1,0);
			    glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p1),0,0,1);
	}
	else if(c == 1)
	{
	  glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_PITCH_p2),1,0,0);
			    glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_ROLL_p2),0,1,0);
			    glRotatef(joint_ui_data->getDOF(Keyframe::R_SHOULDER_YAW_p2),0,0,1);
	}
	glTranslatef(-1,-2,0);
	glScalef(0.25,0.7,0.25);
	glTranslatef(5.0,1.7,0);
	drawCube();
	//Drawing Right Forearm
	
	glPushMatrix();
	  glScalef(1/0.25,1/0.7,1/0.25);
	  glTranslatef(0,-0.5,0);
	  if (c == 0)
	  {
	    glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p1),1,0,0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p1),0,1,0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p1),0,0,1);
	  }
	  else if (c == 1)
	  {
	    glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW_X_p2),1,0,0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW_Y_p2),0,1,0);
			glRotatef(joint_ui_data->getDOF(Keyframe::R_ELBOW_Z_p2),0,0,1);
	  }
	  glTranslatef(0,0.5,0);		      
	  glScalef(0.25,0.7,0.25);
	  glTranslatef(0.0,-2.0,0);
	  drawCube();
	  
	  //Drawing the Racket
	  glMaterialfv(GL_FRONT,GL_DIFFUSE,brown);
	  glMaterialfv(GL_FRONT,GL_AMBIENT,brown);		  
	  glMaterialfv(GL_FRONT,GL_SPECULAR,brown);
	  glScalef(1/0.25,1/0.7,1/0.25);
	  
	  glTranslatef(0.5,-0.5,-0.5);
	  glRotatef(90,0,0,1);
	  glRotatef(45,1,0,0);
	  glScalef(0.05,0.7,0.05);
	  drawCube();
	  glScalef(1/0.05,1/0.7,1/0.05);
	  glTranslatef(0.0,-0.7,0.0);
	  glScalef(0.5,0.05,0.05);
	  drawCube();
	  glTranslatef(0.0,-15.0,0.0);
	  drawCube();
	  glScalef(1/0.5,1/0.05,1/0.05);
	  glRotatef(90,0,0,1);
	  glScalef(0.43,0.05,0.05);
	  glTranslatef(0.9,10,0);
	  drawCube();
	  glTranslatef(0,-20,0);
	  drawCube();
	  glColor3f(1,1,1);
	  glScalef(1/0.43,1/0.05,1/0.05);
	  glTranslatef(0,0.5,0);
	  glScalef(0.43,0.45,0.01);
	  glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
	  glMaterialfv(GL_FRONT,GL_AMBIENT,white);		  
	  glMaterialfv(GL_FRONT,GL_SPECULAR,white);
	  drawCube();
	  
	  
	  
	glPopMatrix();
      glPopMatrix();
      
    //Drawing Left Arm
        glMaterialfv(GL_FRONT,GL_DIFFUSE,red);
	glMaterialfv(GL_FRONT,GL_AMBIENT,red);		  
	glMaterialfv(GL_FRONT,GL_SPECULAR,red);
	glPushMatrix();
	glScalef(1/1.0,1/2.0,1/0.5);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,red);
	glMaterialfv(GL_FRONT,GL_AMBIENT,red);		  
	glMaterialfv(GL_FRONT,GL_SPECULAR,red);
	glTranslatef(-1,2,0);
	glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_PITCH),1,0,0);
			    glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_ROLL),0,1,0);
			    glRotatef(joint_ui_data->getDOF(Keyframe::L_SHOULDER_YAW),0,0,1);
	glTranslatef(1,-2,0);
	glScalef(0.25,0.7,0.25);
	glTranslatef(-5.0,1.7,0);
	drawCube();
	
	//Drawing Left Forearm
	
	glPushMatrix();
	  glScalef(1/0.25,1/0.7,1/0.25);
	  glTranslatef(0,-0.5,0);
	  glRotatef(joint_ui_data->getDOF(Keyframe::L_ELBOW_X),1,0,0);
			glRotatef(joint_ui_data->getDOF(Keyframe::L_ELBOW_Y),0,1,0);
			glRotatef(joint_ui_data->getDOF(Keyframe::L_ELBOW_Z),0,0,1);
	  glTranslatef(0,0.5,0);		      
	  glScalef(0.25,0.7,0.25);
	  glTranslatef(0.0,-2.0,0);
	  drawCube();	  
	glPopMatrix();
      glPopMatrix();
      
      //Drawing Legs
      glMaterialfv(GL_FRONT,GL_DIFFUSE,brown);
      glMaterialfv(GL_FRONT,GL_AMBIENT,brown);		  
      glMaterialfv(GL_FRONT,GL_SPECULAR,brown);
      //Left Leg
      glPushMatrix();
	glColor3f(0.7,0.5,0);
	glScalef(1.0,1/2.0,1/0.5);
	glScalef(0.1,1,0.1);
	glTranslatef(0,-3,0);
	drawCube();
	glScalef(1/0.1,1,1/0.1);
	glScalef(0.3,0.05,0.3);
	drawCube();
	glTranslatef(0,-4,0);
	drawCube();
	glTranslatef(0,-4,0);
	drawCube();
      glPopMatrix();
    glPopMatrix();

}
void drawCube()
{
	glBegin(GL_QUADS);
		// draw front face
		glNormal3f(0,0,1);
		glVertex3f(-1.0, -1.0, 1.0);
		glNormal3f(0,0,1);
		glVertex3f( 1.0, -1.0, 1.0);
		glNormal3f(0,0,1);
		glVertex3f( 1.0,  1.0, 1.0);
		glNormal3f(0,0,1);
		glVertex3f(-1.0,  1.0, 1.0);
		glNormal3f(0,0,1);

		// draw back face
		glNormal3f(0,0,-1);
		glVertex3f( 1.0, -1.0, -1.0);
		glNormal3f(0,0,-1);
		glVertex3f(-1.0, -1.0, -1.0);
		glNormal3f(0,0,-1);
		glVertex3f(-1.0,  1.0, -1.0);
		glNormal3f(0,0,-1);
		glVertex3f( 1.0,  1.0, -1.0);

		// draw left face
		glNormal3f(-1,0,0);
		glVertex3f(-1.0, -1.0, -1.0);
		glNormal3f(-1,0,0);
		glVertex3f(-1.0, -1.0,  1.0);
		glNormal3f(-1,0,0);
		glVertex3f(-1.0,  1.0,  1.0);
		glNormal3f(-1,0,0);
		glVertex3f(-1.0,  1.0, -1.0);

		// draw right face
		glNormal3f(1,0,0);
		glVertex3f( 1.0, -1.0,  1.0);
		glNormal3f(1,0,0);
		glVertex3f( 1.0, -1.0, -1.0);
		glNormal3f(1,0,0);
		glVertex3f( 1.0,  1.0, -1.0);
		glNormal3f(1,0,0);
		glVertex3f( 1.0,  1.0,  1.0);

		// draw top
		glNormal3f(0,1,0);
		glVertex3f(-1.0,  1.0,  1.0);
		glNormal3f(0,1,0);
		glVertex3f( 1.0,  1.0,  1.0);
		glNormal3f(0,1,0);
		glVertex3f( 1.0,  1.0, -1.0);
		glNormal3f(0,1,0);
		glVertex3f(-1.0,  1.0, -1.0);

		// draw bottom
		glNormal3f(0,-1,0);
		glVertex3f(-1.0, -1.0, -1.0);
		glNormal3f(0,-1,0);
		glVertex3f( 1.0, -1.0, -1.0);
		glNormal3f(0,-1,0);
		glVertex3f( 1.0, -1.0,  1.0);
		glNormal3f(0,-1,0);
		glVertex3f(-1.0, -1.0,  1.0);
	glEnd();
}

void drawCourt()
{	
	//glColor3f(0,1,0);
	glTranslatef(0,-6,0);
	glScalef(3,1,4);
	//glRotatef(joint_ui_data->getDOF(Keyframe::HEAD),0,1,0);
	glBegin(GL_QUADS);
		// draw top
		glNormal3d(0, 1, 0);
		glVertex3f(-10.0,  00.0,  10.0);
		glVertex3f( 10.0,  00.0,  10.0);
		glVertex3f( 10.0,  00.0, -10.0);
		glVertex3f(-10.0,  00.0, -10.0);

	glEnd();
}

void drawOne()
{
      glVertex3f(0.5,1,0);
      glVertex3f(0.5,0,0);
      glVertex3f(0,0,0);
      glVertex3f(0,1,0);
}
void drawPoly()
{
	glBegin(GL_QUADS);
		// draw front face
		glNormal3f(0,0,1);
		glVertex3f(-1.0, -1.0, 1.0);
		glNormal3f(0,0,1);
		glVertex3f( 1.0, -1.0, 1.0);
		glNormal3f(0,0,1);
		glVertex3f( 0.5,  1.0, 1.0);
		glNormal3f(0,0,1);
		glVertex3f(-0.5,  1.0, 1.0);

		// draw back face
		glNormal3f(0,0,-1);
		glVertex3f( 1.0, -1.0, -1.0);
		glNormal3f(0,0,-1);
		glVertex3f(-1.0, -1.0, -1.0);
		glNormal3f(0,0,-1);
		glVertex3f(-0.5,  1.0, -1.0);
		glNormal3f(0,0,-1);
		glVertex3f( 0.5,  1.0, -1.0);

		// draw left face
		glNormal3f(-0.5,0.5,0);
		glVertex3f(-1.0, -1.0, -1.0);
		glNormal3f(-0.5,0.5,0);
		glVertex3f(-1.0, -1.0,  1.0);
		glNormal3f(-0.5,0.5,0);
		glVertex3f(-0.5,  1.0,  1.0);
		glNormal3f(-0.5,0.5,0);
		glVertex3f(-0.5,  1.0, -1.0);
		
		// draw right face
		glNormal3f(0.5,0.5,0);
		glVertex3f( 1.0, -1.0,  1.0);
		glNormal3f(0.5,0.5,0);
		glVertex3f( 1.0, -1.0, -1.0);
		glNormal3f(0.5,0.5,0);
		glVertex3f( 0.5,  1.0, -1.0);
		glNormal3f(0.5,0.5,0);
		glVertex3f( 0.5,  1.0,  1.0);

		// draw top
		glNormal3f(0,1,0);
		glVertex3f(-0.5,  1.0,  1.0);
		glNormal3f(0,1,0);
		glVertex3f( 0.5,  1.0,  1.0);
		glNormal3f(0,1,0);
		glVertex3f( 0.5,  1.0, -1.0);
		glNormal3f(0,1,0);
		glVertex3f(-0.5,  1.0, -1.0);

		// draw bottom
		glNormal3f(0,-1,0);
		glVertex3f(-1.0, -1.0, -1.0);
		glNormal3f(0,-1,0);
		glVertex3f( 1.0, -1.0, -1.0);
		glNormal3f(0,-1,0);
		glVertex3f( 1.0, -1.0,  1.0);
		glNormal3f(0,-1,0);
		glVertex3f(-1.0, -1.0,  1.0);
	glEnd();
}

