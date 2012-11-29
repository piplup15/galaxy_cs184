/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <FreeImage.h>
#include "UCB/grader.h"
#include <sys/time.h>

using namespace std ; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp
#include "ModelObj.h"

bool * key_states = new bool[256];
struct timeval time_register_key, train_one_loop, train_two_loop;
int train_one_counter, train_two_counter;

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


// Uses the Projection matrices (technically deprecated) to set perspective 
// We could also do this in a more modern fashion with glm.  
void reshape(int width, int height){
	w = width;
	h = height;
    mat4 mv ; // just like for lookat
	glMatrixMode(GL_PROJECTION);
    float aspect = w / (float) h, zNear = 0.1, zFar = 999.0 ;
    mv = Transform::perspective(fovy,aspect,zNear,zFar) ; 
    mv = glm::transpose(mv) ; // accounting for row major 
    glLoadMatrixf(&mv[0][0]) ; 
    glViewport(0, 0, w, h);
}

void handleAnimation ( ) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    if ((current_time.tv_sec - train_one_loop.tv_sec) * 1000000.0 + (current_time.tv_usec - train_one_loop.tv_usec > 30000.0)) {
        train_one_counter = 32;
        gettimeofday(&train_one_loop, NULL);
    }
    if ((current_time.tv_sec - train_two_loop.tv_sec) * 1000000.0 + (current_time.tv_usec - train_two_loop.tv_usec > 30000.0)) {
        train_two_counter = 32;
        gettimeofday(&train_two_loop, NULL);
    }
    for (int i = 0; i < numobjects; i++) {
        object * obj = &(objects[i]);
        if (obj -> animation_state == "train_one_loop") {
            if (train_one_counter != 0) {
                obj->transform = Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * obj->transform; 
                gettimeofday(&(obj->timeUpdate), NULL);
                train_one_counter--;
            }
        } else if (obj -> animation_state == "wheel_one_loop") {
            if (train_one_counter != 0) {
                obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * glm::mat4(Transform::rotate(0.5, glm::normalize(glm::cross(glm::vec3(0.0,0.0,1.0), obj->direction)))) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
                obj->transform = Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * obj->transform;
                obj->direction = glm::vec3(obj->transform * glm::vec4(-1.0, 0.0, 0.0, 0.0));
                obj->position = glm::vec3(obj->transform * glm::vec4(0.0, 0.0, 0.0, 1.0));
                gettimeofday(&(obj->timeUpdate), NULL);
                train_one_counter--;
            }
        }
        if (obj -> animation_state == "train_two_loop") {
            if (train_two_counter != 0) {
                obj->transform = Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * obj->transform; 
                gettimeofday(&(obj->timeUpdate), NULL);
                train_two_counter--;
            }
        } else if (obj -> animation_state == "wheel_two_loop") {
            if (train_two_counter != 0) {
                obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * glm::mat4(Transform::rotate(0.5, glm::normalize(glm::cross(glm::vec3(0.0,0.0,1.0), obj->direction)))) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
                obj->transform = Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * obj->transform;
                obj->direction = glm::vec3(obj->transform * glm::vec4(-1.0, 0.0, 0.0, 0.0));
                obj->position = glm::vec3(obj->transform * glm::vec4(0.0, 0.0, 0.0, 1.0));
                gettimeofday(&(obj->timeUpdate), NULL);
                train_two_counter--;
            }
        }
    }
}


void printHelp() {
    std::cout << "\npress 'h' to print this message again.\n";  
}


void keyboard(unsigned char key, int x, int y) {
	key_states[key] = true;
}

void keyUp (unsigned char key, int x, int y) {
    key_states[key] = false;
}    


void idleFunc() {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    if ((current_time.tv_sec - time_register_key.tv_sec)*1000000.0+(current_time.tv_usec - time_register_key.tv_usec) > 20000.0) {
        if (key_states['h']) {
            cout << current_time.tv_usec << endl;
            printHelp();
        }
        if (key_states[27]) { // Escape to quit                                                                                                                                                                   
            exit(0);
        }
        if (key_states['w']) { // forward movement
            glm::vec3 direction = glm::normalize(glm::vec3(center.x - eye.x, center.y - eye.y, 0));
            eye = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
        }
        if (key_states['s']) { // backward movement
            glm::vec3 direction = glm::normalize(glm::vec3(-center.x + eye.x, -center.y + eye.y, 0));
            eye = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
        }
        if (key_states['a']) { // left movement
            glm::vec3 direction = glm::normalize(glm::vec3(-center.y + eye.y, center.x - eye.x, 0));
            eye = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
        }
        if (key_states['d']) { // right movement
            glm::vec3 direction = glm::normalize(glm::vec3(center.y - eye.y, -center.x + eye.x, 0));
            eye = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/50.0, direction.y/50.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
        }
        if (key_states['l']) { // up movement
            glm::vec3 direction = glm::normalize(glm::vec3(0, 0, -1));
            eye = glm::vec3(Transform::translate(0, 0, direction.z/50.0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(0, 0, direction.z/50.0) * glm::vec4(center.x, center.y, center.z, 1));
        }
        if (key_states['o']) { // down movement
            glm::vec3 direction = glm::normalize(glm::vec3(0, 0, 1));
            eye = glm::vec3(Transform::translate(0, 0, direction.z/50.0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(0, 0, direction.z/50.0) * glm::vec4(center.x, center.y, center.z, 1));
        }
        gettimeofday(&time_register_key, NULL);
    }
    handleAnimation();
    glutPostRedisplay();
}

void specialKey(int key,int x,int y) {
	switch(key) {
		case 100: //left
            eyeinit = Transform::rotate(-amount, upinit) * eyeinit;
            eye = eyeinit + center;
			break;
		case 101: //up
			eyeinit = Transform::rotate(amount, glm::cross(eyeinit, upinit)) * eyeinit;
            eye = eyeinit + center;
			break;
		case 102: //right
			eyeinit = Transform::rotate(amount, upinit) * eyeinit;
            eye = eyeinit + center;
			break;
		case 103: //down
			eyeinit = Transform::rotate(-amount, glm::cross(eyeinit, upinit)) * eyeinit;
            eye = eyeinit + center;
			break;
	}
	glutPostRedisplay();
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
    
    gettimeofday(&time_register_key, NULL);
    anim_state = "none";
    gettimeofday(&train_one_loop,NULL);
    train_one_counter = 32; // train 1 has 32 objects
    gettimeofday(&train_two_loop,NULL);
    train_two_counter = 32; // train 2 has 32 objects
    
    char_position = vec3(0.0,0.0,0.0);
}

int main(int argc, char* argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("HW6: Super Mario Galaxy: Toy Time Galaxy");
	init();
    readfile("input/toytimegalaxy.scene") ; 
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idleFunc);
	glutReshapeFunc(reshape);
	glutReshapeWindow(w, h);
    glutPassiveMotionFunc(mouse);
    for (int i = 0; i < 256; i++) {
        key_states[i] = false;
    }
	printHelp();
	glutMainLoop();
	return 0;
}
