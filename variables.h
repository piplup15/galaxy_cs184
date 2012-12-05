/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.

#include <sys/time.h>
#include <vector>

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif 

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 

EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 center ; 
EXTERN int w, h ; 
EXTERN float fovy ; 

EXTERN float mousex ;
EXTERN float mousey ;
 
EXTERN GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {view, translate, scale} transop ; // which operation to transform 
enum shape {cube, sphere, teapot, disk, cylinder, modelobj, char_obj} ;
EXTERN float sx, sy ; // the scale in x and y 
EXTERN float tx, ty ; // the translation in x and y

// Lighting parameter array, similar to that in the fragment shader
const int numLights = 10 ; 
EXTERN GLfloat lightposn [4*numLights] ; // Light Positions
EXTERN GLfloat lightcolor[4*numLights] ; // Light Colors
EXTERN GLfloat lightransf[4*numLights] ; // Lights transformed by modelview
EXTERN int numused ;                     // How many lights are used 

// Materials (read from file) 
// With multiple objects, these are colors for each.
EXTERN GLfloat ambient[4] ; 
EXTERN GLfloat diffuse[4] ; 
EXTERN GLfloat specular[4] ; 
EXTERN GLfloat emission[4] ; 
EXTERN GLfloat shininess ; 

// For multiple objects, read from a file.  
const int maxobjects = 2000 ; 
EXTERN int num_static_objects ; 
EXTERN struct object {
  shape type ; 
  GLfloat size ;
  GLfloat ambient[4] ; 
  GLfloat diffuse[4] ; 
  GLfloat specular[4] ;
  GLfloat emission[4] ; 
  GLfloat shininess ;
  mat4 transform ;
  std::string name; // only necessary for .obj files.
  std::string file_path; // only necessary for .obj files.
  int shape_sides; // either 3 for tri or 4 for quad.
  GLfloat innerRadius; // Disk inner radius
  GLfloat outerRadius; // Disk outer radius
  GLfloat baseRadius; // Disk inner radius
  GLfloat topRadius; // Disk outer radius
  GLint slices; // Disk/cylinder slices
  GLint loops; // Disk loops
  GLint stacks; // Cylinder stacks
  std::string animation_state;
  struct timeval timeUpdate;
  glm::vec3 direction; // Only used for objects which require direction for animation.
  glm::vec3 position; // Only used for objects which require position for animation.
  bool test_collision;
  std::string bounding_type;
    
  glm::vec3 scale_size;

  // Bounding
  GLfloat max_x, max_y, max_z, min_x, min_y, min_z;
  GLfloat max_radius;
    
  // Disappear Cube
  bool disappear;
  GLfloat fraction_left;
    
} static_objects[maxobjects] ;

EXTERN int num_dynamic_objects;
EXTERN std::vector<object> dynamic_objects;

EXTERN object * character;

EXTERN glm::vec3 char_position;
EXTERN glm::vec3 previous_char_position;
EXTERN glm::vec3 char_velocity;
EXTERN GLfloat max_run_velocity;
EXTERN GLfloat jump_velocity;
EXTERN bool is_jumping;

EXTERN int char_tail_rotation;
EXTERN int char_tail_rotation_amount;
EXTERN int char_feet_rotation;
EXTERN int char_feet_rotation_amount;
EXTERN glm::vec3 char_direction_relative;
EXTERN glm::vec3 char_direction_absolute;

EXTERN std::string anim_state;

// Variables to set uniform params for lighting fragment shader 
EXTERN GLuint lightcol ; 
EXTERN GLuint lightpos ; 
EXTERN GLuint numusedcol ; 
EXTERN GLuint enablelighting ; 
EXTERN GLuint ambientcol ; 
EXTERN GLuint diffusecol ; 
EXTERN GLuint specularcol ; 
EXTERN GLuint emissioncol ; 
EXTERN GLuint shininesscol ; 

EXTERN GLfloat backgroundColor[4];

const int max_obj_models = 40; // per scene.
EXTERN int num_obj_models;

EXTERN bool test_collision;
EXTERN std::vector<int> disappear_cube_indices;
EXTERN std::vector<int> coin_indices;

EXTERN GLfloat coin_radius;

EXTERN bool hide_tail;


