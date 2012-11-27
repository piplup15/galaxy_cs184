// Transform.cpp: implementation of the Transform class.


#include "Transform.h"


// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {
  // YOUR CODE FOR HW2 HERE
  // Axis-Angle Formula: Icos(theta) + (1-cos(theta))*a*atranspose + sin(theta)*(A*)
  vec3 n_axis = glm::normalize(axis); // n_axis stands for normalized_axis
  float radians = degrees * pi / 180.0;
  mat3 identity_matrix = mat3(1.0);
  mat3 a_atranspose = mat3( n_axis[0]*n_axis[0], n_axis[0]*n_axis[1], n_axis[0]*n_axis[2], //1st Row
			    n_axis[1]*n_axis[0], n_axis[1]*n_axis[1], n_axis[1]*n_axis[2], //2nd Row
			    n_axis[2]*n_axis[0], n_axis[2]*n_axis[1], n_axis[2]*n_axis[2]);//3rd Row
  mat3 duality_matrix_of_a = mat3(0.0       , -n_axis[2], n_axis[1],
				  n_axis[2] , 0.0       , -n_axis[0],
				  -n_axis[1], n_axis[0] , 0.0);
  mat3 rotateMtx = (glm::cos(radians) * identity_matrix) + ((1 - glm::cos(radians))*a_atranspose) + (glm::sin(radians)*duality_matrix_of_a);
  return glm::transpose(rotateMtx); // accounts for row major

}

void Transform::left(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW2 HERE
  vec3 cross_axis = glm::cross(up, eye);
  vec3 axis = glm::normalize(glm::cross(eye, cross_axis));
  mat3 rotateMtx = rotate(degrees, axis);
  eye = rotateMtx * eye;
  up = rotateMtx * up;
}

void Transform::up(float degrees, vec3& eye, vec3& up) {
  // YOUR CODE FOR HW2 HERE 
  vec3 cross_axis = glm::cross(eye,up);
  mat3 rotateMtx = rotate(degrees, cross_axis);
  eye = rotateMtx * eye;
  up = rotateMtx * up;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) {
  // YOUR CODE FOR HW2 HERE
  vec3 w = glm::normalize(eye-center);
  vec3 u = glm::normalize(glm::cross(up, eye-center));
  vec3 v = glm::normalize(glm::cross(eye-center, u));
  mat4 lookAtMtx = mat4(u[0], u[1], u[2], -eye[0]*u[0] - eye[1]*u[1] - eye[2]*u[2],
		     v[0], v[1], v[2], -eye[0]*v[0] - eye[1]*v[1] - eye[2]*v[2],
		     w[0], w[1], w[2], -eye[0]*w[0] - eye[1]*w[1] - eye[2]*w[2],
		     0.0 , 0.0 , 0.0 , 1.0 );
  return lookAtMtx;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  // YOUR CODE FOR HW2 HERE
  float fovy_rad = fovy * pi / 180.0;
  mat4 perspectiveMtx = mat4(1.0/(aspect*glm::tan(fovy_rad/2.0)), 0.0                       , 0.0                           , 0.0,
			     0.0                                , 1.0/glm::tan(fovy_rad/2.0), 0.0                           , 0.0,
			     0.0                                , 0.0                       , -(zFar + zNear)/(zFar - zNear), -2*(zFar)*(zNear)/(zFar - zNear),
			     0.0                                , 0.0                       , -1.0                          , 0.0);
  return perspectiveMtx;
}


mat4 Transform::scale(const float &sx, const float &sy, const float &sz) {
  // YOUR CODE FOR HW2 HERE
  mat4 scalingMtx = mat4(sx , 0.0, 0.0, 0.0,
			 0.0, sy , 0.0, 0.0,
			 0.0, 0.0, sz , 0.0,
			 0.0, 0.0, 0.0, 1.0);
  return scalingMtx; // transpose is the same.
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) {
  // YOUR CODE FOR HW2 HERE
  mat4 translateMtx = mat4(1.0, 0.0, 0.0, tx,
			   0.0, 1.0, 0.0, ty,
			   0.0, 0.0, 1.0, tz,
			   0.0, 0.0, 0.0, 1.0);
  return glm::transpose(translateMtx); // accounts for row major
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) {
  vec3 x = glm::cross(up,zvec) ; 
  vec3 y = glm::cross(zvec,x) ; 
  vec3 ret = glm::normalize(y) ; 
  return ret ; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
