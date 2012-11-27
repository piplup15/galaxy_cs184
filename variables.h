/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  

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

EXTERN float mousex;
EXTERN float mousey; 

EXTERN bool useGlu; // Toggle use of "official" opengl/glm transform vs user 
EXTERN GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {view, translate, scale} transop ; // which operation to transform 
enum shape {cube, sphere, teapot, cylinder, modelobj} ;
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
const int maxobjects = 1000 ; 
EXTERN int numobjects ; 
EXTERN struct object {
  shape type ; 
  GLfloat size ;
  GLfloat ambient[4] ; 
  GLfloat diffuse[4] ; 
  GLfloat specular[4] ;
  GLfloat emission[4] ; 
  GLfloat shininess ;
  mat4 transform ; 
  GLdouble radius; // for cylinder
  GLdouble height; // for cylinder
  std::string name; // only necessary for .obj files.
  std::string file_path; // only necessary for .obj files.
  int shape_sides; // either 3 for tri or 4 for quad.
} objects[maxobjects] ;
EXTERN object character; 
EXTERN bool use_char;
EXTERN vec3 char_direction;
EXTERN vec3 char_position;
EXTERN int char_frame;

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

//Scene variables
EXTERN GLfloat backgroundColor[4];

//Obj files - since only few models on screen (20 MB limit), we can only have a few .obj models (textures are expensive). Hence, an array is suitable to store .obj vertices/texture coords/normals.
const int max_obj_models = 40; // per scene.
EXTERN int num_obj_models;

//Textures
EXTERN bool textureOn;

EXTERN GLuint dirt;
EXTERN GLubyte washington[256][256][3] ;
EXTERN GLubyte spiral[256][256][3] ;
EXTERN GLubyte fireplace[256][256][3] ;
EXTERN GLuint texNames[9] ; 
EXTERN GLuint isTex ; 

EXTERN GLuint isBump ; 

EXTERN GLubyte anim0[256][256][3] ;
EXTERN GLubyte anim1[256][256][3] ;
EXTERN GLubyte anim2[256][256][3] ;
EXTERN GLubyte anim3[256][256][3] ;
EXTERN GLubyte anim4[256][256][3] ;

EXTERN GLubyte bump0[256][256][3] ;

//Animated tex time variable
EXTERN GLuint texAnimInterval;

//Used for transitioning animation
EXTERN bool isTransitioning;
EXTERN GLuint skeletonIndex;
