/** Parser for .OBJ files **/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>

#include "ModelObj.h"


using namespace std;

// This is a very basic loadObj function. It only works when the obj files have only faces that are EITHER triangles or quads.
bool ModelObj::loadObj(string file_name, int shape, string name) {
  FILE * file = fopen(file_name.c_str(), "r");
  if (file == NULL) {
    printf("Cannot open the file.");
    return false;
  }

  // Valid File: Follow Procedures for Parsing. 
  std::ifstream infile(file_name.c_str());
  string type;

  // Temporary Vectors
  vector<float> temp_vertices;
  vector<float> temp_textures;
  vector<float> temp_normals;
  vector<int> temp_vertex_indices;
  vector<int> temp_texture_indices;
  vector<int> temp_normal_indices;
  
  while(infile>> type) {
    if (type == "v") { // vector 
      float temp;
      int i;
      for (i = 0; i < 3; i++) {
	infile >> temp;
	temp_vertices.push_back(temp);
      }
    } else if (type == "vt") { // texture
      float temp;
      int i;
      for (i = 0; i < 2; i++) {
	infile >> temp;
	temp_textures.push_back(temp);
      }
    } else if (type == "vn") { // normals 
      float temp;
      int i;
      for (i = 0; i < 3; i++) {
	infile >> temp;
	temp_normals.push_back(temp);
      }
    } else if (type == "f") { // face 
      int i;
      for ( i = 0; i < shape; i++ ) {
	string temp;
	int index_vert, index_text, index_norm;
	infile >> temp;
	parse_string(temp, index_vert, index_text, index_norm);
	temp_vertex_indices.push_back(index_vert);
	temp_texture_indices.push_back(index_text);
	temp_normal_indices.push_back(index_norm);
      }
    }
  }

  // All values are now in the temp vectors. We need to convert to arrays.
  int index = 0;
  float vertex_array[temp_vertices.size()];
  vector<float>::iterator float_it;
  for (float_it = temp_vertices.begin(); float_it < temp_vertices.end(); float_it++) {
    *(vertex_array+index) = *float_it;
    index++;
  }
  index = 0;
  float texture_array[temp_textures.size()];
  for (float_it = temp_textures.begin(); float_it < temp_textures.end(); float_it++) {
    *(texture_array+index) = *float_it;
    index++;
  }
  index = 0;
  float normal_array[temp_normals.size()];
  for (float_it = temp_normals.begin(); float_it < temp_normals.end(); float_it++) {
    *(normal_array+index) = *float_it;
    index++;
  }

  vertices = new float[temp_vertex_indices.size()*3];
  textures = new float[temp_texture_indices.size()*3];
  normals = new float[temp_normal_indices.size()*3];
  index = 0;
  vertex_indices = new int[temp_vertex_indices.size()];
  vector <int>::iterator int_it;
  for (int_it = temp_vertex_indices.begin(); int_it < temp_vertex_indices.end(); int_it++) {
    *(vertex_indices+index) = index;
    *(vertices+index*3) = vertex_array[(*int_it)*3];
    *(vertices+index*3+1) = vertex_array[(*int_it)*3+1];
    *(vertices+index*3+2) = vertex_array[(*int_it)*3+2];
    index++;
  }
  index = 0;
  texture_indices = new int[temp_texture_indices.size()];
  for (int_it = temp_texture_indices.begin(); int_it < temp_texture_indices.end(); int_it++) {
    *(texture_indices+index) = index;
    *(textures+index*2) = texture_array[(*int_it)*2];
    *(textures+index*2+1) = texture_array[(*int_it)*2+1];
    index++;
  }
  index = 0;
  normal_indices = new int[temp_normal_indices.size()];
  for (int_it = temp_normal_indices.begin(); int_it < temp_normal_indices.end(); int_it++) {
    *(normal_indices+index) = index;
    *(normals+index*3) = normal_array[(*int_it)*3];
    *(normals+index*3+1) = normal_array[(*int_it)*3 + 1];
    *(normals+index*3+2) = normal_array[(*int_it)*3 + 2];
    index++;
  }

  this->shape = shape;
  this->num_of_indices = temp_vertex_indices.size();
  this->name = name;

  return true;
}

// Given a string of a format a/b/c, parse this string so x = a, y = b, z = c. Elements are 0 if not specified.
// Assume s is well formed.
void ModelObj::parse_string(string s, int &x, int &y, int &z) {
  int index;
  string word;
  int val = 0;
  for (index = 0; index < s.size(); index++) {
    if (s[index] != '/') {
      word += s[index];
    } else {
      if (val == 0) {
	if (word.size() == 0) {
	  x = -1; // no value 
	} else {
	  x = atoi(word.c_str())-1;
	}
      } else if (val == 1) {
	if (word.size() == 0) {
	  y = -1; // no value  
	} else {
	  y = atoi(word.c_str())-1;
	}
      }
      word = "";
      val++;
    }
  }
  if (word.size() == 0) {
    z = -1; // no value 
  } else {
    z = atoi(word.c_str())-1;
  }
}
