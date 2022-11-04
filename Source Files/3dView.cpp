/* 3D view manipulation demo
 * Written by John Tsiombikas <nuclear@member.fsf.org>
 *
 * Demonstrates how to use freeglut callbacks to manipulate a 3D view, similarly
 * to how a modelling program or a model viewer would operate.
 *
 * Rotate: drag with the left mouse button.
 * Scale: drag up/down with the right mouse button.
 * Pan: drag with the middle mouse button.
 *
 * Press space to animate the scene and update the display continuously, press
 *   again to return to updating only when the view needs to change.
 * Press escape or q to exit.
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/freeglut.h>
#include <iostream>

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4305 4244)
#endif

static const char *helpprompt[] = {"Press F1 for help", 0};
static const char *helptext[] = {
  "Rotate: left mouse drag",
  " Scale: right mouse drag up/down",
  "   Pan: middle mouse drag",
  "",
  "Toggle fullscreen: f",
  "Toggle animation: space",
  "Quit: escape",
  0
};

void idle(void);

void display(void);

void print_help(void);

void reshape(int x, int y);

void keypress(unsigned char key, int x, int y);

void skeypress(int key, int x, int y);

void mouse(int bn, int st, int x, int y);

void motion(int x, int y);

int win_width, win_height;
float cam_theta, cam_phi = 25, cam_dist = 8;
float cam_pan[3];
int mouse_x, mouse_y;
int bnstate[8];
int anim, help;
long anim_start;
long nframes;

#ifndef GL_FRAMEBUFFER_SRGB
#define GL_FRAMEBUFFER_SRGB  0x8db9
#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809d
#endif

GLdouble base = 30;
GLdouble height = 60;
GLdouble radius = 30;
GLint slices = 50;
GLint stacks = 50;

GLfloat translate_x = 50;
GLfloat translate_y = 40;
GLfloat translate_z = 0;

GLfloat coneRotation_z;

GLfloat sphereScaleFactor = 2;
GLfloat sphereCurrentScaleFactor = 1;

GLfloat cylinderHeight = 2;
GLfloat teapotSize = 1;

bool isTask2Active = false;

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("freeglut 3D view demo");

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);
  glutSpecialFunc(skeypress);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glutMainLoop();
  return 0;
}


void idle(void)
{
  glutPostRedisplay();
  if(isTask2Active)
  {
    display();
  }
}



void display(void)
{
  long tm;
  float lpos[] = {-1, 2, 3, 0};

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -cam_dist);
  glRotatef(cam_phi, 1, 0, 0);
  glRotatef(cam_theta, 0, 1, 0);
  glTranslatef(cam_pan[0], cam_pan[1], cam_pan[2]);

  glLightfv(GL_LIGHT0, GL_POSITION, lpos);

//  glPushMatrix();
//  if (anim)
//  {
//    tm = glutGet(GLUT_ELAPSED_TIME) - anim_start;
//    glRotatef(tm / 10.0f, 1, 0, 0);
//    glRotatef(tm / 10.0f, 0, 1, 0);
//  }
//  glutSolidTorus(0.3, 1, 16, 24);
//  glPopMatrix();

//  glutSolidSphere(0.4, 16, 8);
//
//  glPushMatrix();
//  glTranslatef(-2.5, 0, 0);
//  glutSolidCube(1.5);
//  glPopMatrix();
//
//  glPushMatrix();
//  glTranslatef(2.5, -1, 0);
//  glRotatef(-90, 1, 0, 0);
//  glutSolidCone(1.1, 2, 16, 2);
//  glPopMatrix();
//
//  glPushMatrix();
//  glTranslatef(0, -0.5, 2.5);
//  glFrontFace(GL_CW);
//  glutSolidTeapot(1.0);
//  glFrontFace(GL_CCW);
//  glPopMatrix();

  //Trying to draw my cone
  glPushMatrix();
  glTranslatef(translate_x, translate_y, translate_z);
  glRotatef(-90, 1, 0, 0);
  //rotating cone -180 degrees for task 2
  if(isTask2Active)
  {
    coneRotation_z -= 0.05;

    glRotatef(coneRotation_z, 0, 0, 1);

    if(coneRotation_z < -180)
    {
      coneRotation_z = 0;
      isTask2Active = false;
    }
  }

  glColor3f(0.8, 0.2, 0.1);
  glutWireCone(base, height, slices, stacks);
  glPopMatrix();

  //drawing a sphere
  glPushMatrix();
  glTranslatef(translate_x, translate_y, translate_z);
  glColor3f(0.8, 0.2, 0.1);
  if(isTask2Active)
  {
    if(sphereCurrentScaleFactor < sphereScaleFactor)
    {
      sphereCurrentScaleFactor += 0.001;
    }
    glScalef(sphereCurrentScaleFactor, sphereCurrentScaleFactor, sphereCurrentScaleFactor);
  }
  glutWireSphere(radius, slices, stacks);
  glPopMatrix();

  //drawing cylinder
  glPushMatrix();
  glTranslatef(0, 0, 0);
  glRotatef(-90, 1, 0, 0);
  glutSolidCylinder(1, cylinderHeight, 10, 10);
  glPopMatrix();

  //drawing teapot
  glPushMatrix();
  glTranslatef(0, cylinderHeight + teapotSize/2 , 0);
  glFrontFace(GL_CW);
  glutSolidTeapot(teapotSize);
  glFrontFace(GL_CCW);
  glPopMatrix();


  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-5, 0, 5);
  glVertex3f(5, 0, 5);
  glVertex3f(5, 0, -5);
  glVertex3f(-5, 0, -5);
  glEnd();

  print_help();

  glutSwapBuffers();
  nframes++;
}

void print_help(void)
{
  int i;
  const char *s, **text;

  glPushAttrib(GL_ENABLE_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, win_width, 0, win_height, -1, 1);

  text = help ? helptext : helpprompt;

  for (i = 0; text[i]; i++)
  {
    glColor3f(0, 0.1, 0);
    glRasterPos2f(7, win_height - (i + 1) * 20 - 2);
    s = text[i];
    while (*s)
    {
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s++);
    }
    glColor3f(0, 0.9, 0);
    glRasterPos2f(5, win_height - (i + 1) * 20);
    s = text[i];
    while (*s)
    {
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s++);
    }
  }

  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glPopAttrib();
}

#define ZNEAR  0.5f

void reshape(int x, int y)
{
  float vsz, aspect = (float) x / (float) y;
  win_width = x;
  win_height = y;

  glViewport(0, 0, x, y);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  vsz = 0.4663f * ZNEAR;
  glFrustum(-aspect * vsz, aspect * vsz, -vsz, vsz, 0.5, 500.0);
}

void keypress(unsigned char key, int x, int y)
{
  static int fullscr;
  static int prev_xsz, prev_ysz;

  std::cout << "Key " << key;

  switch (key)
  {
    case 27:
    case 'q':
      exit(0);
      break;

    case ' ':
      anim ^= 1;
      glutIdleFunc(anim ? idle : 0);
      glutPostRedisplay();

      if (anim)
      {
        anim_start = glutGet(GLUT_ELAPSED_TIME);
        nframes = 0;
      }
      else
      {
        long tm = glutGet(GLUT_ELAPSED_TIME) - anim_start;
        long fps = (nframes * 100000) / tm;
        printf("framerate: %ld.%ld fps\n", fps / 100, fps % 100);
      }
      break;

    case '\n':
    case '\r':
      if (!(glutGetModifiers() & GLUT_ACTIVE_ALT))
      {
        break;
      }
    case 'f':
      fullscr ^= 1;
      if (fullscr)
      {
        prev_xsz = glutGet(GLUT_WINDOW_WIDTH);
        prev_ysz = glutGet(GLUT_WINDOW_HEIGHT);
        glutFullScreen();
      }
      else
      {
        glutReshapeWindow(prev_xsz, prev_ysz);
      }
      break;
      //switch for 2nd task
    case '2':
      sphereCurrentScaleFactor = 1;
      coneRotation_z = 0;

      isTask2Active = !isTask2Active;
      std::cout << "Task 2 active " << isTask2Active;
      glutIdleFunc(isTask2Active ? idle : 0);
      glutPostRedisplay();
      break;

  }
}


void skeypress(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_F1:
      help ^= 1;
      glutPostRedisplay();

    default:
      break;
  }
}

void mouse(int bn, int st, int x, int y)
{
  int bidx = bn - GLUT_LEFT_BUTTON;
  bnstate[bidx] = st == GLUT_DOWN;
  mouse_x = x;
  mouse_y = y;
}

void motion(int x, int y)
{
  int dx = x - mouse_x;
  int dy = y - mouse_y;
  mouse_x = x;
  mouse_y = y;

  if (!(dx | dy))
  { return; }

  if (bnstate[0])
  {
    cam_theta += dx * 0.5;
    cam_phi += dy * 0.5;
    if (cam_phi < -90)
    { cam_phi = -90; }
    if (cam_phi > 90)
    { cam_phi = 90; }
    glutPostRedisplay();
  }
  if (bnstate[1])
  {
    float up[3], right[3];
    float theta = cam_theta * M_PI / 180.0f;
    float phi = cam_phi * M_PI / 180.0f;

    up[0] = -sin(theta) * sin(phi);
    up[1] = -cos(phi);
    up[2] = cos(theta) * sin(phi);
    right[0] = cos(theta);
    right[1] = 0;
    right[2] = sin(theta);

    cam_pan[0] += (right[0] * dx + up[0] * dy) * 0.01;
    cam_pan[1] += up[1] * dy * 0.01;
    cam_pan[2] += (right[2] * dx + up[2] * dy) * 0.01;
    glutPostRedisplay();
  }
  if (bnstate[2])
  {
    cam_dist += dy * 0.1;
    if (cam_dist < 0)
    { cam_dist = 0; }
    glutPostRedisplay();
  }
}
