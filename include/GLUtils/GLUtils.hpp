#ifndef _GLUTILS_HPP__
#define _GLUTILS_HPP__

#include <cstdlib>
#include <sstream>
#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <GL/glew.h>

#include "GLUtils/Program.hpp"
#include "GLUtils/VBO.hpp"
#include "GameException.h"


namespace GLUtils {

inline void checkGLErrors(const char* file, unsigned int line) {
	GLenum ASSERT_GL_err = glGetError(); 
    if( ASSERT_GL_err != GL_NO_ERROR ) { 
		std::stringstream ASSERT_GL_string; 
		ASSERT_GL_string << file << '@' << line << ": OpenGL error:" 
             << std::hex << ASSERT_GL_err << " " << gluErrorString(ASSERT_GL_err); 
			 THROW_EXCEPTION( ASSERT_GL_string.str() ); 
    } 
}
#define CHECK_GL_ERROR() GLUtils::checkGLErrors(__FILE__, __LINE__)


inline std::string readFile(std::string file) {
	int length;
	std::string buffer;
	std::string contents;

	std::ifstream is;
	is.open(file.c_str());

	if (!is.good()) {
		std::string err = "Could not open ";
		err.append(file);
		THROW_EXCEPTION(err);
	}

	// get length of file:
	is.seekg (0, std::ios::end);
	length = static_cast<int>(is.tellg());
	is.seekg (0, std::ios::beg);

	// reserve memory:
	contents.reserve(length);

	// read data
	while(getline(is,buffer)) {
		contents.append(buffer);
		contents.append("\n");
	}
	is.close();

	return contents;
}

inline glm::vec3 normaliseVector(glm::vec3 v)
{
	float length = glm::sqrt(glm::pow(v.x, 2.0f) + glm::pow(v.y, 2.0f) + glm::pow(v.z, 2.0f));
	return v / length;
}

inline std::string mat3toString(glm::mat3 matrix)
{
	float m11 = matrix[0][0];
	float m12 = matrix[1][0];
	float m13 = matrix[2][0];
	float m21 = matrix[0][1];
	float m22 = matrix[1][1];
	float m23 = matrix[2][1];
	float m31 = matrix[0][2];
	float m32 = matrix[1][2];
	float m33 = matrix[2][2];
	
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) <<
		"| " << m11 << " " << m12 << " " << m13 << " |\n" <<
		"| " << m21 << " " << m22 << " " << m23 << " |\n" <<
		"| " << m31 << " " << m32 << " " << m33 << " |\n";
	
	return ss.str();
	
}

inline std::string mat4toString(glm::mat4 matrix)
{
	float m11 = matrix[0][0];
	float m12 = matrix[1][0];
	float m13 = matrix[2][0];
	float m14 = matrix[3][0];
	float m21 = matrix[0][1];
	float m22 = matrix[1][1];
	float m23 = matrix[2][1];
	float m24 = matrix[3][1];
	float m31 = matrix[0][2];
	float m32 = matrix[1][2];
	float m33 = matrix[2][2];
	float m34 = matrix[3][2];
	float m41 = matrix[0][3];
	float m42 = matrix[1][3];
	float m43 = matrix[2][3];
	float m44 = matrix[3][3];

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) <<
		"| " << m11 << " " << m12 << " " << m13 << " " << m14 << " |\n" <<
		"| " << m21 << " " << m22 << " " << m23 << " " << m24 << " |\n" <<
		"| " << m31 << " " << m32 << " " << m33 << " " << m34 << " |\n" <<
		"| " << m41 << " " << m42 << " " << m43 << " " << m44 << " |\n";

	return ss.str();

}

}; //Namespace GLUtils

#endif