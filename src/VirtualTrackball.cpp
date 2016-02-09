#include "VirtualTrackball.h"
#include <cmath>
#include <iostream>

glm::mat4 quatToMat4(glm::quat m_q) {
	/**
	  * Implement so that the we generate the correct transformation
	  * matrix from the input quaternion
	  */
	return glm::mat4(1.0);
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
	
	std::cout << "Angle: " << theta << std::endl;
	std::cout << "Axis: " << axis_of_rotation.x << " " << axis_of_rotation.y << " " << axis_of_rotation.z << std::endl;

	return quatToMat4(quat_new);
}

void VirtualTrackball::setWindowSize(int w, int h) {
	this->w = w;
	this->h = h;
}

glm::vec2 VirtualTrackball::getNormalizedWindowCoordinates(int x, int y) {
	glm::vec2 coord = glm::vec2(0.0f);
	/**
	  * Here, you need to find the normalized window coordinates
	  */

	std::cout << "Normalized coordinates: " << coord.x << ", " << coord.y << std::endl;

	return coord;
}

glm::vec3 VirtualTrackball::getClosestPointOnUnitSphere(int x, int y) {
	glm::vec2 normalized_coords;
	glm::vec3 point_on_sphere;
	float k;

	normalized_coords = getNormalizedWindowCoordinates(x, y);
	
	/**
	  * Find the point on the unit sphere here from the
	  * normalized window coordinates
	  */

	std::cout << "Point on sphere: " << point_on_sphere.x << ", " << point_on_sphere.y << ", " << point_on_sphere.z << std::endl;

	return point_on_sphere;
}