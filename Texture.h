#include <iostream>
#include <vector>

class Texture {
 public: 

  GLubyte fileContent[256][256][3] ; //Contents of texture file loaded onto mem 
  GLuint texIndex ;
};
