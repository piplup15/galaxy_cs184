// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"

using namespace std ; 
#include "variables.h"
#include "readfile.h"
#include "ModelObj.h"

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.  
// Some of you may want to use the more modern routines in readfile.cpp 
// that can also be used.  

ModelObj list_of_models [max_obj_models]; // List containing all the models.
int size_of_list_models = 0;

void transformvec (const GLfloat input[4], GLfloat output[4]) {
  GLfloat modelview[16] ; // in column major order
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ; 
  
  for (int i = 0 ; i < 4 ; i++) {
    output[i] = 0 ; 
    for (int j = 0 ; j < 4 ; j++) 
      output[i] += modelview[4*j+i] * input[j] ; 
  }
}

void display() {
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	mat4 mv ; 
    mv = Transform::lookAt(eye,center,up) ; 
    mv = glm::transpose(mv) ; // accounting for row major
    glLoadMatrixf(&mv[0][0]) ; 

    // Set Light and Material properties for the teapot
    // Lights are transformed by current modelview matrix. 
    // The shader can't do this globally. 
    // So we need to do so manually.  
    glUniform1i(enablelighting,true) ; 
    for (int light_index = 0; light_index < numused; light_index++) {
        int offset = light_index*4;
        const GLfloat light_position[4] = {lightposn[offset+0], lightposn[offset+1], lightposn[offset+2], lightposn[offset+3]};
        GLfloat light_position_mv[4];
        transformvec(light_position, light_position_mv);
        for (int i = 0; i < 4; i++) {
            lightransf[offset + i] = light_position_mv[i];
        }
    }
    glUniform4fv(lightpos, numused, lightransf);
    glUniform4fv(lightcol, numused, lightcolor);
    glUniform1i(numusedcol, numused); 


    for (int i = 0 ; i < numobjects ; i++) {
        object * obj = &(objects[i]) ; 

        {
            mat4 result = mv * obj->transform ;
            glLoadMatrixf(&result[0][0]);
            glUniform4fv(ambientcol, 1, obj->ambient); 
            glUniform4fv(diffusecol, 1, obj->diffuse);
            glUniform4fv(specularcol, 1, obj->specular);
            glUniform4fv(emissioncol, 1, obj->emission);
            glUniform1f(shininesscol, obj->shininess);
        }


        if (obj -> type == cube) {
            glutSolidCube(obj->size) ; 
        }
            
        else if (obj -> type == sphere) {
            const int tessel = 20 ; 
            glutSolidSphere(obj->size, tessel, tessel) ; 
        }
            
        else if (obj -> type == teapot) {
            glutSolidTeapot(obj->size) ; 
        } 
        
        else if (obj -> type == disk) {
            GLUquadric * quadric = gluNewQuadric();
            gluDisk(quadric, obj -> innerRadius, obj -> outerRadius, obj -> slices, obj -> loops);
        }
        
        else if (obj -> type == cylinder) {
            GLUquadric * quadric = gluNewQuadric();
            gluCylinder(quadric, obj -> baseRadius, obj -> topRadius, obj -> size,  obj -> slices, obj -> stacks);
        }
            
        else if (obj -> type == modelobj) {
            ModelObj * object = new ModelObj();
            bool found_flag = false;
            for (int i = 0; i < size_of_list_models; i++) {
                if (list_of_models[i].name == obj->name) {
                    found_flag = true;
                    *object = list_of_models[i];
                }
            }
            if (!found_flag) {
                if (!object->loadObj(obj->file_path, obj->shape_sides, obj->name)) {
                    exit(1);
                }
                list_of_models[size_of_list_models] = *object;
                size_of_list_models++;
            }
              
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_INDEX_ARRAY);
              
            glIndexPointer(GL_UNSIGNED_INT, 0, object->vertex_indices);
            glNormalPointer(GL_FLOAT, 0, object->normals);
            glVertexPointer(3, GL_FLOAT, 0, object->vertices);
            if (object->shape == 4) {
                glDrawElements(GL_QUADS, object->num_of_indices, GL_UNSIGNED_INT, object->vertex_indices);
            } else if (object -> shape == 3) {
                glDrawElements(GL_TRIANGLES, object->num_of_indices, GL_UNSIGNED_INT, object->vertex_indices);
            }
            glDisableClientState(GL_INDEX_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
        }

    }
    glutSwapBuffers();
}
