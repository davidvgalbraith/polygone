#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string.h>
#define PI 3.14159265  // Should be used from mathlib
#define randy ((float) (rand() % 1000000000) / 10000000000000)
//#define randy 0

using namespace std;
//camera crap copeid from Rob Bateman http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/opengl_programming.html#4
inline float sqr(float x) { return x*x; }
class Viewport;
class Viewport {
public:
  int w, h; // width and height
};
int vertex, drawn;
//Holds the patches
vector<vector<vector<vector<float> > > > patches;
//What superpowers?
int flat = 0;
float zoom = 0.1f;
float rotx = 0;
float roty = 0.001f;
float tx = 0;
float ty = 0;
int lastx=0;
int lasty=0;
int wireframe = 0;
int adaptive;

float lookatx = 0.0;
float lookaty = 0.0;
float lookatz = 0.0;
float camerax;
float cameray;
float cameraz;
//uniform shading step
float step;
//useless crap
Viewport viewport;
vector<float> pluss(vector<float> x, vector<float> y);
vector<float> times(vector<float> x, float y);
vector<float> minuss(vector<float> x, vector<float> y);
vector<float> cross(vector<float> u, vector<float> v);
vector<vector<float> >beezerpatch(vector<vector<vector<float> > > patch, float u, float v);
void printvect(vector<float> x);
void printvectvect(vector<vector<float> > x);
void printvectarray(vector<vector<vector<float> > > x);
vector<float> normalize(vector<float> v);
GLfloat* floady(vector<float> x);
int isEqual(char *s, char *t, int len);
void keyPressed (unsigned char key, int x, int y);
void arrows(int key, int x, int y);
vector<vector<vector<float> > > triangulate(vector<vector<vector<float> > > triangle);

//****************************************************
// reshape viewport if the window is resized
//****************************************************

void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;
  glViewport (0,0,viewport.w,viewport.h);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glutPostRedisplay();

}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
float colorx = 1.5;
float colory = -1.5;
float colorz = 0.5;

void uniformDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();
  //gluLookAt(camerax, cameray, cameraz, lookatx, lookaty, lookatz, 0, 0, 1);
  gluLookAt(camerax, cameray, cameraz, 0, 0, 0, 0, 0, 1);
  //glLoadIdentity();

  /*glTranslatef(0,-zoom,0);*/
  glTranslatef(tx,0,ty);
  glRotatef(rotx,0,0,1);
  glRotatef(roty,1,0,0);
  // Start drawing
  if (flat) {
    glShadeModel(GL_FLAT);
  } else {
    glShadeModel(GL_SMOOTH);
    //copied from http://www.cs.uml.edu/~haim/teaching/cg/resources/presentations/427/AngelCG20_shading_OpenGL.pdf.
    GLfloat diffuse0[]={1.0, 0.0, 0.0, 1.0};
    GLfloat ambient0[]={0.0, 0.0, 0.0, 1.0};
    GLfloat specular0[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light0_pos[]={colorx, colory, colorz, 1.0};
    GLfloat diffuse1[]={1.0, 0.0, 0.0, 1.0};
    GLfloat ambient1[]={0.0, 0.0, 0.0, 1.0};
    GLfloat specular1[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light1_pos[]={camerax, cameray, cameraz, 1.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    GLfloat ambient[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[] = {1.0, 0.8, 0.0, 1.0};
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shine = 100.0;
    /*glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine); */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shine); 
  }
  glPointSize(10.0f);
  glBegin(GL_POINTS);
  glVertex3f(colorx, colory, colorz);
  glEnd();
  for (int patch = 0; patch < patches.size(); patch++) {
    //for (int patch = 0; patch < 1; patch++) {
    vector<vector<vector<float> > > currPatch = patches[patch];
    int numdiv = (1 + step/10) / step;
    for (int iu = 0; iu < numdiv; iu++) {
      float u = iu * step;
      for (int iv = 0; iv < numdiv; iv++) {
	float v = iv * step;
	vector<vector<float> >interp1 = beezerpatch(currPatch, u, v);
	vector<vector<float> >interp2 = beezerpatch(currPatch, u+step, v);
	vector<vector<float> >interp3 = beezerpatch(currPatch, u, v+step);
	vector<vector<float> >interp4 = beezerpatch(currPatch, u+step, v+step);
	if (wireframe) {
	  glBegin(GL_LINES);
	  glNormal3fv(floady(interp1[1]));
	  glVertex3fv(floady(interp1[0]));
	  glNormal3fv(floady(interp3[1]));
	  glVertex3fv(floady(interp3[0]));
	  glNormal3fv(floady(interp2[1]));
	  glVertex3fv(floady(interp2[0]));
	  glNormal3fv(floady(interp4[1]));
	  glVertex3fv(floady(interp4[0]));
	} else {
	  glBegin(GL_QUADS);
	}
	glNormal3fv(floady(interp1[1]));
	glVertex3fv(floady(interp1[0]));
	glNormal3fv(floady(interp2[1]));
	glVertex3fv(floady(interp2[0]));
	glNormal3fv(floady(interp4[1]));
	glVertex3fv(floady(interp4[0]));
	glNormal3fv(floady(interp3[1]));
	glVertex3fv(floady(interp3[0]));
	glEnd();
	/*glBegin(GL_LINES);
	  glVertex3fv(floady(interp1[0]));
	  glVertex3fv(floady(pluss(interp1[0], times(interp1[1], 0.15))));
	  glVertex3fv(floady(interp2[0]));
	  glVertex3fv(floady(pluss(interp2[0], times(interp2[1], 0.15))));
	  glVertex3fv(floady(interp3[0]));
	  glVertex3fv(floady(pluss(interp3[0], times(interp3[1], 0.15))));	
	  glVertex3fv(floady(interp4[0]));
	  glVertex3fv(floady(pluss(interp4[0], times(interp4[1], 0.15))));
	  glEnd();*/
      }
    }
  }
  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set float buffer)
}

void adaptiveDisplay() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();
  //gluLookAt(camerax, cameray, cameraz, lookatx, lookaty, lookatz, 0, 0, 1);
  gluLookAt(camerax, cameray, cameraz, 0, 0, 0, 0, 0, 1);
  //glLoadIdentity();

  /*glTranslatef(0,-zoom,0);*/
  glTranslatef(tx,0,ty);
  glRotatef(rotx,0,0,1);
  glRotatef(roty,1,0,0);
  // Start drawing
  if (flat) {
    glShadeModel(GL_FLAT);
  } else {
    glShadeModel(GL_SMOOTH);
    //copied from http://www.cs.uml.edu/~haim/teaching/cg/resources/presentations/427/AngelCG20_shading_OpenGL.pdf.
    GLfloat diffuse0[]={1.0, 0.0, 0.0, 1.0};
    GLfloat ambient0[]={0.0, 0.0, 0.0, 1.0};
    GLfloat specular0[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light0_pos[]={colorx, colory, colorz, 1.0};
    GLfloat diffuse1[]={1.0, 0.0, 0.0, 1.0};
    GLfloat ambient1[]={0.0, 0.0, 0.0, 1.0};
    GLfloat specular1[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light1_pos[]={camerax, cameray, cameraz, 1.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
    GLfloat ambient[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[] = {1.0, 0.8, 0.0, 1.0};
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shine = 100.0;
    /*glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine); */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shine); 
  }
  glPointSize(10.0f);
  glBegin(GL_POINTS);
  glVertex3f(colorx, colory, colorz);
  glEnd();
  for (int patch = 0; patch < patches.size(); patch++) {
    //for (int patch = 0; patch < 1; patch++) {
    vector<vector<vector<float> > > currPatch = patches[patch];
    vector<vector<float> >interp1 = beezerpatch(currPatch, 0, 0);
    vector<vector<float> >interp2 = beezerpatch(currPatch, 1, 0);
    vector<vector<float> >interp3 = beezerpatch(currPatch, 0, 1);
    vector<vector<float> >interp4 = beezerpatch(currPatch, 1, 1);
    if (wireframe) {
    } else {
      glBegin(GL_TRIANGLES);
    }
    vector<vector<vector<float> > > triangle1, triangle2;
    triangle1.push_back(interp1);
    triangle1.push_back(interp2);
    triangle1.push_back(interp4);
    triangle2.push_back(interp1);
    triangle2.push_back(interp4);
    triangle2.push_back(interp3);
    triangle1 = triangulate(triangle1);
    triangle2 = triangulate(triangle2);
    for(int kk = 0; kk < triangle1.size(); kk++) {
      glNormal3fv(floady(triangle1[kk][1]));
      glVertex3fv(floady(triangle1[kk][0]));
    }
    for(int l = 0; l < triangle2.size(); l++) {
      glNormal3fv(floady(triangle2[l][1]));
      glVertex3fv(floady(triangle2[l][0]));
    }
    glEnd();
    /*glBegin(GL_LINES);
      glVertex3fv(floady(interp1[0]));
      glVertex3fv(floady(pluss(interp1[0], times(interp1[1], 0.15))));
      glVertex3fv(floady(interp2[0]));
      glVertex3fv(floady(pluss(interp2[0], times(interp2[1], 0.15))));
      glVertex3fv(floady(interp3[0]));
      glVertex3fv(floady(pluss(interp3[0], times(interp3[1], 0.15))));	
      glVertex3fv(floady(interp4[0]));
      glVertex3fv(floady(pluss(interp4[0], times(interp4[1], 0.15))));
      glEnd();*/
  }
  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set float buffer)
}

vector<vector<vector<float> > > triangulate(vector<vector<vector<float> > > triangle) {
  return triangle;
}

GLfloat* floady(vector<float> x) {
  GLfloat * ret = (GLfloat*) malloc(3 * sizeof(GLfloat));;
  ret[0] = x[0];
  ret[1] = x[1];
  ret[2] = x[2];
  return ret;
}

//returns 2-vector of 3-vectors, first is point and second is derivative
vector<vector<float> > beezercurve(vector<vector<float> > curve, float u) {
  vector<float> a = pluss(times(curve[0], 1-u), times(curve[1], u));
  vector<float> b = pluss(times(curve[1], 1-u), times(curve[2], u));
  vector<float> c = pluss(times(curve[2], 1-u), times(curve[3], u));
  vector<float> d = pluss(times(a, 1-u), times(b, u));
  vector<float> e = pluss(times(b, 1-u), times(c, u));
  vector<float> p = pluss(times(d, 1-u), times(e, u));
  vector<float> dpdu = times(minuss(e, d), 3);
  /*if (dpdu[0] == 0 && dpdu[1] == 0 && dpdu[2] == 0) {
    cout << "Horrror\n";
    printvect(e);
    printvect(d);
    cout << u << "\n";
    printvectvect(curve);
    cout << "/horror\n\n";
    }*/
  vector<vector<float> > result;
  result.push_back(p);
  result.push_back(dpdu);
  return result;
}

//interpolates the beezer patch, returning the 2-vector of the point and surface normal
vector<vector<float> >beezerpatch(vector<vector<vector<float> > > patch, float u, float v) {
  vector<vector<float> > draw;
  vector<vector<float> > ucurve;
  vector<vector<float> > vcurve;
  vcurve.push_back(beezercurve(patch[0], u+randy)[0]);
  vcurve.push_back(beezercurve(patch[1], u+randy)[0]);
  vcurve.push_back(beezercurve(patch[2], u+randy)[0]);
  vcurve.push_back(beezercurve(patch[3], u+randy)[0]);
  vector<vector<float> > col1;
  vector<vector<float> > col2;
  vector<vector<float> > col3;
  vector<vector<float> > col4;
  col1.push_back(patch[0][0]);
  col1.push_back(patch[1][0]);
  col1.push_back(patch[2][0]);
  col1.push_back(patch[3][0]);
  col2.push_back(patch[0][1]);
  col2.push_back(patch[1][1]);
  col2.push_back(patch[2][1]);
  col2.push_back(patch[3][1]);
  col3.push_back(patch[0][2]);
  col3.push_back(patch[1][2]);
  col3.push_back(patch[2][2]);
  col3.push_back(patch[3][2]);
  col4.push_back(patch[0][3]);
  col4.push_back(patch[1][3]);
  col4.push_back(patch[2][3]);
  col4.push_back(patch[3][3]);
  ucurve.push_back(beezercurve(col1, v+randy)[0]);
  ucurve.push_back(beezercurve(col2, v+randy)[0]);
  ucurve.push_back(beezercurve(col3, v+randy)[0]);
  ucurve.push_back(beezercurve(col4, v+randy)[0]);
  ///cout << "V";
  vector<vector<float> > pdpdv = beezercurve(vcurve, v);
  ///cout << "U";
  vector<vector<float> > pdpdu = beezercurve(ucurve, u);
  ///cout << "free\n";
  vector<float> n = normalize(cross(pdpdu[1], pdpdv[1]));
  //cout << "lol";
  //printvect(n);
  if (n[0] == 0 && n[1] == 0 && n[2] == 0) {
    cout << "Diagnostics\n\n";
    cout << "Patch:";
    printvectarray(patch);
    cout << "\nCol1: ";
    printvectvect(col1);
    cout << "\ncol2: ";
    printvectvect(col2);
    cout << "\ncol3: ";
    printvectvect(col3);
    cout << "\ncol4: ";
    printvectvect(col4);
    cout << "\nvcurve: ";
    printvectvect(vcurve);
    cout << "ucurve";
    printvectvect(ucurve);
    cout << "PdPdu1:";
    printvect(pdpdu[1]);
    cout << "PdPdv1:";
    printvect(pdpdv[1]);
    cout << "\n";
  }
  draw.push_back(pdpdu[0]);
  draw.push_back(n);
  return draw;
}

void initScene(int argc, char *argv[]) {
  glutInit(&argc, argv);
  //This tells glut to use a float-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;
  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (adaptive) {						// quick function to set up scene
    glutDisplayFunc(adaptiveDisplay);
  } else {
    glutDisplayFunc(uniformDisplay); // function to run when its time to draw something
  }
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
  glutKeyboardFunc(keyPressed); // Tell GLUT to use the method "keyPressed" for key presses (http://www.swiftless.com/tutorials/opengl/keyboard.html)
  glutSpecialFunc(arrows);
  gluPerspective(45, 1, 0.01, 10);
  camerax = lookatx/2;
  cameray = 2 * (lookatx + lookaty + lookatz);
  cameraz = 8;
  cout << camerax << " " << cameray << " " << cameraz;
}

int main(int argc, char *argv[]) {
  adaptive = 0;
  if (argc > 3) {
    if (strlen(argv[3]) == 2) {
      if (isEqual(argv[3], "-a", 2)) {
	adaptive = 1;
      }
    }
  }
  //cout << adaptive << "\n";
  ifstream file(argv[1]);
  step = atof(argv[2]);
  int paches;
  if (file.is_open()) {
    file >> paches;
    for (int patchno = 0; patchno < paches; patchno++) {
      vector<vector<vector<float> > > patch;
      for (int j = 0; j < 4; j++) {
	vector<vector<float> > row;
	for (int k = 0; k < 4; k++) {
	  vector<float> point;
	  float xcoordinate;
	  file >> xcoordinate;
	  point.push_back(xcoordinate);
	  float ycoordinate;
	  file >> ycoordinate;
	  point.push_back(ycoordinate);
	  float zcoordinate;
	  file >> zcoordinate;
	  point.push_back(zcoordinate);
	  row.push_back(point);
	  lookatx += xcoordinate;
	  lookaty += ycoordinate;
	  lookatz += zcoordinate;
	}
	patch.push_back(row);
      }
      patches.push_back(patch);
    }
    file.close();
  }
  lookatx /= (16.0 * paches);
  lookaty /= (16.0 * paches);
  lookatz /= (16.0 * paches);
  cout << lookatx << " " << lookaty << " " << lookatz << "\n";
  //This initializes glut
 
  initScene(argc, argv);
 
  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else
  return 0;
}

void arrows(int key, int x, int y) {
  int shift = glutGetModifiers();
  if (shift == GLUT_ACTIVE_SHIFT) {
    if (key == GLUT_KEY_UP) {
      ty += 0.1;
    }
    if (key == GLUT_KEY_DOWN) {
      ty -= 0.1;
    } 
    if (key == GLUT_KEY_LEFT) {
      tx += 0.1;
    } 
    if (key == GLUT_KEY_RIGHT) {
      tx -= 0.1;
    }
  } else {
    if (key == GLUT_KEY_UP) {
      roty += 1;
    }
    if (key == GLUT_KEY_DOWN) {
      roty -= 1;
    } 
    if (key == GLUT_KEY_LEFT) {
      rotx -= 1;
    } 
    if (key == GLUT_KEY_RIGHT) {
      rotx += 1;
    }
  }
  glutPostRedisplay();
}

void keyPressed (unsigned char key, int x, int y) {  
  if (key == 's') {
    flat = flat ^ 1;
  }
  if (key == 'w') {
    wireframe = wireframe ^ 1;
  }
  if (key == '-') {
    camerax = camerax + 0.1 * (camerax - lookatx);
    cameray = cameray + 0.1 * (cameray - lookaty);
    cameraz = cameraz + 0.1 * (cameraz - lookatz);
  }
  if (key == '+') {
    camerax = camerax - 0.1 * (camerax - lookatx);
    cameray = cameray - 0.1 * (cameray - lookaty);
    cameraz = cameraz - 0.1 * (cameraz - lookatz);
  }
  glutPostRedisplay();
}  

vector<float> pluss(vector<float> x, vector<float> y) {
  vector<float> result;
  result.push_back(x[0] + y[0]);
  result.push_back(x[1] + y[1]);
  result.push_back(x[2] + y[2]);
  return result;
}

vector<float> times(vector<float> x, float y) {
  vector<float> result;
  result.push_back(x[0] * y);
  result.push_back(x[1] * y);
  result.push_back(x[2] * y);
  return result;
}

vector<float> minuss(vector<float> x, vector<float> y) {
  vector<float> result;
  result.push_back(x[0] - y[0]);
  result.push_back(x[1] - y[1]);
  result.push_back(x[2] - y[2]);
  return result;
}

vector<float> cross(vector<float> u, vector<float> v) {
  vector<float> result;
  result.push_back(u[1] * v[2] - u[2] * v[1]);
  result.push_back(u[2] * v[0] - u[0] * v[2]);
  result.push_back(u[0] * v[1] - u[1] * v[0]);
  return result;
}

vector<float> normalize(vector<float> v) {
  //cout << "normalizen: ";
  //printvect(v);
  float norm = 0;
  vector<float> result;
  for (int k = 0; k < v.size(); k++) {
    norm += v[k] * v[k];
  }
  if (norm == 0) {
    cout << "Tragedy: nomrmalized a zero\n";
    norm = 1;
  }
  //cout << "\nnorm " << norm << "\n";
  //printvect(v);
  norm = sqrt(norm);
  result.push_back(v[0] / norm);
  result.push_back(v[1] / norm);
  result.push_back(v[2] / norm);
  //cout << "Normalizeded ";
  //printvect(result);
  return result;
}

void printate(GLfloat x[], int length) {
  cout << "\n\n";
  for (int k = 0; k < length; k++) {
    cout << x[k] << ", ";
  }
  cout << "\n";
}

void printvectarray(vector<vector<vector<float> > > x) {
  for (int k = 0; k < x.size(); k++) {
    cout <<"\n"<< k << ": ";
    for (int j = 0; j < x[k].size(); j++) {
      cout << "\n  " << j << ": ";
      for (int a = 0; a < x[k][j].size(); a++) {
	cout << x[k][j][a] << ", ";
      }
      cout << "  ";
    }
  }
  cout << "\n";
}

void printvectvect(vector<vector<float> > x) {
  for (int k = 0; k < x.size(); k++) {
    cout << "\n" << k << ": ";
    for (int j = 0; j < x[k].size(); j++) {
      cout << x[k][j] << " ";
    }
  }
  cout << "\n";
}

void printvect(vector<float> x) {
  for (int k = 0; k < x.size(); k++) {
    cout << x[k] << " ";
  }
  cout << "\n";
}

int isEqual(char *s, char *t, int len) {
  for (int k = 0; k < len; k++) {
    if (s[k] != t[k]) {
      return 0;
    }
  }
  return 1;
}
