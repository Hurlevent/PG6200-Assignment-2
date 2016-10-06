#include "VirtualTrackball.h"
#include <cmath>
#include <iostream>
#include "GLUtils/GLUtils.hpp"

glm::mat4 quatToMat4(glm::quat m_q) {
	/**
	  * Implement so that the we generate the correct transformation
	  * matrix from the input quaternion
	  */
	float m11, m12, m13;
	float m21, m22, m23;
	float m31, m32, m33;

	m11 = 1 - 2 * glm::pow(m_q.y, 2.0f) - 2 * glm::pow(m_q.z, 2.0f);
	m21 = 2 * m_q.x * m_q.y + 2 * m_q.w * m_q.z;
	m31 = 2 * m_q.x * m_q.z - 2 * m_q.w * m_q.y;
	m12 = 2 * m_q.x * m_q.y - 2 * m_q.w * m_q.z;
	m22 = 1 - 2 * glm::pow(m_q.x, 2.0f) - 2 * glm::pow(m_q.z, 2.0f);
	m32 = 2 * m_q.y * m_q.z + 2 * m_q.w * m_q.x;
	m13 = 2 * m_q.x * m_q.z + 2 * m_q.w * m_q.y;
	m23 = 2 * m_q.y * m_q.z - 2 * m_q.w * m_q.x;
	m33 = 1 - 2 * glm::pow(m_q.x, 2.0f) - 2 * glm::pow(m_q.y, 2.0f);

	glm::mat3x3 rot_matrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);

	//std::cout << "Rotation matrix: \n" << GLUtils::mat3toString(rot_matrix).c_str() << std::endl;

	return glm::mat4(rot_matrix);
}

VirtualTrackball::VirtualTrackball() {
	quat_old.w = 1.0;
	quat_old.x = 0.0;
	quat_old.y = 0.0;
	quat_old.z = 0.0;
	rotating = false;
}

VirtualTrackball::~VirtualTrackball() {}

void VirtualTrackball::rotateBegin(int x, int y) {
	rotating = true;
	point_on_sphere_begin = getClosestPointOnUnitSphere(x, y);
}

void VirtualTrackball::rotateEnd(int x, int y) {
	rotating = false;
	quat_old = quat_new;
}

glm::mat4 VirtualTrackball::rotate(int x, int y) {

	//If not rotating, simply return the old rotation matrix
	if (!rotating) return quatToMat4(quat_old);

	glm::vec3 point_on_sphere_end; //Current point on unit sphere
	glm::vec3 axis_of_rotation; //axis of rotation
	float theta = 0.0f; //angle of rotation

	point_on_sphere_end = getClosestPointOnUnitSphere(x, y);

	/**
	  * Find axis of rotation and angle here. Construct the
	  * rotation quaternion using glm helper functions
	  */

	// Calculating the CROSS product between the source and destination vectors, in order to find the axis if rotation
	axis_of_rotation.x = point_on_sphere_end.y * point_on_sphere_begin.z - point_on_sphere_end.z * point_on_sphere_begin.y;
	axis_of_rotation.y = -(point_on_sphere_end.x * point_on_sphere_begin.z - point_on_sphere_end.z * point_on_sphere_begin.x);
	axis_of_rotation.z = point_on_sphere_end.x * point_on_sphere_begin.y - point_on_sphere_end.y * point_on_sphere_begin.x;


	// Normalize length of axis_of_rotation
	axis_of_rotation = GLUtils::normaliseVector(axis_of_rotation);
	//float length_of_axis_of_rotation = glm::sqrt(glm::pow(axis_of_rotation.x, 2.0f) + glm::pow(axis_of_rotation.y, 2.0f) + glm::pow(axis_of_rotation.z, 2.0f));

	//std::cout << "Length of axis_of_rotation: " << length_of_axis_of_rotation << std::endl;

	// Calculating the DOT product of the source and destination vectors
	float dot = point_on_sphere_begin.x * point_on_sphere_end.x +
		point_on_sphere_begin.y * point_on_sphere_end.y +
		point_on_sphere_begin.z * point_on_sphere_end.z;
	std::cout << "DOT:: " << dot << std::endl;
	
	// Calculation the angle between the source and destination vectors
	theta = glm::degrees(glm::acos(dot));

	std::cout << "rotate: " << std::endl;
	std::cout << "Angle: " << theta << std::endl;
	std::cout << "Axis: " << axis_of_rotation.x << " " << axis_of_rotation.y << " " << axis_of_rotation.z << std::endl;

	//glm::quat rotate(glm::cos(theta / 2), glm::sin(theta / 2) * axis_of_rotation);
	//glm::quat rotate = glm::angleAxis(theta, axis_of_rotation);
	//quat_new = rotate;
	//quat_old = quat_new;
	quat_new = glm::inverse(quat_old) * glm::rotate(quat_old, theta, axis_of_rotation) * quat_old;

	return quatToMat4(quat_new);
}

void VirtualTrackball::setWindowSize(int w, int h) {
	this->w = w;
	this->h = h;
}


// FUNGERER !!!
glm::vec2 VirtualTrackball::getNormalizedWindowCoordinates(int x, int y) {
	glm::vec2 coord = glm::vec2(0.0f);
	/**
	  * Here, you need to find the normalized window coordinates
	  */

	coord.x = ((x / static_cast<float>(w)) - 0.5f);
	coord.y = (0.5f - (y / static_cast<float>(h)));

	std::cout << "NormalizedWindowCoordinates: " << std::endl;
	std::cout << "Normalized coordinates: " << coord.x << ", " << coord.y << std::endl;

	return coord;
}

// DENNE TROR JEG AT FUNGERER! Litt skeptisk med den "else"'en da
glm::vec3 VirtualTrackball::getClosestPointOnUnitSphere(int x, int y) {
	glm::vec2 normalized_coords;
	glm::vec3 point_on_sphere;
	float k;

	normalized_coords = getNormalizedWindowCoordinates(x, y);
	
	k = glm::sqrt(glm::pow(normalized_coords.x, 2.0f) + glm::pow(normalized_coords.y, 2.0f));
	/**
	  * Find the point on the unit sphere here from the
	  * normalized window coordinates
	  */
	
	if(k <= 0.5f)
	{
		point_on_sphere.x = normalized_coords.x * 2;
		point_on_sphere.y = normalized_coords.y * 2;
		point_on_sphere.z = glm::sqrt(1 - (4 * glm::pow(k, 2.0f)));
	} else
	{
		point_on_sphere.x = normalized_coords.x / k;
		point_on_sphere.y = normalized_coords.y / k;

	}

	std::cout << "ClosestPointOnUnitSphere: " << "length: " << k << std::endl;
	std::cout << "Point on sphere: " << point_on_sphere.x << ", " << point_on_sphere.y << ", " << point_on_sphere.z << std::endl;

	return point_on_sphere;
}