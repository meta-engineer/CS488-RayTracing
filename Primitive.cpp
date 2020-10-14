// Fall 2019

#include "Primitive.hpp"
#include <glm/gtx/norm.hpp>
#include "polyroots.hpp"
#include <math.h>

#include <iostream>

Primitive::~Primitive()
{
}

bool Primitive::rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans) {
	// should always delegate to subclasses
	return false;
}

Sphere::~Sphere()
{
}

bool Sphere::rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans) {
	// inverse transform ray
	rayOrigin = glm::vec3(glm::inverse(trans) * glm::vec4(rayOrigin, 1.0));
	rayPoint = glm::vec3(glm::inverse(trans) * glm::vec4(rayPoint, 1.0));

	// pretend m_pos, m_radius are unit (ray has been transformed
	glm::vec3 m_pos = glm::vec3(0.0);
	double m_radius = 1;
	// calculate intercept t based on sphere equation
	int rootIndex;
	double ts[2] = {0.0, 0.0};
	glm::vec3 sphereIntercepts[2];

	// OI wikipedia, what's good?
	double A =   glm::dot(rayPoint-rayOrigin, rayPoint-rayOrigin);
	double B = 2*glm::dot(rayPoint-rayOrigin, rayOrigin-m_pos);
	double C =   glm::dot(rayOrigin-m_pos, rayOrigin-m_pos) - pow(m_radius,2);

	// return codes? 0 is no roots?
	// 1 is 1 root ([2] is unchanged), 2 is 2 roots
	if (2 != quadraticRoots(A, B, C, ts)) {
		return false;
	}

	for (int ti = 0; ti < 2; ti++) {
		sphereIntercepts[ti] = rayOrigin + (float)ts[ti]*(rayPoint - rayOrigin);
	}

	//get closest non negative
	if (ts[0] < 0 && ts[1] < 0) return false;
	else if (ts[0] < 0) rootIndex = 1;
	else if (ts[1] < 0) rootIndex = 0;
	else {
		if (glm::length2(sphereIntercepts[0]-rayOrigin) < 
			glm::length2(sphereIntercepts[1]-rayOrigin)) {
			// intercept 0 is closer
			rootIndex = 0;
		} else {
			rootIndex = 1;
		}

		//if rootIndex is too close to eye, take other one
		if (glm::length2(sphereIntercepts[rootIndex]-rayOrigin) < RAYEPS) {
			rootIndex = (rootIndex+1) % 2;
		}
	}

	// set intercept, set normal, and return true
	// transform intercept back to true space
	intercept = glm::vec3(trans * glm::vec4(sphereIntercepts[rootIndex], 1.0));
	normal = glm::transpose(glm::mat3(glm::inverse(trans))) * (float)100.0*(sphereIntercepts[rootIndex] - m_pos);
	// geometry node passed buffer values it will validate closest
	return true;
}

Cube::~Cube()
{
}

bool Cube::rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans) {
	// no bounding box for non-mesh prims
	// m_pos, m_size
	glm::vec3 m_pos = glm::vec3(0.0);
	double m_size = 1.0;

	// inverse transform ray
	rayOrigin = glm::vec3(glm::inverse(trans) * glm::vec4(rayOrigin, 1.0));
	rayPoint = glm::vec3(glm::inverse(trans) * glm::vec4(rayPoint, 1.0));

	float t;
	glm::vec3 planeIntercept;
	bool interceptOverridden = false;

	for (int dim = 0; dim < 3; dim++) {
		for (int mod = 0; mod < 2; mod++) {
			// build plane and transform it, then intersect with ray?
			// calc t based on dim/mod
			t = (m_pos[dim] + (mod*m_size) - rayOrigin[dim]) / (rayPoint[dim] - rayOrigin[dim]);
			if (t<0) continue;
			planeIntercept = rayOrigin + t*(rayPoint - rayOrigin);
			// if not first intercept then short circuit if intercept is too far away
			// intercepts are only relative to self (Geometrynode will check others)
			// therefor no transformation needed
			if (interceptOverridden && glm::length2(intercept - rayOrigin) < glm::length2(planeIntercept - rayOrigin)) {
				continue;
			}
			// if too close to eye then skip
			if (glm::length2(planeIntercept-rayOrigin) < RAYEPS) {
				continue;
			}
			int a = (dim+2) % 3;
			int b = (dim+1) % 3;
			// otherwise check if within plane segment bounds
			if ((planeIntercept[a] > m_pos[a] && planeIntercept[a] < m_pos[a] + m_size)
			&& (planeIntercept[b] > m_pos[b] && planeIntercept[b] < m_pos[b] + m_size)) {
				//already guarenteed "closer"
				intercept = planeIntercept;
				normal = glm::vec3(0.0, 0.0, 0.0);
				normal[dim] = (2*mod) - 1;
				interceptOverridden = true;
			}

		}
	}

	// give best internal intercept to GeometryNode (will check against other nodes)
	intercept = glm::vec3(trans*glm::vec4(intercept, 1.0));
	normal = glm::transpose(glm::mat3(glm::inverse(trans))) * normal;
	return interceptOverridden;
}

NonhierSphere::~NonhierSphere()
{
}

bool NonhierSphere::rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans) {
	// m_pos, m_radius

	// actually heirarchical???
	// inverse transform ray
	rayOrigin = glm::vec3(glm::inverse(trans) * glm::vec4(rayOrigin, 1.0));
	rayPoint = glm::vec3(glm::inverse(trans) * glm::vec4(rayPoint, 1.0));

	// calculate intercept t based on sphere equation
	int rootIndex;
	double ts[2] = {0.0, 0.0};
	glm::vec3 sphereIntercepts[2];
	
	// OI wikipedia, what's good?
	double A =   glm::dot(rayPoint-rayOrigin, rayPoint-rayOrigin);
	double B = 2*glm::dot(rayPoint-rayOrigin, rayOrigin-m_pos);
	double C =   glm::dot(rayOrigin-m_pos,    rayOrigin-m_pos) - pow(m_radius,2);

	// return codes? 0 is no roots?
	// 1 is 1 root ([2] is unchanged), 2 is 2 roots
	if (2 != quadraticRoots(A, B, C, ts)) {
		return false;
	}

	for (int ti = 0; ti < 2; ti++) {
		sphereIntercepts[ti] = rayOrigin + (float)ts[ti]*(rayPoint - rayOrigin);
	}

	if (ts[0] < 0 && ts[1] < 0) return false;
	else if (ts[0] < 0) rootIndex = 1;
	else if (ts[1] < 0) rootIndex = 0;
	else {
		if (glm::length2(sphereIntercepts[0]-rayOrigin) < 
			glm::length2(sphereIntercepts[1]-rayOrigin)) {
			// intercept 0 is closer
			rootIndex = 0;
		} else {
			rootIndex = 1;
		}

		//if rootIndex is too close to eye, take other one
		if (glm::length2(sphereIntercepts[rootIndex]-rayOrigin) < RAYEPS) {
			rootIndex = (rootIndex+1) % 2;
		}
	}
	
	// check for closest?
	

	// else set intercept, set normal, and return true
	// give best internal intercept to GeometryNode (will check against other nodes)
	intercept = glm::vec3(trans*glm::vec4(sphereIntercepts[1], 1.0));
	normal = glm::transpose(glm::mat3(glm::inverse(trans))) * (float)100.0*(sphereIntercepts[rootIndex] - m_pos);
	//intercept = sphereIntercepts[rootIndex];
	//normal = (sphereIntercepts[rootIndex] - m_pos);
	return true;
}

NonhierBox::~NonhierBox()
{
}

bool NonhierBox::rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans) {
	// no bounding box for non-mesh prims

	// actually heirarchical???
	// inverse transform ray
	rayOrigin = glm::vec3(glm::inverse(trans) * glm::vec4(rayOrigin, 1.0));
	rayPoint = glm::vec3(glm::inverse(trans) * glm::vec4(rayPoint, 1.0));

	// m_pos, m_size
	float t;
	glm::vec3 planeIntercept;
	bool interceptOverridden = false;

	for (int dim = 0; dim < 3; dim++) {
		for (int mod = 0; mod < 2; mod++) {
			// calc t based on dim/mod
			t = (m_pos[dim] + (mod*m_size) - rayOrigin[dim]) / (rayPoint[dim] - rayOrigin[dim]);
			if (t<0) continue;
			planeIntercept = rayOrigin + t*(rayPoint - rayOrigin);			

			// if not first intercept then short circuit if intercept is too far away
			if (interceptOverridden && glm::length2(intercept - rayOrigin) < glm::length2(planeIntercept - rayOrigin)) {
				continue;
			}
			// if too close to eye then skip
			if (glm::length2(planeIntercept-rayOrigin) < RAYEPS) {
				continue;
			}
			int a = (dim+2) % 3;
			int b = (dim+1) % 3;
			// otherwise check if within plane segment bounds
			if ((planeIntercept[a] > m_pos[a] && planeIntercept[a] < m_pos[a] + m_size) &&
			    (planeIntercept[b] > m_pos[b] && planeIntercept[b] < m_pos[b] + m_size)) {
				//already guarenteed "closer"
				intercept = planeIntercept;
				normal = glm::vec3(0.0, 0.0, 0.0);
				normal[dim] = (2*mod) - 1;
				interceptOverridden = true;
			}

		}
	}

	// give best internal intercept to GeometryNode (will check against other nodes)
	intercept = glm::vec3(trans*glm::vec4(intercept, 1.0));
	normal = glm::transpose(glm::mat3(glm::inverse(trans))) * normal;
	return interceptOverridden;
}

