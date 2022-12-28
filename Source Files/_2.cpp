#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <iostream>

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

void init();
void exit();
void idleCallback();
void keyPress(int key, int x, int y);
void mouseMotionCallback(int x, int y);
void mouseCallback(int button, int state, int x, int y);

RenderState rs;
double rotationValue = 0;

float redValue = 0.7f;
float greenValue = 0.7f;
float blueValue = 0.7f;
float alphaValue = 1.0f;

float scaleRatio = 1;
float trRatio = 0.0;
GLfloat xRotated, yRotated, zRotated;

struct Image
{
  unsigned long sizeX;
  unsigned long sizeY;
  char* data;
};
GLuint textureid;
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

Image* loadTexture() {
  Image* image1;
  image1 = (Image*)malloc(sizeof(Image));
  if (image1 == NULL) {
    printf("Error allocating space for image");
    exit(0);
  }
  if (!ImageLoad("D:\\University\\ComputerGraphics\\Source Files\\1.bmp", image1))
  {
    exit(1);
  }
  return image1;
}

void displaySolid(void)
{

  GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  GLfloat mat_diffuse[] = { .5f, .5f, .5f, 1.0f };
  GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat mat_shininess[] = { 50.0f };
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


  GLfloat lightIntensity[] = { redValue, greenValue, blueValue, alphaValue };
  GLfloat light_position[] = { 2.0f, 6.0f, 3.0f, 0.0f };

  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);

  //camera
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double winHt = 1.0;           //half-height of window
  glOrtho(-winHt * 64 / 48.0, winHt * 64 / 48.0, -winHt, winHt, -0.1, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
//  gluLookAt(2.0, 1.3, 2.0, 0.0, 0.25, 0.0, 0.0, 1.0, 0.0);

  //start drawing
  /*glPushMatrix();

  glTranslated(0.5, 0.40, 0.5);
  glutWireCube(1.0);
  glRotatef(xRotated, 1.0, 0.0, 0.0);
  glRotatef(yRotated, 0.0, 1.0, 0.0);
  glutSolidTeapot(0.15);
  //glutSolidCone(0.15, 0.15, 50, 50);
  */

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslatef(0, 0, -5.0f);
  glRotatef(rs.cameraX, 1, 0, 0);
  glRotatef(rs.cameraY, 0, 1, 0);

  glPushMatrix();
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glTranslatef(1.0f, 0, 0.0);

  glBindTexture(GL_TEXTURE_2D, textureid);
  glutSolidTeapot(0.15);
  //glutSolidCone(0.15, 0.15, 50, 50);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopMatrix();

  glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
  glutSolidSphere(0.2, 100, 100);

  glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
  glRotatef(-90, 1, 0, 0);
  glTranslatef(0, 0, -0.5);
  glutSolidCone(1, 2, 100.0, 100.0);
  glRotatef(yRotated, 0.0, 1.0, 0.0);

  glFlush();
}

void idle(void) {
  xRotated += 0.3;
  yRotated += 0.3;
  zRotated += 0.3;
  displaySolid();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(960, 728);

  xRotated = yRotated = zRotated = 30.0;
  xRotated = 43;
  yRotated = 50;

  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(displaySolid);
  glutIdleFunc(idle);
  glutMouseFunc(mouseCallback);
  glutMotionFunc(mouseMotionCallback);
  glutSpecialFunc(keyPress);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, (GLsizei)500, (GLsizei)500);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 500.0);

  glEnable(GL_LIGHTING);//lighting effect
  glEnable(GL_LIGHT0);//enabling 1 light effect
  glEnable(GL_DEPTH_TEST);//3d test//F B
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);
  glutMainLoop();
  exit();
  return 0;
}

void init() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  Image* image_smile = loadTexture();
  if (image_smile == 0)
  {
    printf("failed to load texture!\n");
    exit(1);
  }

  glGenTextures(1, &textureid);
  glBindTexture(GL_TEXTURE_2D, textureid);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_smile->sizeX, image_smile->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image_smile->data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  //auto* rsp = new RenderState();
  //rs = *rsp;
}

void exit() {
  //delete& rs;
}

void mouseCallback(int button, int state, int x, int y) {
  rs.mouseX = x;
  rs.mouseY = y;

  if (button == GLUT_LEFT_BUTTON)
  {
    if (state == GLUT_DOWN)
    {
      rs.mouseLeftDown = true;
    }
    else if (state == GLUT_UP)
      rs.mouseLeftDown = false;
  }
}

void mouseMotionCallback(int x, int y) {
  if (rs.mouseLeftDown)
  {
    rs.cameraY += (x - rs.mouseX);
    rs.cameraX += (y - rs.mouseY);
    rs.mouseX = x;
    rs.mouseY = y;
  }
}

void idleCallback() {
  glutPostRedisplay();
}

void keyPress(int key, int x, int y) {
  if (key == GLUT_KEY_UP) {
    redValue += 0.3;
    greenValue += 0.3;
    blueValue += 0.3;
    alphaValue += 0.3;
  }
  else if (key == GLUT_KEY_DOWN) {
    redValue -= 0.3;
    greenValue -= 0.3;
    blueValue -= 0.3;
    alphaValue -= 0.3;
  }
  else if (key == GLUT_KEY_RIGHT) {
    redValue += 1;
    greenValue += 0.5;
    alphaValue -= 3;
  }
  else if (key == GLUT_KEY_LEFT) {
    redValue -= 1;
    greenValue -= 0.5;
    alphaValue += 3;
  }
  glutPostRedisplay();
}
