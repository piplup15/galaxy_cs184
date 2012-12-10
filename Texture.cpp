#include <iostream>
#include <fstream>
#include <string>
#include <GL/glut.h>
#include <sstream>
#include <stdio.h>
#include <assert.h>

#include "Transform.h"
#include "variables.h"
#include "Texture.h"

using namespace std;

/* Loads and stores texture and assigns the texture ID 
 * Returns true if sucessfully set, false otherwise (debugging purposes)
 */
bool Texture::set(const char * filename){
	//Set shading to inactive
	this->shaded = false;

	//Set the name of the texture
	this->name = texNames[texNumInUse] ;
	texNumInUse++;

	//Load the texture file and save to this texture obj
	int i,j,k ;
	FILE * fp ; 
	GLint err ; 
	assert(fp = fopen(filename,"rb")) ;
	fscanf(fp,"%*s %*d %*d %*d%*c") ;
	for (i = 0 ; i < 256 ; i++)
		for (j = 0 ; j < 256 ; j++)
			for (k = 0 ; k < 3 ; k++)
				fscanf(fp,"%c",&(this->fileContent[i][j][k])) ;
	fclose(fp) ;  

	//Init texture in Opengl
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  	glEnable(GL_TEXTURE_2D) ; 
  	glBindTexture (GL_TEXTURE_2D, this->name) ; 

  	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, this->fileContent) ;
  	glDisable(GL_TEXTURE_2D) ;

	//For debugging, currently not used (always false)
	return false;
}
