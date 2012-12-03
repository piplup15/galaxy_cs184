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
#include <math.h>
#include <vector>

using namespace std ; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp
#include "ModelObj.h"

// Keyboard variables
bool * key_states = new bool[256];
bool keyboard_locked;

// Animation Variables
struct timeval time_register_key, train_one_loop, train_two_loop, train_three_loop, train_four_loop, blue_coin_time, char_animation_time;
int train_one_counter, train_two_counter, train_three_counter, train_four_counter, blue_coin_counter;

// Warp Star Variables
GLfloat first_warp_star_t_val;
bool first_warp_star;

// Menu Variables
int num_blue_coins_collected;

// Collision Variables
bool on_train_one;
bool on_train_two;
bool on_train_three;
bool on_train_four;

void display(void) ;  // prototype for display function.
void loadTex(const char * filename, GLubyte textureLocation[256][256][3]);
void evaluateQuadraticBezierCurve(glm::vec3 & result, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, GLfloat t);
void handleFirstWarpStar();

// Simple Physics Engine - Handle Jumps & Collisions with Floor
void handleCharacterGravity();
void updateCharacterGravity();
bool objectTopCollision(object * obj, bool & updateGravity);

// Disappearing cubes
GLfloat rate_disappear;
void handleDisappearCube();

// Coin Collision 
GLfloat distBetweenTwoPoints(glm::vec3 p1, glm::vec3 p2);
void handleCoinCollision();


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

void evaluateQuadraticBezierCurve(glm::vec3 & result, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3, GLfloat t) {
    result.x = (1.0-t)*(1.0-t)*point1.x + 2.0*t*(1.0-t)*point2.x + t*t*point3.x;
    result.y = (1.0-t)*(1.0-t)*point1.y + 2.0*t*(1.0-t)*point2.y + t*t*point3.y;
    result.z = (1.0-t)*(1.0-t)*point1.z + 2.0*t*(1.0-t)*point2.z + t*t*point3.z;
}

bool approx_equals(float val1, float val2, float approximate) {
    return abs(val1 - val2) <= approximate;
}

// First Warp Star: Initial Position: (0, 16, 0.2), Final Position: (30, 100, -20), Control Point: (60, 50, 50)
void handleFirstWarpStar() {
    glm::vec3 point1 = glm::vec3(0.0, 16.0, 0.2);
    glm::vec3 point2 = glm::vec3(0.0, 100.0, 100.0);
    glm::vec3 point3 = glm::vec3(0.0, 150.0, 10.0);
    //glm::vec3 point3 = glm::vec3(-50.0, 70.0, -60.0);
    glm::vec3 result = glm::vec3(0.0,0.0,0.0);
    evaluateQuadraticBezierCurve(result, point1, point2, point3, first_warp_star_t_val);
    character->transform = Transform::translate(result.x - char_position.x, result.y - char_position.y, result.z - char_position.z) * character->transform;
    eye = glm::vec3(Transform::translate(result.x - char_position.x, result.y - char_position.y, result.z - char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
    center = glm::vec3(Transform::translate(result.x - char_position.x, result.y - char_position.y, result.z - char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
    char_position = glm::vec3(Transform::translate(result.x - char_position.x, result.y - char_position.y, result.z - char_position.z) * glm::vec4(char_position.x, char_position.y, char_position.z, 1.0));
    //cout << first_warp_star_t_val << endl;
    if (approx_equals(first_warp_star_t_val, 1.000, 0.001)) {
        character->transform = Transform::translate(point3.x - char_position.x, point3.y - char_position.y, point3.z - char_position.z) * character->transform;
        eye = glm::vec3(Transform::translate(point3.x - char_position.x, point3.y - char_position.y, point3.z - char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
        center = glm::vec3(Transform::translate(point3.x - char_position.x, point3.y - char_position.y, point3.z - char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
        glm::vec3(Transform::translate(point3.x - char_position.x, point3.y - char_position.y, point3.z - char_position.z) * glm::vec4(char_position.x, char_position.y, char_position.z, 1.0));
        keyboard_locked = false;
        first_warp_star = false;
        previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
        char_velocity.z = 0;
        return;
    }
    first_warp_star_t_val += 0.004;
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
    
    // UPDATE TIME COUNTERS
    if ((current_time.tv_sec - train_one_loop.tv_sec) * 1000000.0 + (current_time.tv_usec - train_one_loop.tv_usec) > 30000.0) {
        train_one_counter = 32;
        gettimeofday(&train_one_loop, NULL);
    }
    if ((current_time.tv_sec - train_two_loop.tv_sec) * 1000000.0 + (current_time.tv_usec - train_two_loop.tv_usec) > 30000.0) {
        train_two_counter = 32;
        gettimeofday(&train_two_loop, NULL);
    }
    if ((current_time.tv_sec - train_three_loop.tv_sec) * 1000000.0 + (current_time.tv_usec - train_three_loop.tv_usec) > 30000.0) {
        train_three_counter = 32;
        gettimeofday(&train_three_loop, NULL);
    }
    if ((current_time.tv_sec - train_four_loop.tv_sec) * 1000000.0 + (current_time.tv_usec - train_four_loop.tv_usec) > 30000.0) {
        train_four_counter = 32;
        gettimeofday(&train_four_loop, NULL);
    }
    
    if ((current_time.tv_sec - blue_coin_time.tv_sec) * 1000000.0 + (current_time.tv_usec - blue_coin_time.tv_usec) > 30000.0) {
        blue_coin_counter = 100 - num_blue_coins_collected;
        gettimeofday(&blue_coin_time, NULL);
    }
    
    // TRAIN ANIMATION
    if ((train_one_counter != 0 && on_train_one) || (train_two_counter != 0 && on_train_two)) {
        char_position = glm::vec3(Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * glm::vec4(char_position.x, char_position.y, char_position.z, 1.0));
        center = glm::vec3(Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * glm::vec4(center.x, center.y, center.z, 1.0));
        character->transform = Transform::translate(char_position.x, char_position.y, char_position.z) * glm::mat4(Transform::rotate(0.4, glm::vec3(0.0,0.0,1.0)))* Transform::translate(-char_position.x, -char_position.y, -char_position.z) * Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * character->transform;
        eye = center + eyeinit;
        previous_char_position = char_position;
    }
    if ((train_three_counter != 0 && on_train_three) || (train_four_counter != 0 && on_train_four)) {
        char_position = glm::vec3(Transform::translate(0.0, 11.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -11.5, 0.0) * glm::vec4(char_position.x, char_position.y, char_position.z, 1.0));
        center = glm::vec3(Transform::translate(0.0, 11.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -11.5, 0.0) * glm::vec4(center.x, center.y, center.z, 1.0));
        character->transform = Transform::translate(char_position.x, char_position.y, char_position.z) * glm::mat4(Transform::rotate(0.4, glm::vec3(0.0,0.0,1.0)))* Transform::translate(-char_position.x, -char_position.y, -char_position.z) * Transform::translate(0.0, 11.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -11.5, 0.0) * character->transform;
        eye = center + eyeinit;
        previous_char_position = char_position;
    }
    
    for (int i = 0; i < num_static_objects; i++) {
        object * obj = &(static_objects[i]);
        if (obj -> animation_state == "train_one_loop") {
            if (train_one_counter != 0) {
                obj->transform = Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * obj->transform;
                gettimeofday(&(obj->timeUpdate), NULL);
                train_one_counter--;
            }
        } else if (obj -> animation_state == "wheel_one_loop") {
            if (train_one_counter != 0) {
                obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * glm::mat4(Transform::rotate(5.0, glm::normalize(glm::cross(glm::vec3(0.0,0.0,1.0), obj->direction)))) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
                obj->transform = Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * obj->transform;
                
                obj->direction = glm::vec3(glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * glm::vec4(obj->direction.x, obj->direction.y, obj->direction.z, 0.0));
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
                obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * glm::mat4(Transform::rotate(5.0, glm::normalize(glm::cross(glm::vec3(0.0,0.0,1.0), obj->direction)))) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
                obj->transform = Transform::translate(0.0, 4.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -4.5, 0.0) * obj->transform;
                obj->direction = glm::vec3(glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * glm::vec4(obj->direction.x, obj->direction.y, obj->direction.z, 0.0));
                obj->position = glm::vec3(obj->transform * glm::vec4(0.0, 0.0, 0.0, 1.0));
                gettimeofday(&(obj->timeUpdate), NULL);
                train_two_counter--;
            }
        }
        if (obj -> animation_state == "train_three_loop") {
            if (train_three_counter != 0) {
                obj->transform = Transform::translate(0.0, 11.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -11.5, 0.0) * obj->transform;
                gettimeofday(&(obj->timeUpdate), NULL);
                train_three_counter--;
            }
        } else if (obj -> animation_state == "wheel_three_loop") {
            if (train_three_counter != 0) {
                obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * glm::mat4(Transform::rotate(5.0, glm::normalize(glm::cross(glm::vec3(0.0,0.0,1.0), obj->direction)))) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
                obj->transform = Transform::translate(0.0, 11.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -11.5, 0.0) * obj->transform;
                
                obj->direction = glm::vec3(glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * glm::vec4(obj->direction.x, obj->direction.y, obj->direction.z, 0.0));
                obj->position = glm::vec3(obj->transform * glm::vec4(0.0, 0.0, 0.0, 1.0));
                gettimeofday(&(obj->timeUpdate), NULL);
                train_three_counter--;
            }
        }
        if (obj -> animation_state == "train_four_loop") {
            if (train_four_counter != 0) {
                obj->transform = Transform::translate(0.0, 11.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -11.5, 0.0) * obj->transform; 
                gettimeofday(&(obj->timeUpdate), NULL);
                train_four_counter--;
            }
        } else if (obj -> animation_state == "wheel_four_loop") {
            if (train_four_counter != 0) {
                obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * glm::mat4(Transform::rotate(5.0, glm::normalize(glm::cross(glm::vec3(0.0,0.0,1.0), obj->direction)))) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
                obj->transform = Transform::translate(0.0, 11.5, 0.0) * glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(0.0, -11.5, 0.0) * obj->transform;
                obj->direction = glm::vec3(glm::mat4(Transform::rotate(-0.4, glm::vec3(0.0, 0.0, 1.0))) * glm::vec4(obj->direction.x, obj->direction.y, obj->direction.z, 0.0));
                obj->position = glm::vec3(obj->transform * glm::vec4(0.0, 0.0, 0.0, 1.0));
                gettimeofday(&(obj->timeUpdate), NULL);
                train_four_counter--;
            }
        }
    }
    
    // DISAPPEARING CUBE ANIMATION
    handleDisappearCube();
    
    // blue COIN ANIMATION
    for (std::vector<object>::iterator it = dynamic_objects.begin(); it != dynamic_objects.end(); ++it) {
        object * obj = &(*it);
        if (obj -> name == "blue_coin") {
            if (blue_coin_counter != 0) {
                obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * glm::mat4(Transform::rotate(10.0, glm::vec3(0.0, 0.0, 1.0))) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
                obj->position = glm::vec3(obj->transform * glm::vec4(0.0, 0.0, 0.0, 1.0));
                gettimeofday(&(obj->timeUpdate), NULL);
                blue_coin_counter--;
            }
        }
    }
    
    // COLLISION TEST - blue COINS
    handleCoinCollision();
    
    
}

GLfloat distBetweenTwoPoints(glm::vec3 p1, glm::vec3 p2) {
    return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) + (p2.z - p1.z)*(p2.z - p1.z));
}

void handleCoinCollision() {
    int index = 0;
    for (std::vector<object>::iterator it = dynamic_objects.begin(); it != dynamic_objects.end(); ++it) {
        object * obj = &(*it);
        if (obj -> name == "blue_coin") {
            if (distBetweenTwoPoints(char_position + glm::vec3(0.0, 0.0, (character->max_z - character->min_z)/2.0), obj->position) <= (coin_radius + character->max_radius)) {
                coin_indices.push_back(index);
                num_blue_coins_collected++;
            }
        }
        index++;
    }
    for (std::vector<int>::iterator index_it = coin_indices.end()-1; index_it != coin_indices.begin()-1; --index_it) {
        dynamic_objects.erase(dynamic_objects.begin() + (int)(*index_it));
    }
    coin_indices = std::vector<int>();
}

bool objectTopCollision(object * obj, bool & updateGravity) {
    if (obj->bounding_type == "square") {
        glm::vec3 transformed_old_char_position = glm::vec3(glm::inverse(obj->transform) * glm::vec4(previous_char_position.x, previous_char_position.y, previous_char_position.z, 1.0));
        glm::vec3 transformed_char_position = glm::vec3(glm::inverse(obj->transform) * glm::vec4(char_position.x, char_position.y, char_position.z, 1.0));
        glm::vec3 transformed_char_normal = glm::normalize(glm::vec3(glm::inverse(glm::transpose(glm::inverse(obj->transform))) * glm::vec4(0.0, 0.0, 1.0, 0.0)));
        if (approx_equals(transformed_char_normal.x, 1.0, 0.001)) {
            if (approx_equals(transformed_char_position.x, obj->max_x,  obj->max_x/20.0/obj->scale_size.x) || (transformed_old_char_position.x >= obj->max_x && transformed_char_position.x <= obj->max_x)) {
                if (transformed_char_position.y >= (obj->min_y + character->min_y) && transformed_char_position.y <= (obj->max_y + character->max_y) && transformed_char_position.z >= (obj->min_z + character->min_z) && transformed_char_position.z <= (obj->max_z + character->max_z)) {
                    if (obj->animation_state == "train_one_loop") {
                        on_train_one = true;
                    } else if (obj->animation_state == "train_two_loop") {
                        on_train_two = true;
                    } else if (obj->animation_state == "train_three_loop") {
                        on_train_three = true;
                    } else if (obj->animation_state == "train_four_loop") {
                        on_train_four = true;
                    }
                    updateGravity = false;
                    is_jumping = false;
                    char_velocity.z = 0.0;
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    char_position.z = (obj->transform * glm::vec4(obj->max_x, 0.0, 0.0, 1.0)).z;
                    character -> transform = Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * character -> transform;
                    eye = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
                    center = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    return true;
                }
            }
        } else if (approx_equals(transformed_char_normal.y, 1.0, 0.001)) {
            if (approx_equals(transformed_char_position.y, obj->max_y, obj->max_y/20.0/obj->scale_size.y) || (transformed_old_char_position.y >= obj->max_y && transformed_char_position.y <= obj->max_y)) {
                if (transformed_char_position.x >= (obj->min_x + character->min_x) && transformed_char_position.x <= (obj->max_x + character->max_x) && transformed_char_position.z >= (obj->min_z + character->min_z) && transformed_char_position.z <= (obj->max_z + character->max_z)) {
                    if (obj->animation_state == "train_one_loop") {
                        on_train_one = true;
                    } else if (obj->animation_state == "train_two_loop") {
                        on_train_two = true;
                    } else if (obj->animation_state == "train_three_loop") {
                        on_train_three = true;
                    } else if (obj->animation_state == "train_four_loop") {
                        on_train_four = true;
                    }
                    updateGravity = false;
                    is_jumping = false;
                    char_velocity.z = 0.0;
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    char_position.z = (obj->transform * glm::vec4(0.0, obj->max_y, 0.0, 1.0)).z;
                    character -> transform = Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * character -> transform;
                    eye = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
                    center = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    return true;
                }
            }
        } else if (approx_equals(transformed_char_normal.z, 1.0, 0.001)) {
            if ( approx_equals(transformed_char_position.z, obj->max_z, obj->max_z/20.0/obj->scale_size.z) || (transformed_old_char_position.z >= obj->max_z && transformed_char_position.z <= obj->max_z)){
                if (transformed_char_position.x >= (obj->min_x + character->min_x) && transformed_char_position.x <= (obj->max_x + character->max_x) && transformed_char_position.y >= (obj->min_y + character->min_y) && transformed_char_position.y <= (obj->max_y + character->max_y)) {
                    if (obj->animation_state == "train_one_loop") {
                        on_train_one = true;
                    } else if (obj->animation_state == "train_two_loop") {
                        on_train_two = true;
                    } else if (obj->animation_state == "train_three_loop") {
                        on_train_three = true;
                    } else if (obj->animation_state == "train_four_loop") {
                        on_train_four = true;
                    }
                    updateGravity = false;
                    is_jumping = false;
                    char_velocity.z = 0.0;
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    char_position.z = (obj->transform * glm::vec4(0.0, 0.0, obj->max_z, 1.0)).z;
                    character -> transform = Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * character -> transform;
                    eye = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
                    center = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    return true;
                }
            }
        } else if (approx_equals(transformed_char_normal.x, -1.0, 0.001)) {
            if (approx_equals(transformed_char_position.x, obj->min_x, obj->max_x/20.0/obj->scale_size.x) || (transformed_old_char_position.x <= obj->min_x && transformed_char_position.x >= obj->min_x)) {
                if (transformed_char_position.y >= (obj->min_y + character->min_y) && transformed_char_position.y <= (obj->max_y + character->max_y) && transformed_char_position.z >= (obj->min_z + character->min_z) && transformed_char_position.z <= (obj->max_z + character->max_z)) {
                    if (obj->animation_state == "train_one_loop") {
                        on_train_one = true;
                    } else if (obj->animation_state == "train_two_loop") {
                        on_train_two = true;
                    } else if (obj->animation_state == "train_three_loop") {
                        on_train_three = true;
                    } else if (obj->animation_state == "train_four_loop") {
                        on_train_four = true;
                    }
                    updateGravity = false;
                    is_jumping = false;
                    char_velocity.z = 0.0;
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    char_position.z = (obj->transform * glm::vec4(obj->min_x, 0.0, 0.0, 1.0)).z;
                    character -> transform = Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * character -> transform;
                    eye = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
                    center = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    return true;
                }
            }
        } else if (approx_equals(transformed_char_normal.y, -1.0, 0.001)) {
            if (approx_equals(transformed_char_position.y, obj->min_y, obj->max_y/20.0/obj->scale_size.y) || (transformed_old_char_position.y <= obj->min_y && transformed_char_position.y >= obj->min_y)) {
                if (transformed_char_position.x >= (obj->min_x + character->min_x) && transformed_char_position.x <= (obj->max_x + character->max_x) && transformed_char_position.z >= (obj->min_z + character->min_z) && transformed_char_position.z <= (obj->max_z + character->max_z)) {
                    if (obj->animation_state == "train_one_loop") {
                        on_train_one = true;
                    } else if (obj->animation_state == "train_two_loop") {
                        on_train_two = true;
                    } else if (obj->animation_state == "train_three_loop") {
                        on_train_three = true;
                    } else if (obj->animation_state == "train_four_loop") {
                        on_train_four = true;
                    }
                    updateGravity = false;
                    is_jumping = false;
                    char_velocity.z = 0.0;
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    char_position.z = (obj->transform * glm::vec4(0.0, obj->min_y, 0.0, 1.0)).z;
                    character -> transform = Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * character -> transform;
                    eye = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
                    center = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    return true;
                }
            }
        } else if (approx_equals(transformed_char_normal.z, -1.0, 0.001)) {
            if (approx_equals(transformed_char_position.z, obj->min_z, obj->max_z/20.0/obj->scale_size.z) || (transformed_old_char_position.z <= obj->min_z && transformed_char_position.z >= obj->min_z)) {
                if (transformed_char_position.x >= (obj->min_x + character->min_x) && transformed_char_position.x <= (obj->max_x + character->max_x) && transformed_char_position.y >= (obj->min_y + character->min_y) && transformed_char_position.y <= (obj->max_y + character->max_y)) {
                    if (obj->animation_state == "train_one_loop") {
                        on_train_one = true;
                    } else if (obj->animation_state == "train_two_loop") {
                        on_train_two = true;
                    } else if (obj->animation_state == "train_three_loop") {
                        on_train_three = true;
                    } else if (obj->animation_state == "train_four_loop") {
                        on_train_four = true;
                    }
                    updateGravity = false;
                    is_jumping = false;
                    char_velocity.z = 0.0;
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    char_position.z = (obj->transform * glm::vec4(0.0, 0.0, obj->min_z, 1.0)).z;
                    character -> transform = Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * character -> transform;
                    eye = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
                    center = glm::vec3( Transform::translate(0.0, 0.0, char_position.z - previous_char_position.z) * glm::vec4(center.x, center.y, center.z, 1.0));
                    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
                    return true;
                }
            }
        }
    }
    return false;
}

void handleDisappearCube() {
    int index = 0;
    for (std::vector<object>::iterator it = dynamic_objects.begin() ; it != dynamic_objects.end(); ++it) {
        object * obj = &(*it);
        if (obj->name == "disappear_cube" && obj->disappear == true) {
            obj->transform = Transform::translate(obj->position.x, obj->position.y, obj->position.z) * Transform::scale(1-rate_disappear, 1-rate_disappear, 1.0) * Transform::translate(-obj->position.x, -obj->position.y, -obj->position.z) * obj->transform;
            obj->fraction_left = obj->fraction_left * (1 - rate_disappear);
            if (obj->fraction_left < 0.1) {
                disappear_cube_indices.push_back(index);
            }
        }
        index++;
    }
    for (std::vector<int>::iterator index_it = disappear_cube_indices.end()-1; index_it != disappear_cube_indices.begin()-1; --index_it) {
        dynamic_objects.erase(dynamic_objects.begin() + (int)(*index_it));
    }
    disappear_cube_indices = std::vector<int>();
}

void handleCharacterGravity () {
    bool updateGravity = true;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    if ((current_time.tv_sec - char_animation_time.tv_sec) * 1000000.0 + (current_time.tv_usec - char_animation_time.tv_usec) > 20000.0) {
        for (int i = 0; i < num_static_objects; i++) {
            object * obj = &(static_objects[i]);
            if (obj->test_collision) {
                if (objectTopCollision(obj, updateGravity)) {
                    break;
                }
            }
        }
        
        for (std::vector<object>::iterator it = dynamic_objects.begin(); it != dynamic_objects.end(); ++it) {
            object * obj = &(*it);
            if (obj->test_collision) {
                if (objectTopCollision(obj, updateGravity)) {
                    if (obj->name == "disappear_cube" && obj->disappear == false) {
                        obj->disappear = true;
                    }
                }
            }
        }
        
        if (updateGravity) {
            is_jumping = true;
            on_train_one = false;
            on_train_two = false;
            on_train_three = false;
            on_train_four = false;
            if (char_velocity.z > -0.5) {
                char_velocity.z -= 0.008;
            }
            updateCharacterGravity();
        }
        gettimeofday(&char_animation_time, NULL);
    }
}

void updateCharacterGravity() {
    character -> transform = Transform::translate(char_velocity.x, char_velocity.y, char_velocity.z) * character -> transform;
    previous_char_position = glm::vec3(char_position.x, char_position.y, char_position.z);
    char_position.z += char_velocity.z;
    eye = glm::vec3(Transform::translate(char_velocity.x, char_velocity.y, char_velocity.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
    center = glm::vec3(Transform::translate(char_velocity.x, char_velocity.y, char_velocity.z) * glm::vec4(center.x, center.y, center.z, 1.0));
}


void printHelp() {
    std::cout << "\npress 'h' to print this message again.\n";
    std::cout << "press 'w' to move forward.\n";
    std::cout << "press 's' to move backward.\n";
    std::cout << "press 'a' to move left.\n";
    std::cout << "press 'd' to move right.\n";
    std::cout << "press 'SPACE' to jump.\n";
    std::cout << "press 'LEFT ARROW' to shift camera left.\n";
    std::cout << "press 'RIGHT ARROW' to shift camera right.\n";

}


void keyboard(unsigned char key, int x, int y) {
	key_states[key] = true;
    if (key == 'h') {
        printHelp();
    }
    if (key == 27) { // Escape to quit                                                                                                                                                                   
        exit(0);
    }
}

void keyUp (unsigned char key, int x, int y) {
    key_states[key] = false;
}    


void idleFunc() {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    cout << char_position.x << " , " << char_position.y << " , " << char_position.z << endl;
    if ((current_time.tv_sec - time_register_key.tv_sec)*1000000.0+(current_time.tv_usec - time_register_key.tv_usec) > 20000.0 && !keyboard_locked) {
        if (key_states['w']) { // forward movement
            glm::vec3 direction = glm::normalize(glm::vec3(center.x - eye.x, center.y - eye.y, 0));
            eye = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
            character -> transform = Transform::translate(direction.x/25.0, direction.y/25.0, 0) * character->transform;
            char_position = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(char_position.x, char_position.y, char_position.z, 1));
        }
        if (key_states['s']) { // backward movement
            glm::vec3 direction = glm::normalize(glm::vec3(-center.x + eye.x, -center.y + eye.y, 0));
            eye = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
            character -> transform = Transform::translate(direction.x/25.0, direction.y/25.0, 0) * character->transform;
            char_position = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(char_position.x, char_position.y, char_position.z, 1));
        }
        if (key_states['a']) { // left movement
            glm::vec3 direction = glm::normalize(glm::vec3(-center.y + eye.y, center.x - eye.x, 0));
            eye = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
            character -> transform = Transform::translate(direction.x/25.0, direction.y/25.0, 0) * character->transform;
            char_position = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(char_position.x, char_position.y, char_position.z, 1));
        }
        if (key_states['d']) { // right movement
            glm::vec3 direction = glm::normalize(glm::vec3(center.y - eye.y, -center.x + eye.x, 0));
            eye = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(eye.x, eye.y, eye.z, 1));
            center = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(center.x, center.y, center.z, 1));
            character -> transform = Transform::translate(direction.x/25.0, direction.y/25.0, 0) * character->transform;
            char_position = glm::vec3(Transform::translate(direction.x/25.0, direction.y/25.0, 0) * glm::vec4(char_position.x, char_position.y, char_position.z, 1));
        }
        if (key_states[' ']) { // space, jump
            if (!is_jumping) {
                char_velocity.z = jump_velocity;
                is_jumping = true;
                on_train_one = false;
                on_train_two = false;
                on_train_three = false;
                on_train_four = false;
                updateCharacterGravity();
            }
        }
        if (char_position.x >= -0.1 && char_position.x <= 0.1 && char_position.y >= 15.9 && char_position.y <= 16.1 && char_position.z >= -0.2 && char_position.z <= 0.2) {
            character->transform = Transform::translate(-char_position.x, 16.0 - char_position.y, 0.2 - char_position.z) * character->transform;
            char_position = glm::vec3(0.0, 16.0, 0.2);
            eye = glm::vec3(Transform::translate(-char_position.x, 16.0 - char_position.y, 0.2 - char_position.z) * glm::vec4(eye.x, eye.y, eye.z, 1.0));
            center = glm::vec3(0.0, 16.0, 0.2);
            keyboard_locked = true;
            first_warp_star = true;
        }
        gettimeofday(&time_register_key, NULL);
    }
    
    if (first_warp_star && (current_time.tv_sec - time_register_key.tv_sec)*1000000.0+(current_time.tv_usec - time_register_key.tv_usec) > 20000.0) {
        handleFirstWarpStar();
        gettimeofday(&time_register_key, NULL);
    }
    handleAnimation();
    if (!keyboard_locked) {
        handleCharacterGravity();
    }
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
    gettimeofday(&train_three_loop,NULL);
    train_three_counter = 32; // train 3 has 32 objects
    gettimeofday(&train_four_loop,NULL);
    train_four_counter = 32; // train 4 has 32 objects
    gettimeofday(&blue_coin_time,NULL);
    blue_coin_counter = 100; // initially 100 blue coins
    gettimeofday(&char_animation_time,NULL);
    keyboard_locked = false;
    
    first_warp_star_t_val = 0.0;
    
    char_position = vec3(0.0,0.0,0.0);
    previous_char_position = vec3(0.0,0.0,0.0);
    char_velocity = vec3(0.0,0.0,0.0);
    max_run_velocity = 0.04;
    jump_velocity = 0.10;
    is_jumping = false;
    
    num_static_objects = 0;
    num_dynamic_objects = 0;
    dynamic_objects = std::vector<object>();
    
    num_blue_coins_collected = 0;
    
    on_train_one = false;
    on_train_two = false;
    on_train_three = false;
    on_train_four = false;
    
    test_collision = false;
    
    rate_disappear = 0.005;
    disappear_cube_indices = std::vector<int>();
    coin_indices = std::vector<int>();
    
    coin_radius = 0.12;
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
