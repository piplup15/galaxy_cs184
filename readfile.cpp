/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  These functions are given to aid in setting up the 
// transformations properly, and to use glm functions in the right way.  
// Their use is optional in your program.  
  

// The functions readvals and readfile do basic parsing.  You can of course 
// rewrite the parser as you wish, but we think this basic form might be 
// useful to you.  It is a very simple parser.

// Please fill in parts that say YOUR CODE FOR HW 2 HERE. 
// Read the other parts to get a context of what is going on. 
  
/*****************************************************************************/

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"
#include <sys/time.h>

using namespace std ;
#include "variables.h" 
#include "readfile.h"
#include "ModelObj.h"

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, GLfloat * values) {
  mat4 transform = transfstack.top() ; 
  vec4 valvec = vec4(values[0],values[1],values[2],values[3]) ; 
  vec4 newval = valvec * transform ; 
  for (int i = 0 ; i < 4 ; i++) values[i] = newval[i] ; 
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) {
  mat4 &T = transfstack.top() ; 
  // Right multiply M, but do this left to account for row/column major 
  T = M * T ; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, GLfloat * values) {
  for (int i = 0 ; i < numvals ; i++) {
    s >> values[i] ; 
    if (s.fail()) {
      cout << "Failed reading value " << i << " will skip\n" ; 
      return false ;
    }
  }
  return true ; 
}

void readfile(const char * filename) {
    string str, cmd ; 
    ifstream in ;
    in.open(filename) ; 
    if (in.is_open()) {
        
        // I need to implement a matrix stack to store transforms.  
        // This is done using standard STL Templates 
        stack <mat4> transfstack ; 
        transfstack.push(mat4(1.0)) ;  // identity
        stack <glm::vec3> scalestack;
        scalestack.push(glm::vec3(1.0,1.0,1.0));

        getline (in, str) ; 
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) { 
                stringstream s(str) ;
                s >> cmd ; 
                int i ; 
                GLfloat values[10] ; // position and color for light, colors for others
                             // Up to 10 params for cameras.  
                bool validinput ; // validity of input 

                // Process the light, add it to database.
                // Lighting Command
                if (cmd == "light") { 
                    if (numused == numLights) // No more Lights 
                        cerr << "Reached Maximum Number of Lights " << numused << " Will ignore further lights\n" ;
                    else {
                        validinput = readvals(s, 8, values) ; // Position/color for lts.
                        if (validinput) {
                            int offset = 4 * numused;
                            for (int i = 0; i < 4; i++) {
                                lightposn[offset+i] = values[i];
                                lightcolor[offset+i] = values[4+i];
                            }
                            ++numused ; 
                        }
                    }
                }

                // Material Commands 
                // Ambient, diffuse, specular, shininess
                // Filling this in is pretty straightforward, so I've left it in 
                // the skeleton, also as a hint of how to do the more complex ones.
                // Note that no transforms/stacks are applied to the colors. 

                else if (cmd == "backgroundColor") {
                    validinput = readvals(s, 4, values) ;
                    if (validinput)
                        for (i = 0 ; i < 4 ; i++) backgroundColor[i] = values[i] ;
                }
            
                else if (cmd == "ambient") {
                    validinput = readvals(s, 4, values) ;
                    if (validinput) 
                        for (i = 0 ; i < 4 ; i++) ambient[i] = values[i] ; 
                }
                else if (cmd == "diffuse") {
                    validinput = readvals(s, 4, values) ; 
                    if (validinput) 
                        for (i = 0 ; i < 4 ; i++) diffuse[i] = values[i] ; 
                }
                else if (cmd == "specular") {
                    validinput = readvals(s, 4, values) ; 
                    if (validinput) 
                        for (i = 0 ; i < 4 ; i++) specular[i] = values[i] ; 
                }
                else if (cmd == "emission") {
                    validinput = readvals(s, 4, values) ; 
                    if (validinput) 
                        for (i = 0 ; i < 4 ; i++) emission[i] = values[i] ; 
                }
                else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values) ; 
                    if (validinput) shininess = values[0] ; 
                }
                else if (cmd == "size") {
                    validinput = readvals(s,2,values) ; 
                    if (validinput) { w = (int) values[0] ; h = (int) values[1] ; } 
                }
                else if (cmd == "camera") {
                    validinput = readvals(s,10,values) ; // 10 values eye cen up fov
                    if (validinput) {
                        eyeinit = vec3(values[0], values[1], values[2]);
                        eye = vec3(values[0], values[1], values[2]);
                        center = vec3(values[3], values[4], values[5]);
                        upinit = vec3(values[6], values[7], values[8]);
                        up = vec3(values[6], values[7], values[8]);
                        fovy = values[9];
                    }
                }
                else if (cmd == "test_collision") {
                    std::string value;
                    s >> value;
                    if (value == "true") {
                        test_collision = true;
                    } else {
                        test_collision = false;
                    }
                }

                // I've left the code for loading objects in the skeleton, so 
                // you can get a sense of how this works.  
                else if (cmd == "sphere" || cmd == "cube") {
                    if (num_static_objects == maxobjects) // No more objects 
                        cerr << "Reached Maximum Number of Objects " << num_static_objects << " Will ignore further objects\n" ; 
                    else {
                        validinput = readvals(s, 1, values) ; 
                        if (validinput) {
                            object * obj = &(static_objects[num_static_objects]) ; 
                            obj -> size = values[0] ;
                            obj -> scale_size = scalestack.top();
                            for (i = 0 ; i < 4 ; i++) {
                                (obj -> ambient)[i] = ambient[i] ; 
                                (obj -> diffuse)[i] = diffuse[i] ; 
                                (obj -> specular)[i] = specular[i] ; 
                                (obj -> emission)[i] = emission[i] ;
                            }
                            obj -> shininess = shininess ; 
                            obj -> transform = transfstack.top() ;
                            obj -> test_collision = test_collision;
                            if (cmd == "sphere") {
                                obj -> type = sphere ; 
                                obj -> bounding_type = "none";
                            }
                            else if (cmd == "cube") {
                                obj -> type = cube ;
                                obj -> bounding_type = "square";
                                obj->max_x = obj-> max_y = obj->max_z = values[0]*0.5;
                                obj->min_x = obj-> min_y = obj->min_z = -values[0]*0.5;
                            } 
                            obj -> animation_state = anim_state;
                            gettimeofday(&(obj -> timeUpdate), NULL);
                        }
                        ++num_static_objects ; 
                    }
                }
                
                else if (cmd == "disk") {
                    if (num_static_objects == maxobjects) // No more objects
                        cerr << "Reached Maximum Number of Objects " << num_static_objects << " Will ignore further objects\n" ;
                    else {
                        validinput = readvals(s, 4, values) ;
                        if (validinput) {
                            object * obj = &(static_objects[num_static_objects]) ;  
                            for (i = 0 ; i < 4 ; i++) {
                                (obj -> ambient)[i] = ambient[i] ; 
                                (obj -> diffuse)[i] = diffuse[i] ; 
                                (obj -> specular)[i] = specular[i] ; 
                                (obj -> emission)[i] = emission[i] ;
                            }
                            obj -> shininess = shininess ; 
                            obj -> transform = transfstack.top() ;
                            obj -> type = disk;
                            obj -> innerRadius = values[0];
                            obj -> outerRadius = values[1];
                            obj -> slices = values[2];
                            obj -> loops = values[3];
                            obj -> animation_state = anim_state;
                            obj -> test_collision = test_collision;
                            obj -> scale_size = scalestack.top();
                            obj -> bounding_type = "circle";
                            gettimeofday(&(obj -> timeUpdate), NULL);
                        }
                        ++num_static_objects ;
                    }
                }
                
                else if (cmd == "cylinder") {
                    if (num_static_objects == maxobjects) // No more objects
                        cerr << "Reached Maximum Number of Objects " << num_static_objects << " Will ignore further objects\n" ;
                    else {
                        validinput = readvals(s, 5, values) ;
                        if (validinput) {
                            object * obj = &(static_objects[num_static_objects]) ;  
                            for (i = 0 ; i < 4 ; i++) {
                                (obj -> ambient)[i] = ambient[i] ; 
                                (obj -> diffuse)[i] = diffuse[i] ; 
                                (obj -> specular)[i] = specular[i] ; 
                                (obj -> emission)[i] = emission[i] ;
                            }
                            obj -> shininess = shininess ; 
                            obj -> transform = transfstack.top() ;
                            obj -> type = cylinder;
                            obj -> baseRadius = values[0];
                            obj -> topRadius = values[1];
                            obj -> size = values[2];
                            obj -> slices = values[3];
                            obj -> stacks = values[4];
                            obj -> animation_state = anim_state;
                            obj -> test_collision = test_collision;
                            obj -> scale_size = scalestack.top();
                            obj -> bounding_type = "circle";
                            gettimeofday(&(obj -> timeUpdate), NULL);
                        }
                        ++num_static_objects ;
                    }
                }
                
                else if (cmd == "smooth_cube" || cmd == "train_wheel" || cmd == "train_head" || cmd == "train_connect" || cmd == "poison_cube") {
                    if (num_static_objects == maxobjects) // No more objects
                        cerr << "Reached Maximum Number of Objects " << num_static_objects << " Will ignore further objects\n" ;
                    else {
                        validinput = readvals(s, 0, values);
                        if (validinput) {
                            object *obj = &(static_objects[num_static_objects]);
                            for (i = 0; i < 4; i++) {
                                (obj -> ambient)[i] = ambient[i];
                                (obj -> diffuse)[i] = diffuse[i];
                                (obj -> specular)[i] = specular[i];
                                (obj -> emission)[i] = specular[i];
                            }
                            obj -> type = modelobj;
                            obj -> shininess = shininess;
                            obj -> transform = transfstack.top();
                            obj -> animation_state = anim_state;
                            obj -> test_collision = test_collision;
                            obj -> scale_size = scalestack.top();
                            gettimeofday(&(obj -> timeUpdate), NULL);
                        
                            if (cmd == "smooth_cube") {
                                obj -> name = ((std::string)("smooth_cube"));
                                obj -> file_path = ((std::string)("images/shapes/smooth_cube.obj"));
                                obj -> shape_sides = 4;
                                obj -> bounding_type = "square";
                                obj->max_x = obj->max_z = 2.0;
                                obj->min_x = obj->min_z = -2.0;
                                obj->max_y = 1.5;
                                obj->min_y = -1.5;
                            }
                            if (cmd == "train_wheel") {
                                obj -> name = ((std::string)("train_wheel"));
                                obj -> file_path = ((std::string)("images/train/train_wheel.obj"));
                                obj -> shape_sides = 4;
                                obj -> direction = glm::vec3 ( obj->transform * glm::vec4 (-1.0, 0.0, 0.0, 0.0)); // Model of wheel begins direction (-1, 0, 0).
                                obj -> position = glm::vec3 ( obj->transform * glm::vec4 (0.0, 0.0, 0.0, 1.0)); // Model of wheel begins position (0, 0, 0).
                                obj -> bounding_type = "none";
                            }
                            if (cmd == "train_head") {
                                obj -> name = ((std::string)("train_head"));
                                obj -> file_path = ((std::string)("images/train/train_head.obj"));
                                obj -> shape_sides = 4;
                                obj -> bounding_type = "train_head";
                            }
                            if (cmd == "train_connect") {
                                obj -> name = ((std::string)("train_connect"));
                                obj -> file_path = ((std::string)("images/train/train_connect.obj"));
                                obj -> shape_sides = 4;
                                obj -> bounding_type = "square";
                                obj->max_x = obj-> max_y = obj->max_z = 1.0;
                                obj->min_x = obj-> min_y = obj->min_z = -1.0;
                            }
                            if (cmd == "poison_cube") {
                                obj -> name = ((std::string)("poison_cube"));
                                obj -> file_path = ((std::string)("images/shapes/poison_cube.obj"));
                                obj -> shape_sides = 4;
                                obj -> bounding_type = "square";
                                obj->max_x = obj-> max_y = obj->max_z = 0.5;
                                obj->min_x = obj-> min_y = obj->min_z = -0.5;
                            }
                            ++num_static_objects;
                            ++num_obj_models;
                        }
                    }
                }
                
                else if (cmd == "disappear_cube" || cmd == "purple_coin") {
                    if (num_dynamic_objects == maxobjects) // No more objects
                        cerr << "Reached Maximum Number of Objects " << num_dynamic_objects << " Will ignore further objects\n" ;
                    else {
                        validinput = readvals(s, 0, values);
                        if (validinput) {
                            object *obj = new object();
                            for (i = 0; i < 4; i++) {
                                (obj -> ambient)[i] = ambient[i];
                                (obj -> diffuse)[i] = diffuse[i];
                                (obj -> specular)[i] = specular[i];
                                (obj -> emission)[i] = specular[i];
                            }
                            obj -> type = modelobj;
                            obj -> shininess = shininess;
                            obj -> transform = transfstack.top();
                            obj -> animation_state = anim_state;
                            obj -> test_collision = test_collision;
                            obj -> scale_size = scalestack.top();
                            gettimeofday(&(obj -> timeUpdate), NULL);                        
                            if (cmd == "disappear_cube") {
                                obj -> name = ((std::string)("disappear_cube"));
                                obj -> file_path = ((std::string)("images/shapes/disappear_cube.obj"));
                                obj -> shape_sides = 4;
                                obj -> position = glm::vec3 ( obj->transform * glm::vec4 (0.0, 0.0, 0.0, 1.0)); // Model of cube begins position (0, 0, 0).
                                obj -> bounding_type = "square";
                                obj->max_x = obj-> max_y = obj->max_z = 0.5;
                                obj->min_x = obj-> min_y = obj->min_z = -0.5;
                                obj->disappear = false;
                                obj->fraction_left = 1.0;
                            }
                            if (cmd == "purple_coin") {
                                obj -> name = ((std::string)("purple_coin"));
                                obj -> file_path = ((std::string)("images/shapes/coin.obj"));
                                obj -> shape_sides = 4;
                                obj -> position = glm::vec3 ( obj->transform * glm::vec4 (0.0, 0.0, 0.0, 1.0)); // Model of coin begins position (0, 0, 0).
                                obj -> bounding_type = "square";
                            }
                            dynamic_objects.push_back(*obj);
                        }
                    }
                }
                else if (cmd == "character") {
                    validinput = readvals(s, 0, values) ; 
                    if (validinput) {
                        object * obj = &(static_objects[num_static_objects]) ; 
                        obj -> size = 1; 
                        for (i = 0 ; i < 4 ; i++) {
                            (obj -> ambient)[i] = ambient[i] ; 
                            (obj -> diffuse)[i] = diffuse[i] ; 
                            (obj -> specular)[i] = specular[i] ; 
                            (obj -> emission)[i] = emission[i] ;
                        }
                        obj -> shininess = shininess ; 
                        obj -> transform = transfstack.top() ;
                        obj -> type = cube;
                        obj -> animation_state = anim_state;
                        obj -> test_collision = test_collision;
                        obj -> scale_size = scalestack.top();
                        gettimeofday(&(obj -> timeUpdate), NULL);
                        character = obj;
                        char_position = glm::vec3(0.0, 0.0, 0.025);
                        previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                        obj-> max_x = obj-> max_y = obj->max_z = 0.05;
                        obj-> min_x = obj-> min_y = obj->min_z = -0.05;
                        obj-> max_radius = 0.06;
                    }
                    ++num_static_objects ; 
                }
                
                else if (cmd == "animation_state") {
                    std::string value;
                    s >> value;
                    anim_state = value;
                }

                else if (cmd == "translate") {
                    validinput = readvals(s,3,values) ; 
                    if (validinput) { 
                        transfstack.top() = transfstack.top() * Transform::translate(values[0], values[1], values[2]);
                    }
                }
            
                else if (cmd == "scale") {
                    validinput = readvals(s,3,values) ; 
                    if (validinput) {
                        transfstack.top() = transfstack.top() * Transform::scale(values[0], values[1], values[2]);
                        scalestack.top() = glm::vec3(scalestack.top().x * values[0], scalestack.top().y * values[1], scalestack.top().z * values[2]);
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s,4,values) ; 
                    if (validinput) {
                        mat4 rotateMtx = mat4(Transform::rotate(values[3], vec3(values[0], values[1], values[2])));
                        transfstack.top() = transfstack.top() * rotateMtx;
                    }
                }
        
                // I include the basic push/pop code for matrix stacks
                else if (cmd == "pushTransform") {
                    transfstack.push(transfstack.top()) ; 
                    scalestack.push(scalestack.top());
                }
                else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) 
                        cerr << "Stack has no elements.  Cannot Pop\n" ; 
                    else transfstack.pop() ; 
                    if (scalestack.size() <= 1)
                        cerr << "Stack has no elements. Cannot Pop\n";
                    else scalestack.pop();
                }
        
                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n" ; 
                }
            }
            getline (in, str) ; 
        }

        // Set up initial position for eye, up and amount
        // As well as booleans 
        amount = 5;
        sx = sy = 1.0 ; // scales in x and y 
        tx = ty = 0.0 ; // translation in x and y  

        glEnable(GL_DEPTH_TEST);

    } else {
        cerr << "Unable to Open Input Data File " << filename << "\n" ; 
        throw 2 ; 
    }  
}
