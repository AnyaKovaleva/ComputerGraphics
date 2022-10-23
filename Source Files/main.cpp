#include <GL/freeglut.h>
#include <iostream>

/* Flag telling us to keep executing the main loop */
static int continue_in_main_loop = 1;

GLfloat xRotated, yRotated, zRotated;
// Cone
GLdouble base = 3;
GLdouble height = 6;
GLint slices = 50;
GLint stacks = 50;

GLfloat translate_x = 0;
GLfloat translate_y =  -0.2;
GLfloat translate_z = 0;

GLfloat step = 0.2;

GLfloat rotation_x = 1;
GLfloat rotation_y = 0;
GLfloat rotation_z = -6.2;
GLfloat rotation_angle = 90;

GLfloat rotationStep = 1;

bool isIncrementing = true;

bool isModifyingTranslate = true;

static void Display();

static void Key(unsigned char key, int x, int y)
{
  std::cout << "\nrescieved keyboard event\n";
  std::cout << "key: " << key << std::endl;

  switch (key)
  {
    case 27:  /* Escape key */
      continue_in_main_loop = 0;
      break;
    case '+':
      std::cout << "pressed plus\n";
      isIncrementing = true;
      break;
    case '-':
      std::cout << "pressed - \n";
      isIncrementing = false;
      break;
    case 'r':
      std::cout << "pressed r. modifying rotation\n";
      isModifyingTranslate = false;
      break;
    case 't':
      std::cout << "pressed t. modifying translate\n";
      isModifyingTranslate = true;
      break;
    case 'z':
      if(isModifyingTranslate)
      {
        std::cout << "translate\n";
        isIncrementing ? translate_z += step : translate_z -= step;
        std::cout << "z = " << translate_z << std::endl;
      }
      else
      {
        std::cout << "rotation\n";
        rotation_angle = isIncrementing ? rotation_angle + rotationStep : rotation_angle - rotationStep;
        std::cout << "angle " << rotation_angle << std::endl;

        rotation_z= 1;
        rotation_x= 0;
        rotation_y= 0;

        std::cout << rotation_x << rotation_y <<rotation_z;
      }
      break;
    case 'x':
      if(isModifyingTranslate)
      {
        std::cout << "translate\n";
        isIncrementing ? translate_x += step : translate_x -= step;
        std::cout << "x = " << translate_x<< std::endl;
      }
      else
      {
        std::cout << "rotation\n";
        rotation_angle = isIncrementing ? rotation_angle + rotationStep : rotation_angle - rotationStep;
        std::cout << "angle " << rotation_angle<< std::endl;

        rotation_z= 0;
        rotation_x= 1;
        rotation_y= 0;

        std::cout << rotation_x << rotation_y <<rotation_z<< std::endl;
      }
      break;
    case 'y':
      if(isModifyingTranslate)
      {
        std::cout << "translate\n";
        translate_y = isIncrementing ? translate_y + step : translate_y - step;
        std::cout << "y = " << translate_y<< std::endl;
      }
      else
      {
        std::cout << "rotation\n";
        rotation_angle = isIncrementing ? rotation_angle + rotationStep : rotation_angle - rotationStep;
        std::cout << "angle " << rotation_angle<< std::endl;

        rotation_z= 0;
        rotation_x= 0;
        rotation_y= 1;

        std::cout << rotation_x << rotation_y <<rotation_z<< std::endl;
      }
      break;
  }

  Display();
}

static void Display()
{
  glMatrixMode(GL_MODELVIEW);
  // clear the drawing buffer.
  glClear(GL_COLOR_BUFFER_BIT);
  // clear the identity matrix.
  glLoadIdentity();
  // traslate the draw by z = -4.0
//  // Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
//  glTranslatef(0.0,0.0,-1);
//  // Red color used to draw.
//   glColor3f(0.8, 0.2, 0.1);
//  // changing in transformation matrix.
//  // rotation about X axis
//  glRotatef(xRotated,1.0,0.0,0.0);
//  // rotation about Y axis
//  glRotatef(yRotated,0.0,1.0,0.0);
//  // rotation about Z axis
//  glRotatef(zRotated,0.0,0.0,1.0);
//  // scaling transfomation
//  glScalef(1.0,1.0,1.0);
//  // built-in (glut library) function , draw you a Cone.

  //glTranslatef(translate_x, translate_y, translate_z);
  // glRotatef(90, 1, 0, 0);

  // Flush buffers to screen

  glRotatef(rotation_angle, rotation_x, rotation_y, rotation_z);
  glTranslatef(translate_x, translate_y, translate_z);

  glutWireCone(base, height, slices, stacks);


  glFlush();
  // sawp buffers called because we are using double buffering
  glutSwapBuffers();

}

void reshapeCone(int x, int y)
{
  if (y == 0 || x == 0)
  { return; }  //Nothing is visible then, so return
  //Set a new projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //Angle of view:40 degrees
  //Near clipping plane distance: 0.5
  //Far clipping plane distance: 20.0


  // gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,20.0);
  glDepthRange(0.001, 50);
  glViewport(0, 0, x, y);  //Use the whole window for rendering
}

void idleCone(void)
{

  yRotated += 0.01;

  Display();
}

int main(int argc, char *argv[])
{
  glutInitWindowSize(800, 800);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  glutInit(&argc, argv);

  glutCreateWindow("Test  window");

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//  xRotated = yRotated = zRotated = 30.0;
//  xRotated = 33;
//  yRotated = 40;
  glClearColor(0.0, 0.0, 0.0, 0.0);

  //Assign  the function used in events
  glutDisplayFunc(Display);
  glutKeyboardFunc(Key);
  glutReshapeFunc(reshapeCone);
  glutIdleFunc(idleCone);
  //Let start glut loop
  //glutMainLoop();

  while (continue_in_main_loop)
    glutMainLoopEvent();

  // printf ( "Back from the 'freeglut' main loop\n" ) ;

  return 0;
}

