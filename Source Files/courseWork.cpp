#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <iostream>
#include <valarray>
#include <vector>
#include <cmath>

class RenderState
{
public:
  float mouseX, mouseY, cameraX, cameraY;
  bool mouseLeftDown, mouseRightDown;

  RenderState()
  {
    this->mouseX = 0;
    this->mouseY = 0;
    this->mouseLeftDown = false;
    this->mouseRightDown = false;
    this->cameraX = 0.0f;
    this->cameraY = 0.0f;
  }
};

float cam_theta, cam_phi = 25, cam_dist = 8;
float cam_pan[3];
int mouse_x, mouse_y;
int bnstate[8];
int anim, help;
long anim_start;
long nframes;

void init();

void exit();

void idleCallback();

void keyPress(int key, int x, int y);

void mouse(int bn, int st, int x, int y);

void motion(int x, int y);

RenderState rs;
double rotationValue = 0;

float redValue = 1.0f;
float greenValue = 1.0f;
float blueValue = 1.0f;
float alphaValue = 1.0f;

float scaleRatio = 1;
float trRatio = 0.0;
GLfloat xRotated, yRotated, zRotated;

float cubeHeight = 1.5f;

float coneHeight = 3.0f;
float coneBase = 1.0f;

struct Vector3
{
public:
  float x;
  float y;
  float z;

  Vector3(float x, float y, float z) : x(x), y(y), z(z)
  { }

  bool operator<(Vector3 lhs) const
  {
    return x < lhs.x && y < lhs.y && z < lhs.z;
  }

  bool operator>(Vector3 lhs)
  {
    return x > lhs.x && y > lhs.y && z > lhs.z;
  }

  Vector3 ModDifference(Vector3 lhs)
  {
    return Vector3(std::abs(x) - std::abs(lhs.x), std::abs(y) - std::abs(lhs.y), std::abs(z) - std::abs(lhs.z));
  }

};

struct Transform
{
public:
  Vector3 Position;
  Vector3 Rotation;
};

int currentPointID = 0;
bool moveToNextPoint = false;

std::vector<Transform> fallAnimationPoints = {
  {
    {0,    0.8,  -2.4},
    {-30, 0,   0}
  },
  {
    {-0.9, 0.8,  -2.4},
    {-36, 0,   20}
  },
//  {
//    {-1.7, 1,  -2.3},
//    {10, -40,   35}
//  },
//  {
//    {-1.4, 0.85, -2.4},
//    {-33, -10, 400}
//  },
  {
    {-1.8, 1,    -2.3},
    {28,  -46, 40}
  },
  {
    {-1.8, 1,    -2.3},
    {28,  -50, 20}
  },
  {
    {-2,   1,    -2.9},
    {28,  -55, 0}
  }
//  {
//    {3.296, 0.511, 1.618},
//    {10.683, -38.192, -157.66}
//  },
//  {
//    {2.43000007,0.409999996,0.449999988},
//    {-38.23, 82.547, -33.677}
//  },
//  {
//    {2.43000007,0.409999996,0.449999988},
//    {-27.771, -66.722, -42.371}
//  },
//  {
//    {2.43000007,0.5,0.699999988},
//    {-13.843, -51.955, -64.485}
//  },
//  {
//    {2.51999998,0.479999989,0.819999993},
//    {9.29, -52.675, -46.864}
//  },
//  {
//    {2.88, 0.55, 1.3},
//    {10.643, -38.192, -94.291}
//  },
//  {
//    {3.296, 0.511, 1.618},
//    {10.683, -38.192, -157.66}
//  }
};

Transform coneTransform = fallAnimationPoints[0];

GLfloat translate_x = -1.5;
GLfloat translate_y = 1;
GLfloat translate_z = -2.3;

int waitTime = 500;
int currentWait = 0;

struct Image
{
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
};
GLuint textureid;
typedef struct Image Image;

int ImageLoad(const char *filename, Image *image)
{
  FILE *file;
  unsigned long size; // size of the image in bytes.
  unsigned long i; // standard counter.
  unsigned short int planes; // number of planes in image (must be 1)
  unsigned short int bpp; // number of bits per pixel (must be 24)
  char temp; // temporary color storage for bgr-rgb conversion.
  // make sure the file is there.
  if ((file = fopen(filename, "rb")) == NULL)
  {
    printf("File Not Found : %s\n", filename);
    return 0;
  }
  // seek through the bmp header, up to the width/height:
  fseek(file, 18, SEEK_CUR);
  // read the width
  if ((i = fread(&image->sizeX, 4, 1, file)) != 1)
  {
    printf("Error reading width from %s.\n", filename);
    return 0;
  }
  //printf("Width of %s: %lu\n", filename, image->sizeX);
  // read the height
  if ((i = fread(&image->sizeY, 4, 1, file)) != 1)
  {
    printf("Error reading height from %s.\n", filename);
    return 0;
  }
  //printf("Height of %s: %lu\n", filename, image->sizeY);
  // calculate the size (assuming 24 bits or 3 bytes per pixel).
  size = image->sizeX * image->sizeY * 3;
  // read the planes
  if ((fread(&planes, 2, 1, file)) != 1)
  {
    printf("Error reading planes from %s.\n", filename);
    return 0;
  }
  if (planes != 1)
  {
    printf("Planes from %s is not 1: %u\n", filename, planes);
    return 0;
  }
  // read the bitsperpixel
  if ((i = fread(&bpp, 2, 1, file)) != 1)
  {
    printf("Error reading bpp from %s.\n", filename);
    return 0;
  }
  if (bpp != 24)
  {
    printf("Bpp from %s is not 24: %u\n", filename, bpp);
    return 0;
  }
  // seek past the rest of the bitmap header.
  fseek(file, 24, SEEK_CUR);
  // read the data.
  image->data = (char *) malloc(size);
  if (image->data == NULL)
  {
    printf("Error allocating memory for color-corrected image data");
    return 0;
  }
  if ((i = fread(image->data, size, 1, file)) != 1)
  {
    printf("Error reading image data from %s.\n", filename);
    return 0;
  }
  for (i = 0; i < size; i += 3)
  { // reverse all of the colors. (bgr -> rgb)
    temp = image->data[i];
    image->data[i] = image->data[i + 2];
    image->data[i + 2] = temp;
  }
  // we're done.
  return 1;
}

Image *loadTexture()
{
  Image *image1;
  image1 = (Image *) malloc(sizeof(Image));
  if (image1 == NULL)
  {
    printf("Error allocating space for image");
    exit(0);
  }
  if (!ImageLoad("D:\\University\\ComputerGraphics\\Source Files\\1.bmp", image1))
  {
    exit(1);
  }
  return image1;
}

float lerp(float initial, float final, float step)
{
  //return lerp(initial, final, step);
  if(step >= 1)
  {
    step = currentPointID == 1 ? 0.1 :0.5;
  }

  float result = 0;
  float equalizer = 0;

  if(initial <= 0 && final >= 0)
  {
    equalizer = initial*-1;
    initial = 0;
    final += equalizer;

    result = initial * (1.0 - step) + (final * step);
    return result - equalizer;
  }

  if(initial >= 0 && final <= 0)
  {
    equalizer = final *-1;
    final = 0;
    initial += equalizer;

    result = initial * (1.0 - step) + (final * step);
    return result - equalizer;
  }


  return initial * (1.0 - step) + (final * step);
}

float clamp(float lower, float n , float upper)
{
  return n <= lower ? lower : n >= upper ? upper : n;
}

void moveTowards(const Vector3& initial, Vector3& current, const Vector3& final, float error = 0.05)
{
  float percent = currentPointID == 1 ? 0.01 :0.01;
  //float step = 0.05;
  // float step = abs(abs(initial)- abs(final))* percent;


  if (initial.x != final.x  )//&& abs(current.x - final.x) > error)
  {
    current.x = lerp(current.x, final.x, abs(abs(initial.x) - abs(final.x)) * percent );
   // current.x = clamp(initial.x, current.x, final.x);

  }

  if (initial.y != final.y  )//&& abs(current.y - final.y) > error )
  {
    current.y = lerp(current.y, final.y, abs(abs(initial.y) - abs(final.y)) * percent );
  //  current.y = clamp(initial.y, current.y, final.y);
  }

  if (initial.z != final.z )//&&  abs(current.z - final.z) > error )
  {
    current.z = lerp(current.z, final.z, abs(abs(initial.z) - abs(final.z)) * percent );
   // current.x = clamp(initial.z, current.z, final.z);
  }

//  if(initial.x < final.x && current.x < final.x)
//  {
//    current.x += (final.x-initial.x) * percent + step;
//  }
//  else if(current.x > final.x)
//  {
//    current.x -= (initial.x - final.x) * percent + step;
//  }
//
//  if(initial.y < final.y && current.y < final.y)
//  {
//    current.y += (final.y-initial.y) * percent + step;
//  }
//  else if(current.y > final.y)
//  {
//    current.y -= (initial.y - final.y) * percent + step;
//  }
//
//  if(initial.z < final.z && current.z < final.z)
//  {
//    current.z += (final.z-initial.z) * percent + step;
//  }
//  else if(current.z > final.z)
//  {
//    current.z -= (initial.z - final.z) * percent + step;
//  }

}

void moveTowards(const Transform& initial, Transform& current, const Transform& final)
{
  moveTowards(initial.Position, current.Position, final.Position);
  moveTowards(initial.Rotation, current.Rotation, final.Rotation);
}

bool reachedFinal(const float& initial, const float& current, const float& final, const float error = 0.1)
{
  if (initial < final && current < final - error)
  {
    return false;
  }

  if (initial > final && current > final + error)
  {
    return false;
  }

  return true;
}

bool canMoveToNextPoint(const Vector3& initial, const Vector3& current, const Vector3& final)
{
  return reachedFinal(initial.x, current.x, final.x) &&
         reachedFinal(initial.y, current.y, final.y) &&
         reachedFinal(initial.z, current.z, final.z);

}

bool canMoveToNextPoint(const Transform& initial, const Transform& current, const Transform& final)
{
  //return   canMoveToNextPoint(initial.Rotation, current.Rotation, final.Rotation);
  return canMoveToNextPoint(initial.Position, current.Position, final.Position) &&
         canMoveToNextPoint(initial.Rotation, current.Rotation, final.Rotation);
}

void displaySolid(void)
{

  GLfloat mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
  GLfloat mat_diffuse[] = {.5f, .5f, .5f, 1.0f};
  GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat mat_shininess[] = {50.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

  GLfloat lightIntensity[] = {redValue, greenValue, blueValue, alphaValue};
  GLfloat light_position[] = {0.0f, 3.0f, 3.0f, 0.0f};

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

  //float lpos[] = {-1, 2, 3, 0};

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -cam_dist);
  glRotatef(cam_phi, 1, 0, 0);
  glRotatef(cam_theta, 0, 1, 0);
  glTranslatef(cam_pan[0], cam_pan[1], cam_pan[2]);

  // glLightfv(GL_LIGHT0, GL_POSITION, lpos);

  //start drawing
  /*glPushMatrix();

  glTranslated(0.5, 0.40, 0.5);
  glutWireCube(1.0);
  glRotatef(xRotated, 1.0, 0.0, 0.0);
  glRotatef(yRotated, 0.0, 1.0, 0.0);
  glutSolidTeapot(0.15);
  //glutSolidCone(0.15, 0.15, 50, 50);
  */

//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glLoadIdentity();
//  glTranslatef(0, 0, -10.0f);
//  glTranslatef(0, 10, 0);
//  glRotatef(rs.cameraX, 1, 0, 0);
//  glRotatef(rs.cameraY, 0, 1, 0);

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  glVertex3f(-5, 0, 5);
  glVertex3f(5, 0, 5);
  glVertex3f(5, 0, -5);
  glVertex3f(-5, 0, -5);
  glEnd();

//drawing cube
  glPushMatrix();
  glColor4f(0.6, 0, 0, 1);
  glTranslatef(0, cubeHeight / 2, 0.0);
  glutSolidCube(cubeHeight);
  glPopMatrix();

  //drawing cone
  glPushMatrix();
  glColor4f(0.5, 0.5, 0.5, 1);

  if (moveToNextPoint)
  {
    if (currentPointID >= fallAnimationPoints.size())
    {
      currentWait++;

      if (currentWait > waitTime)
      {
        currentWait = 0;


        moveToNextPoint = false;
      }
      currentPointID = 0;

      coneTransform = fallAnimationPoints[currentPointID];
    }
    else
    {

      currentPointID++;


      coneTransform = fallAnimationPoints[currentPointID];
      moveToNextPoint = false;
    }

  }

  // glTranslatef( cubeHeight + coneBase, 0, 0.0);

//  glRotatef(-90, 1, 1, 0);
//  glRotatef(180, 0, 0, 1);

  // glTranslatef(translate_x, translate_y, translate_z);
//  glTranslatef(-0.8, 1, -2.3);
//
//
//  glRotatef(translate_x, 1, 0, 0);
//  glRotatef(translate_y, 0, 1, 0);
//  glRotatef(translate_z, 0, 0, 1);

//  glRotatef(28, 1, 0, 0);
//  glRotatef(-46, 0, 1, 0);
//  glRotatef(40, 0, 0, 1);

//  glTranslatef(0, 0, 3);
  //glRotatef(180, 1, 0, 0);
  //glRotatef(180, 0, 0, 1);
//
//  glRotatef(180, 0, 1, 0);
//
// glTranslatef(0.357, 2.513 , 0);

  //glRotatef(0, 1, 0, 0);
  //glRotatef(0, 0, 1, 0);
//  glRotatef(53, 1, 0, 0);


  //glTranslatef(0,0,0);
  glTranslatef(coneTransform.Position.x, coneTransform.Position.y, coneTransform.Position.z);
  glRotatef(coneTransform.Rotation.x, 1, 0, 0);
  glRotatef(coneTransform.Rotation.y, 0, 1, 0);
  glRotatef(coneTransform.Rotation.z, 0, 0, 1);

  std::cout << "Position: ( " << coneTransform.Position.x << " " << coneTransform.Position.y << " "
            << coneTransform.Position.z << ") \nrotation: ( " << coneTransform.Rotation.x << " "
            << coneTransform.Rotation.y << " " << coneTransform.Rotation.z << ") \n\n";

  if (currentPointID == fallAnimationPoints.size() - 1)
  {
    moveToNextPoint = true;
  }
  else
  {
    moveTowards(fallAnimationPoints[currentPointID], coneTransform, fallAnimationPoints[currentPointID + 1]);
    moveToNextPoint = canMoveToNextPoint(fallAnimationPoints[currentPointID], coneTransform,
                                         fallAnimationPoints[currentPointID + 1]);
  }

  glutSolidCone(coneBase, coneHeight, 50, 50);
  glColor4f(1, 1, 1, 1);
  glPopMatrix();

  glFlush();
}

void idle(void)
{
  xRotated += 0.3;
  yRotated += 0.3;
  zRotated += 0.3;
  displaySolid();
}

int main(int argc, char **argv)
{
//  for(int i =0; i < fallAnimationPoints.size()-1; i++)
//  {
//    Vector3 initialPos = fallAnimationPoints[i].Rotation;
//    Vector3 finalPos = fallAnimationPoints[i+1].Rotation;
//
//    Vector3 diff = Vector3(initialPos.x - finalPos.x, initialPos.y - finalPos.y, initialPos.z - finalPos.z);
//
//    std::cout << "Initial: ( " << initialPos.x << " " << initialPos.y << " " << initialPos.z << ") \ndiff: ( " << diff.x << " " << diff.y << " " << diff.z << " ) \nfinal: ( "<< finalPos.x << " " << finalPos.y << " " << finalPos.z << ") \n\n\n";
//  }

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
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutSpecialFunc(keyPress);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, (GLsizei) 500, (GLsizei) 500);
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

void init()
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  Image *image_smile = loadTexture();
  if (image_smile == 0)
  {
    printf("failed to load texture!\n");
    exit(1);
  }

  glGenTextures(1, &textureid);
  glBindTexture(GL_TEXTURE_2D, textureid);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, image_smile->sizeX, image_smile->sizeY, 0, GL_RGB8, GL_UNSIGNED_BYTE,
               image_smile->data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  //auto* rsp = new RenderState();
  //rs = *rsp;
}

void exit()
{
  //delete& rs;
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

void idleCallback()
{
  glutPostRedisplay();
}

void keyPress(int key, int x, int y)
{
  if (key == GLUT_KEY_UP)
  {
    translate_x += 0.1;
    std::cout << translate_x << " " << translate_y << " " << translate_z;
    std::cout << "\n";
  }

  if (key == GLUT_KEY_RIGHT)
  {
    translate_y += 0.1;
    std::cout << translate_x << " " << translate_y << " " << translate_z;
    std::cout << "\n";

  }

  if (key == GLUT_KEY_PAGE_UP)
  {
    translate_z += 0.1;
    std::cout << translate_x << " " << translate_y << " " << translate_z;
    std::cout << "\n";

  }

  if (key == GLUT_KEY_DOWN)
  {
    translate_x -= 0.1;
    std::cout << translate_x << " " << translate_y << " " << translate_z;
    std::cout << "\n";

  }

  if (key == GLUT_KEY_LEFT)
  {
    translate_y -= 0.1;
    std::cout << translate_x << " " << translate_y << " " << translate_z;
    std::cout << "\n";

  }

  if (key == GLUT_KEY_PAGE_DOWN)
  {
    translate_z -= 0.1;
    std::cout << translate_x << " " << translate_y << " " << translate_z;
    std::cout << "\n";

  }


//  if (key == GLUT_KEY_UP)
//  {
//    redValue += 0.3;
//    greenValue += 0.3;
//    blueValue += 0.3;
//    alphaValue += 0.3;
//  }
//  else if (key == GLUT_KEY_DOWN)
//  {
//    redValue -= 0.3;
//    greenValue -= 0.3;
//    blueValue -= 0.3;
//    alphaValue -= 0.3;
//  }
//  else if (key == GLUT_KEY_RIGHT)
//  {
//    redValue += 1;
//    greenValue += 0.5;
//    alphaValue -= 3;
//  }
//  else if (key == GLUT_KEY_LEFT)
//  {
//    redValue -= 1;
//    greenValue -= 0.5;
//    alphaValue += 3;
//  }
  glutPostRedisplay();
}
