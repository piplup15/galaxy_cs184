#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <GL/glut.h>
#include <sstream>
#include <stdio.h>
#include <assert.h>

#include "Texture.h"

using namespace std;

/* Loads and stores texture and assigns the texture ID 
 * Returns true if sucessfully set, false otherwise (debugging purposes)
 */
bool Texture::set(const char * filename){
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

	//For debugging, currently not used (always false)
	return false;
}
