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

float redValue = 0.7f;
float greenValue = 0.7f;
float blueValue = 0.7f;
float alphaValue = 1.0f;

class RenderState {
public:
  float mouseX, mouseY, cameraX, cameraY;
  bool mouseLeftDown, mouseRightDown;

  RenderState() {
    this->mouseX = 0;
    this->mouseY = 0;
    this->mouseLeftDown = false;
    this->mouseRightDown = false;
    this->cameraX = 0.0f;
    this->cameraY = 0.0f;
  }
};

RenderState rs;
double rotationValue = 0;

struct Image
{
  unsigned long sizeX;
  unsigned long sizeY;
  char* data;
};
GLuint textureid;
GLuint textureid_2;
typedef struct Image Image;


int ImageLoad(const char* filename, Image* image) {
  FILE* file;
  unsigned long size; // size of the image in bytes.
  unsigned long i; // standard counter.
  unsigned short int planes; // number of planes in image (must be 1)
  unsigned short int bpp; // number of bits per pixel (must be 24)
  char temp; // temporary color storage for bgr-rgb conversion.
  // make sure the file is there.
  if ((file = fopen(filename, "rb")) == NULL) {
    printf("File Not Found : %s\n", filename);
    return 0;
  }
  // seek through the bmp header, up to the width/height:
  fseek(file, 18, SEEK_CUR);
  // read the width
  if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
    printf("Error reading width from %s.\n", filename);
    return 0;
  }
  //printf("Width of %s: %lu\n", filename, image->sizeX);
  // read the height
  if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
    printf("Error reading height from %s.\n", filename);
    return 0;
  }
  //printf("Height of %s: %lu\n", filename, image->sizeY);
  // calculate the size (assuming 24 bits or 3 bytes per pixel).
  size = image->sizeX * image->sizeY * 3;
  // read the planes
  if ((fread(&planes, 2, 1, file)) != 1) {
    printf("Error reading planes from %s.\n", filename);
    return 0;
  }
  if (planes != 1) {
    printf("Planes from %s is not 1: %u\n", filename, planes);
    return 0;
  }
  // read the bitsperpixel
  if ((i = fread(&bpp, 2, 1, file)) != 1) {
    printf("Error reading bpp from %s.\n", filename);
    return 0;
  }
  if (bpp != 24) {
    printf("Bpp from %s is not 24: %u\n", filename, bpp);
    return 0;
  }
  // seek past the rest of the bitmap header.
  fseek(file, 24, SEEK_CUR);
  // read the data.
  image->data = (char*)malloc(size);
  if (image->data == NULL) {
    printf("Error allocating memory for color-corrected image data");
    return 0;
  }
  if ((i = fread(image->data, size, 1, file)) != 1) {
    printf("Error reading image data from %s.\n", filename);
    return 0;
  }
  for (i = 0; i < size; i += 3) { // reverse all of the colors. (bgr -> rgb)
    temp = image->data[i];
    image->data[i] = image->data[i + 2];
    image->data[i + 2] = temp;
  }
  // we're done.
  return 1;
}

Image* loadTexture(const char* filename) {
  Image* image1;
  image1 = (Image*)malloc(sizeof(Image));
  if (image1 == NULL) {
    printf("Error allocating space for image");
    exit(0);
  }
  if (!ImageLoad(filename, image1))
  {
    exit(1);
  }
  return image1;
}


void init() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  Image* image_smile = loadTexture("D:\\University\\ComputerGraphics\\Source Files\\1.bmp");
  if (image_smile == 0)
  {
    printf("failed to load texture!\n");
    exit(1);
  }
  Image* image_2 = loadTexture("D:\\University\\ComputerGraphics\\Source Files\\2.bmp");
  if (image_2 == 0)
  {
    printf("failed to load texture!\n");
    exit(1);
  }
  glGenTextures(1, &textureid);
  glBindTexture(GL_TEXTURE_2D, textureid);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_smile->sizeX, image_smile->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image_smile->data);
  glTexImage2D(GL_TEXTURE_2D, 1, GL_RGB, image_2->sizeX, image_2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image_2->data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//  glClearColor(0.0, 0.0, 0.0, 0.0);
//
//
//
//  glGenTextures(2, &textureid);
//  glBindTexture(GL_TEXTURE_2D, textureid);
//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_2->sizeX, image_2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image_2->data);
//  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  RenderState* rsp = new RenderState();
  rs = *rsp;
}


int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("freeglut 3D view demo");

  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keypress);
  glutSpecialFunc(skeypress);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glEnable(GL_LIGHTING);//lighting effect
  glEnable(GL_LIGHT0);//enabling 1 light effect
  glEnable(GL_DEPTH_TEST);//3d test//F B
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);
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


//TODO: все предметы рядом на плоскости. Чайник снять с цилиндра. Источник света перемещать (влево-вправо например). Перекрашивать свет в разные цвета
void display(void)
{
  long tm;
  float lpos[] = {-1, 2, 3, 0};

  GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  GLfloat mat_diffuse[] = { .5f, .5f, .5f, 1.0f };
  GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat mat_shininess[] = { 70.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  GLfloat lightIntensity[] = { redValue, greenValue, blueValue, alphaValue };
  GLfloat light_position[] = { 2.0f, 6.0f, 3.0f, 0.0f };

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);

  //camera
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();
//  double winHt = 1.0;           //half-height of window
//  glOrtho(-winHt * 64 / 48.0, winHt * 64 / 48.0, -winHt, winHt, -0.1, 10.0);
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();
//  gluLookAt(2.0, 1.3, 2.0, 0.0, 0.25, 0.0, 0.0, 1.0, 0.0);

  //

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -cam_dist);
  glRotatef(cam_phi, 1, 0, 0);
  glRotatef(cam_theta, 0, 1, 0);
  glTranslatef(cam_pan[0], cam_pan[1], cam_pan[2]);

//  glLightfv(GL_LIGHT0, GL_POSITION, lpos);

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
//  glPushMatrix();
//  glTranslatef(translate_x, translate_y, translate_z);
//  glRotatef(-90, 1, 0, 0);
//  glColor3f(0.8, 0.2, 0.1);
//  //rotating cone -180 degrees for task 2
//  if(isTask2Active)
//  {
//    coneRotation_z -= 0.05;
//
//    glRotatef(coneRotation_z, 0, 0, 1);
//
//    if(coneRotation_z < -180)
//    {
//      coneRotation_z = 0;
//      isTask2Active = false;
//    }
//  }
//
// // glColor3f(0.8, 0.2, 0.1);
//  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//  glutWireCone(base, height, slices, stacks);
//  glPopMatrix();
//
//  //drawing a sphere
//  glPushMatrix();
//  glTranslatef(translate_x, translate_y, translate_z);
// // glColor3f(0.8, 0.2, 0.1);
//  glColor4f(1.0f, 1.0f, 1.0f, .60f);
//  if(isTask2Active)
//  {
//    if(sphereCurrentScaleFactor < sphereScaleFactor)
//    {
//      sphereCurrentScaleFactor += 0.001;
//    }
//    glScalef(sphereCurrentScaleFactor, sphereCurrentScaleFactor, sphereCurrentScaleFactor);
//  }
//  glutSolidSphere(radius, slices, stacks);
//  //glutWireSphere(radius, slices, stacks);
//  glPopMatrix();

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  //drawing cylinder
  glPushMatrix();
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glTranslatef(0, 0, 0);
  glBindTexture(GL_TEXTURE_2D, textureid);
  glRotatef(-90, 1, 0, 0);
//  glFrontFace(GL_CW);
  glutSolidCylinder(1, cylinderHeight, 10, 10);
  //glFrontFace(GL_CCW);
  glBindTexture(GL_TEXTURE_2D, 0);

  glPopMatrix();





  //drawing teapot
  glPushMatrix();
//  GLfloat mat_teapot_ambient[] = { 0.4f, 0.4f, 0.4f, .4f };
//  GLfloat mat_teapot_diffuse[] = { .5f, .5f, .5f, 1.0f };
//  GLfloat mat_teapot_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//  GLfloat mat_teapot_shininess[] = { 90.0f };
//  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_teapot_ambient);
//  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_teapot_diffuse);
//  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_teapot_specular);
//  glMaterialfv(GL_FRONT, GL_SHININESS, mat_teapot_shininess);

  glTranslatef(1, 0.5 , 0);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glColor3f(.0f, .86f, .86f);

  glFrontFace(GL_CW);
  glBindTexture(GL_TEXTURE_2D, textureid);

  glutSolidTeapot(teapotSize);
  glBindTexture(GL_TEXTURE_2D, 0);

  glFrontFace(GL_CCW);
  glPopMatrix();

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glBindTexture(GL_TEXTURE_2D, 2);
  glFrontFace(GL_CW);

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-5, 0, 5);
  glVertex3f(5, 0, 5);
  glVertex3f(5, 0, -5);
  glVertex3f(-5, 0, -5);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);

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
