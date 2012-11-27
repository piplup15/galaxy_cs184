#include <iostream>
#include <vector>

class ModelObj {
 public: 
  bool loadObj (std::string file_name, int shape, std::string name);

  float* vertices;
  float* textures;
  float* normals;
  int* vertex_indices;
  int* texture_indices;
  int* normal_indices;

  int num_of_indices;
  int shape;
  std::string name;

  void parse_string(std::string s, int &x, int &y, int &z);
};
