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
#include "Texture.h"

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
    Texture tex;
    tex.set("images/shapes/coin.mtl");


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


    for (int i = 0 ; i < num_static_objects ; i++) {
        object * obj = &(static_objects[i]) ; 

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
    
    for (std::vector<object>::iterator it = dynamic_objects.begin(); it != dynamic_objects.end(); ++it) {
        object * obj = &(*it);
        {
            mat4 result = mv * obj->transform ;
            glLoadMatrixf(&result[0][0]);
            glUniform4fv(ambientcol, 1, obj->ambient); 
            glUniform4fv(diffusecol, 1, obj->diffuse);
            glUniform4fv(specularcol, 1, obj->specular);
            glUniform4fv(emissioncol, 1, obj->emission);
            glUniform1f(shininesscol, obj->shininess);
        }

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
    
    
    
    
    // CHARACTER
    GLUquadricObj * quad_obj = gluNewQuadric();
    
    // Body
    GLfloat body_ambient[4] = {0.3,0.3,0.8,1};
    GLfloat body_diffuse[4] = {0.1,0.1,0.1,1};
    GLfloat body_specular[4] = {0.15,0.15,0.2,1};
    GLfloat body_shininess = 2;
    
    glUniform4fv(ambientcol, 1, body_ambient);
    glUniform4fv(diffusecol, 1, body_diffuse);
    glUniform4fv(specularcol, 1, body_specular);
    glUniform1f(shininesscol, body_shininess);
    
    glLoadMatrixf(&(mv * character->transform)[0][0]);
    glutSolidSphere(0.1, 40, 40);
    
    // Ears
    GLfloat ear_ambient[4] = {0.3,0.3,0.8,1};
    GLfloat ear_diffuse[4] = {0.1,0.1,0.1,1};
    GLfloat ear_specular[4] = {0.15,0.15,0.2,1};
    GLfloat ear_shininess = 4;
    
    glUniform4fv(ambientcol, 1, ear_ambient);
    glUniform4fv(diffusecol, 1, ear_diffuse);
    glUniform4fv(specularcol, 1, ear_specular);
    glUniform1f(shininesscol, ear_shininess);
    
    glLoadMatrixf(&(mv * character->transform * Transform::translate(-0.07, -0.02, 0.07) * glm::mat4(Transform::rotate(90, glm::vec3(-1, 0, 0))))[0][0]);
    gluCylinder(quad_obj, 0.05, 0.05, 0.04, 40, 40);
    glLoadMatrixf(&(mv * character->transform * Transform::translate(-0.07, -0.02, 0.07) * glm::mat4(Transform::rotate(90, glm::vec3(-1, 0, 0))))[0][0]);
    gluDisk(quad_obj, 0, 0.05, 40, 40);
    glLoadMatrixf(&(mv * character->transform * Transform::translate(-0.07, 0.02, 0.07) * glm::mat4(Transform::rotate(90, glm::vec3(-1, 0, 0))))[0][0]);
    gluDisk(quad_obj, 0, 0.05, 40, 40);

    glLoadMatrixf(&(mv * character->transform * Transform::translate(0.07, -0.02, 0.07) * glm::mat4(Transform::rotate(90, glm::vec3(-1, 0, 0))))[0][0]);
    gluCylinder(quad_obj, 0.05, 0.05, 0.04, 40, 40);
    glLoadMatrixf(&(mv * character->transform * Transform::translate(0.07, -0.02, 0.07) * glm::mat4(Transform::rotate(90, glm::vec3(-1, 0, 0))))[0][0]);
    gluDisk(quad_obj, 0, 0.05, 40, 40);
    glLoadMatrixf(&(mv * character->transform * Transform::translate(0.07, 0.02, 0.07) * glm::mat4(Transform::rotate(90, glm::vec3(-1, 0, 0))))[0][0]);
    gluDisk(quad_obj, 0, 0.05, 40, 40);
    
    // Feet
    GLfloat foot_ambient[4] = {0.3,0.3,0.8,1};
    GLfloat foot_diffuse[4] = {0.1,0.1,0.1,1};
    GLfloat foot_specular[4] = {0.15,0.15,0.2,1};
    GLfloat foot_shininess = 2;
    
    glUniform4fv(ambientcol, 1, foot_ambient);
    glUniform4fv(diffusecol, 1, foot_diffuse);
    glUniform4fv(specularcol, 1, foot_specular);
    glUniform1f(shininesscol, foot_shininess);
    
    glLoadMatrixf(&(mv * character->transform * glm::mat4(Transform::rotate(char_feet_rotation, glm::vec3(-1, 0, 0))) * Transform::translate(0.05, 0.015, -0.095) * Transform::scale(0.3, 0.5, 0.3))[0][0]);
    glutSolidSphere(0.1, 40, 40);
    
    glLoadMatrixf(&(mv * character->transform * glm::mat4(Transform::rotate(-char_feet_rotation, glm::vec3(-1, 0, 0))) * Transform::translate(-0.05, 0.015, -0.095) * Transform::scale(0.3, 0.5, 0.3))[0][0]);
    glutSolidSphere(0.1, 40, 40);
    
    // Tail
    if (!hide_tail) {
        GLfloat tail_ambient[4] = {0,0,0,1};
        GLfloat tail_diffuse[4] = {0.1,0.1,0.1,1};
        GLfloat tail_specular[4] = {0.05,0.05,0.05,1};
        GLfloat tail_shininess = 2;
    
        glUniform4fv(ambientcol, 1, tail_ambient);
        glUniform4fv(diffusecol, 1, tail_diffuse);
        glUniform4fv(specularcol, 1, tail_specular);
        glUniform1f(shininesscol, tail_shininess);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.08, -0.08) * glm::mat4(Transform::rotate(60, glm::vec3(-1, 0, 0))) * Transform::translate(0, 0, -0.03))[0][0]);
        gluCylinder(quad_obj, 0.01, 0.01, 0.06, 40, 40);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.106, -0.095))[0][0]);
        glutSolidSphere(0.01, 40, 40);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.12, -0.07) * glm::mat4(Transform::rotate(150, glm::vec3(-1, 0, 0))) * Transform::translate(0, 0, -0.03))[0][0]);
        gluCylinder(quad_obj, 0.01, 0.01, 0.06, 40, 40);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.134, -0.045))[0][0]);
        glutSolidSphere(0.01, 40, 40);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.1484, -0.04888) * glm::mat4(Transform::rotate(75, glm::vec3(-1, 0, 0))) * Transform::translate(0, 0, -0.015))[0][0]);
        gluCylinder(quad_obj, 0.01, 0.01, 0.03, 40, 40);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.1628, -0.05276))[0][0]);
        glutSolidSphere(0.01, 40, 40);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.17056, -0.01991) * glm::mat4(Transform::rotate(165, glm::vec3(-1, 0, 0))) * Transform::translate(0, 0, -0.03))[0][0]);
        gluCylinder(quad_obj, 0.01, 0.01, 0.06, 40, 40);
    
        GLfloat tail_ball_ambient[4] = {0.3,0.3,0.8,1};
        GLfloat tail_ball_diffuse[4] = {0.1,0.1,0.1,1};
        GLfloat tail_ball_specular[4] = {0.3,0.3,0.3,1};
        GLfloat tail_ball_shininess = 2;
    
        glUniform4fv(ambientcol, 1, tail_ball_ambient);
        glUniform4fv(diffusecol, 1, tail_ball_diffuse);
        glUniform4fv(specularcol, 1, tail_ball_specular);
        glUniform1f(shininesscol, tail_ball_shininess);
    
        glLoadMatrixf(&(mv * character->transform * Transform::translate(0, -0.0707, -0.0707) * glm::mat4(Transform::rotate(char_tail_rotation, glm::vec3(0, 0.8660254, 0.5))) * Transform::translate(0, 0.0707, 0.0707) * Transform::translate(0, -0.182212, 0.023545))[0][0]);
        glutSolidSphere(0.05, 40, 40);
    }
    
    // Eye
    GLfloat eye_ambient[4] = {0,0,0,1};
    GLfloat eye_diffuse[4] = {0.1,0.1,0.1,1};
    GLfloat eye_specular[4] = {0.05,0.05,0.05,1};
    GLfloat eye_shininess = 2;
    
    glUniform4fv(ambientcol, 1, eye_ambient);
    glUniform4fv(diffusecol, 1, eye_diffuse);
    glUniform4fv(specularcol, 1, eye_specular);
    glUniform1f(shininesscol, eye_shininess);
    
    glLoadMatrixf(&(mv * character->transform * glm::mat4(Transform::rotate(110, glm::vec3(1,0,0))) * Transform::translate(0.03, 0, -0.083) * Transform::scale(0.15, 0.25, 0.15))[0][0]);
    glutSolidSphere(0.1, 40, 40);
    
    glLoadMatrixf(&(mv * character->transform * glm::mat4(Transform::rotate(110, glm::vec3(1,0,0))) * Transform::translate(-0.03, 0, -0.083) * Transform::scale(0.15, 0.25, 0.15))[0][0]);
    glutSolidSphere(0.1, 40, 40);
    
    glutSwapBuffers();
}
