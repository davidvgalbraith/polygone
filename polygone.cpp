//much openGL code ripped from tutorial at http://openglbook.com/the-book/
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
inline float sqr(float x) { return x*x; }
int vertex, drawn;
GLuint
VertexShaderId,
  FragmentShaderId,
  ProgramId,
  VaoId,
  VboId,
  ColorBufferId;

const GLchar* VertexShader = {
  "#version 400\n"\
 
  "layout(location=0) in vec4 in_Position;\n"\
  "layout(location=1) in vec4 in_Color;\n"\
  "out vec4 ex_Color;\n"\
 
  "void main(void)\n"\
  "{\n"\
  "   gl_Position = in_Position;\n"\
  "   ex_Color = in_Color;\n"\
  "}\n"
};

const GLchar* FragmentShader = {
  "#version 400\n"\
 
  "in vec4 ex_Color;\n"\
  "out vec4 out_Color;\n"\
 
  "void main(void)\n"\
  "{\n"\
  "   out_Color = ex_Color;\n"\
  "}\n"
};


using namespace std;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
public:
  int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;




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
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//*********************************
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  glDrawArrays(GL_LINES, 0, 3);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

void printvectarray(vector<vector<vector<double> > > x) {
  for (int k = 0; k < 4; k++) {
    cout <<"\n"<< k << ": ";
    for (int j = 0; j < 4; j++) {
      for (int a = 0; a < 3; a++) {
	cout << x[k][j][a] << " ";
      }
      cout << "  ";
    }
  }
}

void printvectvect(vector<vector<double> > x) {
  for (int k = 0; k < x.size(); k++) {
    cout << "\n" << k << ": ";
    for (int j = 0; j < x[k].size(); j++) {
      cout << x[k][j] << " ";
    }
  }
}
vector<double> pluss(vector<double> x, vector<double> y);
vector<double> times(vector<double> x, double y);
vector<double> minuss(vector<double> x, vector<double> y);
vector<double> cross(vector<double> u, vector<double> v);
vector<double> beezerpatch(vector<vector<vector<double> > > patch, double u, double v);

vector<vector<double> > subdividePatch(vector<vector<vector<double> > > patch, double step) {
  vector<vector<double> > toDraw;
  int numdiv = (1 + step/10) / step;
  for (int iu = 0; iu <= numdiv; iu++) {
    double u = iu * step;
    for (int iv = 0; iv <= numdiv; iv++) {
      double v = iv * step;
      vector<double> carter = beezerpatch(patch, u, v);
      //cout << "\n" << u << ", " << v << ": " << carter[0] << " " << carter[1] << " " << carter[2];
      toDraw.push_back(carter);
    }
  }
  return toDraw;

}
vector<double> pluss(vector<double> x, vector<double> y) {
  vector<double> result;
  result.push_back(x[0] + y[0]);
  result.push_back(x[1] + y[1]);
  result.push_back(x[2] + y[2]);
  return result;
}

vector<double> times(vector<double> x, double y) {
  vector<double> result;
  result.push_back(x[0] * y);
  result.push_back(x[1] * y);
  result.push_back(x[2] * y);
  return result;
}

vector<double> minuss(vector<double> x, vector<double> y) {
  vector<double> result;
  result.push_back(x[0] - y[0]);
  result.push_back(x[1] - y[1]);
  result.push_back(x[2] - y[2]);
  return result;
}

vector<double> cross(vector<double> u, vector<double> v) {
  vector<double> result;
  result.push_back(u[1] * v[2] - u[2] * v[1]);
  result.push_back(u[2] * v[0] - u[0] * v[2]);
  result.push_back(u[0] * v[1] - u[1] * v[0]);
  return result;
}

//returns 2-vector of 3-vectors, first is point and second is derivative
vector<vector<double> > beezercurve(vector<vector<double> > curve, double u) {
  vector<double> a = pluss(times(curve[0], 1-u), times(curve[1], u));
  vector<double> b = pluss(times(curve[1], 1-u), times(curve[2], u));
  vector<double> c = pluss(times(curve[2], 1-u), times(curve[3], u));
  vector<double> d = pluss(times(a, 1-u), times(b, u));
  vector<double> e = pluss(times(b, 1-u), times(c, u));
  vector<double> p = pluss(times(d, 1-u), times(e, u));
  vector<double> dpdu = times(minuss(e, d), 3);
  vector<vector<double> > result;
  result.push_back(p);
  result.push_back(dpdu);
  return result;
}



//interpolates the beezer patch, returning the 6-vector of the point and surface normal
vector<double> beezerpatch(vector<vector<vector<double> > > patch, double u, double v) {
  vector<double> toDraw;
  vector<vector<double> > ucurve;
  vector<vector<double> > vcurve;
  vcurve.push_back(beezercurve(patch[0], u)[0]);
  vcurve.push_back(beezercurve(patch[1], u)[0]);
  vcurve.push_back(beezercurve(patch[2], u)[0]);
  vcurve.push_back(beezercurve(patch[3], u)[0]);
  vector<vector<double> > col1;
  vector<vector<double> > col2;
  vector<vector<double> > col3;
  vector<vector<double> > col4;
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
  vector<vector<double> > pdpdv = beezercurve(vcurve, v);
  vector<vector<double> > pdpdu = beezercurve(ucurve, u);
  vector<double> n = cross(pdpdv[1], pdpdu[1]);
  toDraw.push_back(pdpdu[0][0]);
  toDraw.push_back(pdpdu[0][1]);
  toDraw.push_back(pdpdu[0][2]);
  toDraw.push_back(n[0]);
  toDraw.push_back(n[1]);
  toDraw.push_back(n[2]);
  return toDraw;
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  ifstream file(argv[1]);
  int patches;
  //each even element in toDraw is a point and the odds are that point's surface normal
  vector<vector<double> > toDraw;
  if (file.is_open()) {
    file >> patches;
    for (int patchno = 0; patchno < patches; patchno++) {
      vector<vector<vector<double> > > patch;
      for (int j = 0; j < 4; j++) {
	vector<vector<double> > row;
	for (int k = 0; k < 4; k++) {
	  vector<double> point;
	  for (int kk = 0; kk < 3; kk++) {
	    double coordinate;
	    file >> coordinate;
	    point.push_back(coordinate);
	  }
	  row.push_back(point);
	}
	patch.push_back(row);
      }
      vector<vector<double> > betterDraw = subdividePatch(patch, 0.1);
      for (int better = 0; better < betterDraw.size(); better++) {
	vector<double> poinoint;
	poinoint.push_back(betterDraw[better][0]);
	poinoint.push_back(betterDraw[better][1]);
	poinoint.push_back(betterDraw[better][2]);
	poinoint.push_back(1);
	toDraw.push_back(poinoint);
      }
    }
    file.close();
  }
  printvectvect(toDraw);
  cout << "\n";
  //return 2;
  GLfloat *vertices = new GLfloat[toDraw.size()];
  vertex = 0;
  for (int aardvark = 0; aardvark < toDraw.size(); aardvark += 1) {
    cout << "\ntodrawsize: " << toDraw[aardvark].size() << "  aardvark " << aardvark << "   vertex " << vertex;
    vertices[vertex++] = toDraw[aardvark][0];
    vertices[vertex++] = toDraw[aardvark][1];
    vertices[vertex++] = toDraw[aardvark][2];
    vertices[vertex++] = 1.0;
  }
  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);
  
  glGenBuffers(1, &VboId);
  glBindBuffer(GL_ARRAY_BUFFER, VboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}
