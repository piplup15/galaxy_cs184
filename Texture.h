#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <sstream>

class Texture {
 public: 

  GLubyte fileContent[256][256][3] ; //Contents of texture file loaded onto mem 
  GLuint name ; //Index used in opengl
  bool shaded;  //Determines whether to use shading along with this texture obj

  /* Loads and stores texture and assigns the texture ID */
  bool set(const char * fileName);

private:

  GLuint setTexName(void);
};
