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
#define PI 3.14159265  // Should be used from mathlib

using namespace std;

inline float sqr(float x) { return x*x; }
class Viewport;
class Viewport {
public:
  int w, h; // width and height
};
int vertex, drawn;
//Holds the patches
vector<vector<vector<vector<float> > > > patches;
//What shading?
int flat = 0;
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

//****************************************************
// Simple init function
//****************************************************

void initScene(){
  // Nothing to do here for this simple example.
}
//****************************************************
// reshape viewport if the window is resized
//****************************************************

void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;
  glViewport (0,0,viewport.w,viewport.h);
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************

void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer
  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();
  gluLookAt(.2, .1, 0.5, 0.5, 0.5, 0.7, 0, 0, 1);
  
// Start drawing
  if (flat) {
    glColor3f(1.0f, 1.0f, 1.0f);
  } else {
    glShadeModel(GL_SMOOTH);
    //copied from http://www.cs.uml.edu/~haim/teaching/cg/resources/presentations/427/AngelCG20_shading_OpenGL.pdf.
    GLfloat diffuse0[]={1.0, 0.0, 0.0, 1.0};
    GLfloat ambient0[]={0.0, 0.0, 0.0, 1.0};
    GLfloat specular0[]={1.0, 1.0, 1.0, 1.0};
    GLfloat light0_pos[]={0.0, 0.0, 1.0, 1.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    GLfloat ambient[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[] = {1.0, 0.8, 0.0, 1.0};
    GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shine = 100.0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine); 
  }
  glPointSize(50.0f);
  //glBegin(GL_POINTS);
  //glVertex3f(0, 0, 0);
  //glEnd();
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
	cout << "One\n";
	printvect(interp1[0]);
	/*cout << "Two\n";
	printvect(interp2);
	cout << "Three\n";
	printvect(interp3);
	cout << "Four\n";
	printvect(interp4);*/
	glBegin(GL_QUADS);
	glNormal3fv(floady(interp1[1]));
	glVertex3fv(floady(interp1[0]));
	glVertex3fv(floady(interp2[0]));
	glVertex3fv(floady(interp4[0]));
	glVertex3fv(floady(interp3[0]));
	glEnd();
	/*glBegin(GL_LINES);
	glVertex3fv(floady(interp1[0]));
	glVertex3fv(floady(pluss(interp1[0], times(interp1[1], 0.1))));
	glEnd();*/
      }
    }
  }
  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set float buffer)
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
  vcurve.push_back(beezercurve(patch[0], u)[0]);
  vcurve.push_back(beezercurve(patch[1], u)[0]);
  vcurve.push_back(beezercurve(patch[2], u)[0]);
  vcurve.push_back(beezercurve(patch[3], u)[0]);
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
  ucurve.push_back(beezercurve(col1, v)[0]);
  ucurve.push_back(beezercurve(col2, v)[0]);
  ucurve.push_back(beezercurve(col3, v)[0]);
  ucurve.push_back(beezercurve(col4, v)[0]);
  vector<vector<float> > pdpdv = beezercurve(vcurve, v);
  vector<vector<float> > pdpdu = beezercurve(ucurve, u);
  vector<float> n = normalize(cross(pdpdu[1], pdpdv[1]));
  draw.push_back(pdpdu[0]);
  draw.push_back(n);
  return draw;
}

int main(int argc, char *argv[]) {
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
	  for (int kk = 0; kk < 3; kk++) {
	    float coordinate;
	    file >> coordinate;
	    point.push_back(coordinate);
	  }
	  row.push_back(point);
	}
	patch.push_back(row);
      }
      patches.push_back(patch);
    }
    file.close();
  }
   //This initializes glut
  glutInit(&argc, argv);
  //This tells glut to use a float-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;
  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho(-1, 1, -1, 1, -1, 1);
  initScene();							// quick function to set up scene
  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else
  return 0;
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
  //  cout << "normalizen: ";
  //printvect(v);
  float norm;
  vector<float> result;
  for (int k = 0; k < v.size(); k++) {
    norm += v[k] * v[k];
  }
  if (norm == 0) {
    cout << "Tragedy: nomrmalized a zero";
    norm = 1;
  }
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
}

void printvectvect(vector<vector<float> > x) {
  for (int k = 0; k < x.size(); k++) {
    cout << "\n" << k << ": ";
    for (int j = 0; j < x[k].size(); j++) {
      cout << x[k][j] << " ";
    }
  }
}

void printvect(vector<float> x) {
  for (int k = 0; k < x.size(); k++) {
    cout << x[k] << " ";
  }
  cout << "\n";
}
