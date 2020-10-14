// Fall 2019

#include "GeometryNode.hpp"
#include <glm/gtx/norm.hpp>

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

bool GeometryNode::rayInterceptHeirScene(glm::vec3 rayOrigin, glm::vec3 rayPoint, SceneNode *& collided, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 p_trans) {
	// will intercept ray with all my children and take best result
	bool res = SceneNode::rayInterceptHeirScene(rayOrigin, rayPoint, collided, intercept, normal, p_trans);
	// m_primitive can be heir cube/sphere/mesh, or nonheir cube/sphere
	// must be virtually delegated
	glm::vec3 potentialIntercept;
	glm::vec3 potentialNormal;
	if (m_primitive->rayInterceptPrim(rayOrigin, rayPoint, potentialIntercept, potentialNormal, p_trans * trans)) {
		
		//ensure eye isn't too close
		if (glm::length2(potentialIntercept-rayOrigin) < RAYEPS) return res;
		
		// prim returns true, add self to collided (check for closest dist)
		// could store interception if all hits are required
		if (collided==NULL||glm::length2(potentialIntercept - rayOrigin) <= glm::length2(intercept - rayOrigin)) {
			res = true;
			intercept = potentialIntercept;
			normal = potentialNormal;
			collided = this;
		}
	}
	// return best result of my prim or my children's
	return res;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}
