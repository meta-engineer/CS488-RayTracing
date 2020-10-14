// Fall 2019

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include <math.h>

bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b) {
	glm::vec3 cp1 = glm::cross(b-a, p1-a);
	glm::vec3 cp2 = glm::cross(b-a, p2-a);
	if (glm::dot(cp1, cp2) >= 0) return true;
	return false;
}

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
}

bool Mesh::rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans) {
	
	rayOrigin = glm::vec3(glm::inverse(trans) * glm::vec4(rayOrigin, 1.0));
	rayPoint = glm::vec3(glm::inverse(trans) * glm::vec4(rayPoint, 1.0));
	// bounding box check first?
	// divide and conquer optimization?
	// transform each point and check for max?
	if (m_vertices.empty()) {
		std::cout << "Mesh failed: " << m_vertices.size() << ", " << m_faces.size() << std::endl;
		std::cout << "The obj file may not be in this directory." << std::endl;
		return false;
	}


	glm::vec3 maxBound = m_vertices.front();
	glm::vec3 minBound = m_vertices.back();
	
	for (std::vector<glm::vec3>::iterator pre_v = m_vertices.begin(); pre_v !=m_vertices.end(); ++pre_v) {
		glm::vec3 v = *pre_v;
		if (v.x > maxBound.x) maxBound.x = v.x;
		else if (v.x < minBound.x) minBound.x = v.x;
		if (v.y > maxBound.y) maxBound.y = v.y;
		else if (v.y < minBound.y) minBound.y = v.y;
		if (v.z > maxBound.z) maxBound.z = v.z;
		else if (v.z < minBound.z) minBound.z = v.z;
	}

	float t;
	glm::vec3 planeIntercept;
	bool interceptOverridden = false;

	for (int dim = 0; dim < 3; dim++) {
		for (int mod = 0; mod < 2; mod++) {
			t = ((minBound[dim]*((mod+1)%2) + maxBound[dim]*mod) - rayOrigin[dim]) 
				/ (rayPoint[dim] - rayOrigin[dim]);
			if (t<0) continue;
			planeIntercept = rayOrigin + t*(rayPoint - rayOrigin);
			if (interceptOverridden && glm::length2(intercept-rayOrigin) < glm::length2(planeIntercept-rayOrigin)) {
				continue;
			}
			if (glm::length2(planeIntercept-rayOrigin) < RAYEPS) {
				continue;
			}
			int a = (dim+2) % 3;
			int b = (dim+1) % 3;
			//if plane intercept is within bounds break out and start mesh intersect
			if (planeIntercept[a] > minBound[a] && planeIntercept[a] < maxBound[a] &&
				planeIntercept[b] > minBound[b] && planeIntercept[b] < maxBound[b]) {
				intercept = planeIntercept;
				normal = glm::vec3(0.0, 0.0, 0.0);
				normal[dim] = (2*mod) - 1;
				//must look for first for boundingbox render
				//goto endBoundBox;
				interceptOverridden = true;
			}
		}
	}
	// end of boundbox dimensions and no hit?p
	if (!interceptOverridden) return false;

	// for now, ass
	#ifdef RENDER_BOUNDING_VOLUMES
		intercept = glm::vec3(trans * glm::vec4(planeIntercept, 1.0));
		normal = glm::transpose(glm::mat3(glm::inverse(trans))) * normal;
		return true;
	#endif
	interceptOverridden = false;

	// else check against all Triangle members?
	//vector<vec3> m_verticies;
	//vector<Triangle> m_faces;
	//Triangle.v1, Triangle.v2, Triangle.v3
	// shortcut by checking box around each tri? No, intercepting plane is as cheap
	for (std::vector<Triangle>::iterator f = m_faces.begin(); f != m_faces.end(); ++f) {
		glm::vec3 b0 = m_vertices[f->v3] - m_vertices[f->v2];
		glm::vec3 b1 = m_vertices[f->v1] - m_vertices[f->v2];

		//build plane from face
		glm::vec3 n = glm::cross(b0, b1);
		// use any vertex (v1) to solve
		float D = -(n.x*m_vertices[f->v1].x + n.y*m_vertices[f->v1].y + n.z*m_vertices[f->v1].z);
		// sub ray equation into plane equation
		t = (-D - n.x*rayOrigin.x - n.y*rayOrigin.y - n.z*rayOrigin.z)
		/ (n.x*(rayPoint.x-rayOrigin.x) +n.y*(rayPoint.y-rayOrigin.y) +n.z*(rayPoint.z-rayOrigin.z));
		
		planeIntercept = rayOrigin + t*(rayPoint - rayOrigin);

		// if ever unsure, jsut google solutions....
		if (SameSide(planeIntercept, m_vertices[f->v1], m_vertices[f->v2], m_vertices[f->v3]) &&
			SameSide(planeIntercept, m_vertices[f->v2], m_vertices[f->v3], m_vertices[f->v1]) &&
			SameSide(planeIntercept, m_vertices[f->v3], m_vertices[f->v1], m_vertices[f->v2])) {
			if (!interceptOverridden || glm::length2(planeIntercept - rayOrigin) < glm::length2(intercept - rayOrigin)) {
				intercept = planeIntercept;
				normal = n;
				interceptOverridden = true; //set once one is found
			}
		}
	}

	//translate out of local space
	intercept = glm::vec3(trans * glm::vec4(intercept, 1.0));
	normal = glm::transpose(glm::mat3(glm::inverse(trans))) * normal;
	return interceptOverridden;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
