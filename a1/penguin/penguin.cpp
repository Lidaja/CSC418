/***********************************************************
             CSC418/2504, Fall 2011
  
                 robot.cpp
                 
       Simple demo program using OpenGL and the glut/glui 
       libraries

  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.
		
		Some windows-specific code remains in the includes; 
		we are not maintaining windows build files this term, 
		but we're leaving that here in case you want to try building
		on windows on your own.

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

#ifndef _WIN32
#include <unistd.h>
#else
void usleep(unsigned int nanosec)
{
    Sleep(nanosec / 1000);
}
#endif


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;               // Glut window ID (for display)
GLUI *glui;                 // Glui window (for controls)
int Win[2];                 // window (x,y) size


// ---------------- ANIMATION VARIABLES ---------------------

// Animation settings
int animate_mode = 0;       // 0 = no anim, 1 = animate
int animation_frame = 0;      // Specify current frame of animation
int run_mode = 0;
int run_frame = 0;
int jump_mode = 0;
int jump_frame = 0;

// Joint parameters
const float JOINT_MIN = -45.0f;
const float JOINT_MAX =  45.0f;

const float WING_MIN = -20.0f;
const float WING_MAX =  16.0f;

const float BEAK_MIN = 0.0f;
const float BEAK_MAX = 10.0f;

const float PENGUIN_MAX = 300.0f;
const float PENGUIN_MIN = -300.0f;

const float run_vel = 5.0f;

float joint_rot = 0.0f;

//////////////////////////////////////////////////////
// TODO: Add additional joint parameters here
//////////////////////////////////////////////////////

float head_joint = 0.0f;
float arm_joint = 0.0f;
float right_leg_joint = 10.0f;
float right_foot_joint = 0.0f;
float left_leg_joint = -20.0f;
float left_foot_joint = 0.0f;
float wing_joint = 0.0f;
float beak_joint = 0.0f;
float penguinX = 0.0f;
float penguinY = 0.0f;

// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initGlut(char* winName);
void initGlui();
void initGl();


// Callbacks for handling events in glut
void myReshape(int w, int h);
void animate();
void run();
void jump();
void display(void);

// Callback for handling events in glui
void GLUI_Control(int id);


// Functions to help draw the object
void drawSquare(float size);
void drawBody(float size);
void drawHead(float size);
void drawArm(float size);
void drawLeg(float size);
void drawFoot(float size);
void drawBeak(float size);
void drawJoint(int segments, float radius);
void drawEye(int segments, float radius);

// Return the current system clock (in seconds)
double getTime();


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
        printf("Using 300x200 window by default...\n");
        Win[0] = 500;
        Win[1] = 400;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize glut, glui, and opengl
    glutInit(&argc, argv);
    initGlut(argv[0]);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


// Initialize glut and create a window with the specified caption 
void initGlut(char* winName)
{
    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(winName);

    // Setup callback functions to handle events
    glutReshapeFunc(myReshape); // Call myReshape whenever window resized
    glutDisplayFunc(display);   // Call display whenever new frame needed 
}


// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Animate button handler.  Called when the "animate" checkbox is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui->sync_live();

  animation_frame = 0;
  if(animate_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(animate);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

void runButton(int)
{
  if (run_frame == 0){
    penguinX = 300;
  }
  else{
    penguinX = 0;
  }
  // synchronize variables that GLUT uses
  glui->sync_live();

  run_frame = 0;
  if(run_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(run);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

void jumpButton(int)
{
  if (jump_frame == 0){
    penguinY = -Win[1]/5;
  }
  else{
    penguinY = 0;
  }
  // synchronize variables that GLUT uses
  glui->sync_live();

  jump_frame = 0;
  if(jump_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(jump);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

// Initialize GLUI and the user interface
void initGlui()
{
    GLUI_Master.set_glutIdleFunc(NULL);

    // Create GLUI window
    glui = GLUI_Master.create_glui("Glui Window", 0, Win[0]+10, 0);

    // Create a control to specify the rotation of the joint
    GLUI_Spinner *arm_spinner
        = glui->add_spinner("Arm Joint", GLUI_SPINNER_FLOAT, &arm_joint);
    arm_spinner->set_speed(2.0);
    arm_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

    ///////////////////////////////////////////////////////////
    // TODO: 
    //   Add controls for additional joints here
    ///////////////////////////////////////////////////////////
    GLUI_Spinner *head_spinner
        = glui->add_spinner("Head Joint", GLUI_SPINNER_FLOAT, &head_joint);
    head_spinner->set_speed(2.0);
    head_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    GLUI_Spinner *left_leg_spinner
        = glui->add_spinner("Left Leg Joint", GLUI_SPINNER_FLOAT, &left_leg_joint);
    left_leg_spinner->set_speed(2.0);
    left_leg_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    GLUI_Spinner *right_leg_spinner
        = glui->add_spinner("Right Leg Joint", GLUI_SPINNER_FLOAT, &right_leg_joint);
    right_leg_spinner->set_speed(2.0);
    right_leg_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    GLUI_Spinner *left_foot_spinner
        = glui->add_spinner("Left Foot Joint", GLUI_SPINNER_FLOAT, &left_foot_joint);
    left_foot_spinner->set_speed(2.0);
    left_foot_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    GLUI_Spinner *right_foot_spinner
        = glui->add_spinner("Right Foot Joint", GLUI_SPINNER_FLOAT, &right_foot_joint);
    right_foot_spinner->set_speed(2.0);
    right_foot_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    GLUI_Spinner *wing_flap
        = glui->add_spinner("Wing Flap", GLUI_SPINNER_FLOAT, &wing_joint);
    wing_flap->set_speed(2.0);
    wing_flap->set_float_limits(WING_MIN, WING_MAX, GLUI_LIMIT_CLAMP);
    
    GLUI_Spinner *beak_spinner
        = glui->add_spinner("Beak", GLUI_SPINNER_FLOAT, &beak_joint);
    beak_spinner->set_speed(2.0);
    beak_spinner->set_float_limits(BEAK_MIN, BEAK_MAX, GLUI_LIMIT_CLAMP);
    
    GLUI_Spinner *penguin_spinnerX
        = glui->add_spinner("Penguin's X Position", GLUI_SPINNER_FLOAT, &penguinX);
    penguin_spinnerX->set_speed(2.0);
    penguin_spinnerX->set_float_limits(PENGUIN_MIN, PENGUIN_MAX, GLUI_LIMIT_CLAMP);
    
    GLUI_Spinner *penguin_spinnerY
        = glui->add_spinner("Penguin's Y Position", GLUI_SPINNER_FLOAT, &penguinY);
    penguin_spinnerY->set_speed(2.0);
    penguin_spinnerY->set_float_limits(PENGUIN_MIN, PENGUIN_MAX, GLUI_LIMIT_CLAMP);
    // Add button to specify animation mode 
    glui->add_separator();
    glui->add_checkbox("Animate Everything", &animate_mode, 0, animateButton);

    glui->add_separator();
    glui->add_checkbox("Run Across the Screen", &run_mode, 0, runButton);

    glui->add_separator();
    glui->add_checkbox("JUMP!", &jump_mode, 0, jumpButton);

    // Add "Quit" button
    glui->add_separator();
    glui->add_button("Quit", 0, quitButton);

    // Set the main window to be the "active" window
    glui->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}




// Callback idle function for animating the scene
void animate()
{
    // Update geometry
    const double joint_rot_speed = 0.1;
    double joint_rot_t = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function animate the character's joints
    //   Note: Nothing should be drawn in this function!  OpenGL drawing
    //   should only happen in the display() callback.
    ///////////////////////////////////////////////////////////
    arm_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    head_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    right_leg_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    left_leg_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    left_foot_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    right_foot_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    wing_joint = joint_rot_t * WING_MIN + (1 - joint_rot_t) * WING_MAX;
    beak_joint = joint_rot_t * BEAK_MIN + (1 - joint_rot_t) * BEAK_MAX;
    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    animation_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}

void run()
{
    // Update geometry
    const double joint_rot_speed = 0.3;
    double joint_rot_t1 = (sin(run_frame*joint_rot_speed) + 1.0) / 2.0;
    double joint_rot_t2 = (cos(run_frame*joint_rot_speed) + 1.0) / 2.0;
    //double joint_rot_t2 = (cos(run_frame*joint_rot_speed) + 1.0) / 2.0;
    //double joint_rot_t2 = (cos(run_frame*joint_rot_speed) + 1.0) / 2.0;
    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function animate the character's joints
    //   Note: Nothing should be drawn in this function!  OpenGL drawing
    //   should only happen in the display() callback.
    ///////////////////////////////////////////////////////////
    arm_joint = joint_rot_t1 * JOINT_MIN + (1 - joint_rot_t1) * JOINT_MAX;
    //head_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    right_leg_joint = joint_rot_t1 * JOINT_MIN + (1 - joint_rot_t1) * JOINT_MAX;
    left_leg_joint = joint_rot_t2 * JOINT_MIN + (1 - joint_rot_t2) * JOINT_MAX;
    left_foot_joint = joint_rot_t2 * JOINT_MIN + (1 - joint_rot_t2) * JOINT_MAX;
    right_foot_joint = joint_rot_t1 * JOINT_MIN + (1 - joint_rot_t1) * JOINT_MAX;
    penguinX -= run_vel;
    if (penguinX == -Win[0]/2)
    {
      penguinX = Win[0]/2;
    }
    //wing_joint = joint_rot_t * WING_MIN + (1 - joint_rot_t) * WING_MAX;
    //beak_joint = joint_rot_t * BEAK_MIN + (1 - joint_rot_t) * BEAK_MAX;
    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    run_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}

void jump()
{
    // Update geometry
    const double joint_rot_speed = 0.3;
    double joint_rot_t = (sin(run_frame*joint_rot_speed) + 1.0) / 2.0;
    penguinY += 20*cos(run_frame*joint_rot_speed);
    //double joint_rot_t2 = (cos(run_frame*joint_rot_speed) + 1.0) / 2.0;
    //double joint_rot_t2 = (cos(run_frame*joint_rot_speed) + 1.0) / 2.0;
    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function animate the character's joints
    //   Note: Nothing should be drawn in this function!  OpenGL drawing
    //   should only happen in the display() callback.
    ///////////////////////////////////////////////////////////

    left_foot_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    right_foot_joint = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    wing_joint = joint_rot_t * WING_MIN + (1 - joint_rot_t) * WING_MAX;
    beak_joint = joint_rot_t * BEAK_MIN + (1 - joint_rot_t) * BEAK_MAX;
    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    run_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}

// Handles the window being resized by updating the viewport
// and projection matrices
void myReshape(int w, int h)
{
    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w/2, w/2, -h/2, h/2);

    // Update OpenGL viewport and internal variables
    glViewport(0,0, w,h);
    Win[0] = w;
    Win[1] = h;
}



// display callback
//
// This gets called by the event handler to draw
// the scene, so this is where you need to build
// your scene -- make your changes and additions here.
// All rendering happens in this function.  For Assignment 1,
// updates to geometry should happen in the "animate" function.
void display(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);

    // OK, now clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function draw the scene
    //   This should include function calls to pieces that
    //   apply the appropriate transformation matrice and
    //   render the individual body parts.
    ///////////////////////////////////////////////////////////

    // Draw our hinged object
    const float BODY_WIDTH = 30.0f;
    const float BODY_LENGTH = 50.0f;
    const float ARM_LENGTH = 50.0f;
    const float ARM_WIDTH = 20.0f;
    const float HEAD_LENGTH = 25.0f;
    const float HEAD_WIDTH = 45.0f;
    const float LEG_WIDTH = 10.0f;
    const float LEG_LENGTH = 30.0f;
    const float FOOT_WIDTH = 50.0f;
    const float FOOT_LENGTH = 10.0f;
    const float LOWER_BEAK_WIDTH = 45.0;
    const float LOWER_BEAK_LENGTH = 5.0;
    const float UPPER_BEAK_WIDTH = 45.0;
    const float UPPER_BEAK_LENGTH = 10.0;
    const float EYE_RAD = 1.0f;

    // Push the current transformation matrix on the stack
    glPushMatrix();
        glTranslatef(penguinX,penguinY,0.0);
        // Draw the 'body'
        glPushMatrix();
            // Scale square to size of body
            glScalef(BODY_WIDTH, BODY_LENGTH, 1.0);

            // Set the colour to green
            glColor3f(1.0, 1.0, 1.0);

            // Draw the square for the body
            // drawSquare(1.0);
	    drawBody(1.0);
        glPopMatrix();

	glPushMatrix();
	  // Draw the 'head'
	  glTranslatef(0.0, BODY_LENGTH/3 + HEAD_WIDTH,0.0);
	  glRotatef(head_joint,0.0,0.0,1.0);
	  //glTranslatef(0.0, -(BODY_LENGTH/7 + HEAD_WIDTH),0.0);
	  //Draw the upper beak
	  glPushMatrix();
	    glTranslatef(-(UPPER_BEAK_WIDTH*7/8),UPPER_BEAK_LENGTH/2,0.0);
	    glScalef(UPPER_BEAK_WIDTH,UPPER_BEAK_LENGTH,0.0);
	    glColor3f(1.0,0.56,0.0);
	    drawBeak(1.0);
	  glPopMatrix();

	  //Draw the lower beak
	  glPushMatrix();
	    glTranslatef(-(LOWER_BEAK_WIDTH*7/8),-LOWER_BEAK_LENGTH+beak_joint,0.0);
	    glScalef(LOWER_BEAK_WIDTH,LOWER_BEAK_LENGTH,0.0);
	    glColor3f(1.0,0.56,0.0);
	    drawBeak(1.0);
	  glPopMatrix();
	  
	  glScalef(HEAD_WIDTH, HEAD_LENGTH, 1.0);
	  glColor3f(0.0,0.0,0.0);
	  drawHead(1.0);
	  glColor3f(1,1,1);
	  glScalef(1.0f/HEAD_WIDTH,1.0f/HEAD_LENGTH,0.0);
	  glScalef(EYE_RAD,EYE_RAD,0.0);
	  glTranslatef(0.0,10*EYE_RAD,0.0);
	  drawEye(10,5);
	glPopMatrix();

	
	
	
	
	
	
	
	//Draw the 'arm'
	glPushMatrix();
	  glTranslatef(0.0,BODY_LENGTH/4 + ARM_WIDTH,0.0);
	  glRotatef(arm_joint, 0.0, 0.0, 1.0);
	  glScalef(ARM_WIDTH, ARM_LENGTH+wing_joint, 1.0);
	  glTranslatef(0.0, -0.5, 0.0);
	  glColor3f(0.0, 0.0, 0.0);
	  drawArm(1.0);
	glPopMatrix();
	
	//Left Leg
	glPushMatrix();
	  glTranslatef(-BODY_WIDTH/2,-BODY_LENGTH + LEG_WIDTH,0.0);
	  glRotatef(left_leg_joint, 0.0, 0.0, 1.0);
	  //Left Foot
	  glPushMatrix();
	    glTranslatef(0.0,-LEG_LENGTH,0.0);
	    glRotatef(left_foot_joint,0.0,0.0,1.0);
	    glTranslatef(-FOOT_WIDTH/2,0.0,0.0);
	    glScalef(FOOT_WIDTH, FOOT_LENGTH, 1.0);
	    glColor3f(0.0, 0.0, 0.0);
	    drawSquare(1.0);
	  glPopMatrix();
	  glScalef(LEG_WIDTH, LEG_LENGTH, 1.0);
	  glTranslatef(0.0, -0.5, 0.0);
	  glColor3f(1.0, 0.56, 0.0);
	  drawSquare(1.0);
	  glColor3f(1,1,1);
	  glScalef(1.0f/LEG_WIDTH,1.0f/LEG_LENGTH,1.0);
	  glTranslatef(0,-LEG_LENGTH/2,0);
	  drawJoint(10,5);
	glPopMatrix();

	//Right Leg
	glPushMatrix();
	  glTranslatef(BODY_WIDTH/2,-BODY_LENGTH + LEG_WIDTH,0.0);
	  glRotatef(right_leg_joint, 0.0, 0.0, 1.0);
	  //Right Foot
	  
	  glPushMatrix();
	    glTranslatef(0.0,-LEG_LENGTH,0.0);
	    glRotatef(right_foot_joint,0.0,0.0,1.0);
	    glTranslatef(-FOOT_WIDTH/2,0.0,0.0);
	    glScalef(FOOT_WIDTH, FOOT_LENGTH, 1.0);
	    glColor3f(0.0, 0.0, 0.0);
	    drawSquare(1.0);
	  glPopMatrix();
	  
	  glScalef(LEG_WIDTH, LEG_LENGTH, 1.0);
	  glTranslatef(0.0, -0.5, 0.0);
	  glColor3f(1.0, 0.56, 0.0);
	  drawSquare(1.0);
	  glColor3f(1,1,1);
	  glScalef(1.0f/LEG_WIDTH,1.0f/LEG_LENGTH,1.0);
	  glTranslatef(0,-LEG_LENGTH/2,0);
	  drawJoint(10,5);
	glPopMatrix();
	
	glPushMatrix();
	  glTranslatef(BODY_WIDTH/2,-BODY_LENGTH + LEG_WIDTH,0.0);
	  glColor3f(0,0,0);
	  drawJoint(10,5);
	glPopMatrix();
	
	glPushMatrix();
	  glColor3f(0,0,0);
	  glTranslatef(-BODY_WIDTH/2,-BODY_LENGTH + LEG_WIDTH,0.0);
	  drawJoint(10,5);
	glPopMatrix();
	
	
	glPushMatrix();
	  glColor3f(1.0,0.56,0.0);	
	  glTranslatef(0.0,BODY_LENGTH/4 + ARM_WIDTH,0.0);
	  drawJoint(10,5);
	glPopMatrix();
	
	glPushMatrix();
	  glColor3f(1.0,0.56,0.0);
	  glTranslatef(0.0, BODY_LENGTH/4 + HEAD_WIDTH,0.0);
	  drawJoint(10,5);
	glPopMatrix();  
	
    // Retrieve the previous state of the transformation stack
    glPopMatrix();


    // Execute any GL functions that are in the queue just to be safe
    glFlush();

    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}


// Draw a square of the specified size, centered at the current location
void drawSquare(float width)
{
    // Draw the square
    glBegin(GL_POLYGON);
    glVertex2d(-width/2, -width/2);
    glVertex2d(width/2, -width/2);
    glVertex2d(width/2, width/2);
    glVertex2d(-width/2, width/2);
    glEnd();
}

void drawBody(float width)
{
	width = 2 * width;
	glBegin(GL_POLYGON);
	glVertex2d(-width/3,-width/2);
	glVertex2d(-width/2,-width/4);
	glVertex2d(-width/3,width/2);
	glVertex2d(width/3,width/2);
	glVertex2d(width/2,-width/4);
	glVertex2d(width/3,-width/2);
	glEnd();
}

void drawHead(float width)
{
	glBegin(GL_POLYGON);
	glVertex2d(-width/2,-width/2);
	glVertex2d(width/2,-width/2);
	glVertex2d((width*2/3)/2,(width*3/4));
	glVertex2d((-width*1/3)/2,width);
	glVertex2d((-width*2/3)/2,(width*3/4));
	glEnd();
}

void drawArm(float width)
{
	glBegin(GL_POLYGON);
	glVertex2d(-width/2,width/2);
	glVertex2d(-width/4,-width/2);
	glVertex2d(width/4,-width/2);
	glVertex2d(width/2,width/2);
	glEnd();
}

void drawBeak(float width)
{
	glBegin(GL_POLYGON);
	glVertex2d(-width/2,-width/2);
	glVertex2d(width/2,-width/2);
	glVertex2d(width/2,width/2);
	glVertex2d(-width/2,width/4);
	glEnd();
}

void drawJoint(int segments, float radius){
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++)   {
        float theta = 2.0f * PI * float(i) / float(segments);//get the current angle 
        float xPos = radius * cosf(theta);//calculate the x component 
        float yPos = radius * sinf(theta);//calculate the y component 
        glVertex2f(xPos, yPos); 
    }
    glEnd();
}

void drawEye(int segments, float radius){
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++)   {
        float theta = 2.0f * PI * float(i) / float(segments);//get the current angle 
        float xPos = radius * cosf(theta);//calculate the x component 
        float yPos = radius * sinf(theta);//calculate the y component 
        glVertex2f(xPos, yPos); 
    }
    glEnd(); 
}

