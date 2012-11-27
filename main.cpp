#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <math.h>
#include <sys/time.h>

using namespace std;

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
#include "ModelObj.h"

bool * key_states = new bool[256];

void display(void) ;  // prototype for display function.
void loadTex(const char * filename, GLubyte textureLocation[256][256][3]);

// Very basic code to read a ppm file
// And then set up buffers for texture coordinates
void loadTex (const char * filename, GLubyte textureLocation[256][256][3]) {
	int i,j,k ;
	FILE * fp ; 
	GLint err ; 
	assert(fp = fopen(filename,"rb")) ;
	fscanf(fp,"%*s %*d %*d %*d%*c") ;
	for (i = 0 ; i < 256 ; i++)
		for (j = 0 ; j < 256 ; j++)
			for (k = 0 ; k < 3 ; k++)
				fscanf(fp,"%c",&(textureLocation[i][j][k])) ;
	fclose(fp) ;  
}

void reshape(int width, int height) {
    w = width;
    h = height;
    mat4 mv; // for lookat
    float aspect = w / (float) h, zNear = 0.001, zFar = 99.99 ;
    glMatrixMode(GL_PROJECTION);
    mv = Transform::perspective(fovy,aspect,zNear,zFar);
    mv = glm::transpose(mv);
    glLoadMatrixf(&mv[0][0]);
    glViewport(0,0,w,h);
}

void printHelp() {
    std::cout << "\npress 'h' to print this message again.\n";
    std::cout << "press 'ESC' to quit.\n";
}



void keyboard(unsigned char key, int x, int y) {
    key_states[key] = true;
}

void idleFunc ( ) {
    if (key_states['h']) {
        printHelp();
    }
    if (key_states[27]) { // Escape to quit                                                                                                                                                                   
        exit(0);
    }
}


void keyUp (unsigned char key, int x, int y) {
    key_states[key] = false;
}

void mouse(int x, int y) {
    mousex = x;
    mousey = y;
    if (mousex < 0) {
        mousex = 0;
    } else if (mousex > w) {
        mousex = w;
    }
    if (mousey < 0) {
        mousey = 0;
    } else if (mousey > h) {
        mousey = h;
    }
}

/* Initializes textures */\
void initTextures(){
 
}

void init() {

  // Initialize shaders
  vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
  fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
  shaderprogram = initprogram(vertexshader, fragmentshader) ; 
  enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
  lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;       
  lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;       
  numusedcol = glGetUniformLocation(shaderprogram,"numused") ;       
  ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
  diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
  specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
  emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
  shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;
  
  w = 640.0;
  h = 640.0;
    
  
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
  initTextures();
  
}

int main (int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("HW6: Super Mario Galaxy --- Toy Time Galaxy");
    init();
    readfile("input/toytimegalaxy.scene");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutKeyboardUpFunc(keyUp);
    glutReshapeWindow(w,h);
    glutIdleFunc(idleFunc);
    glutPassiveMotionFunc(mouse);
    for (int i = 0; i < 256; i++) {
        key_states[i] = false;
    }
    printHelp();
    glutMainLoop();
    return 0;
}
