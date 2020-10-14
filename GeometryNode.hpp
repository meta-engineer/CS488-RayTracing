// Fall 2019

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	virtual bool rayInterceptHeirScene(glm::vec3 rayOrigin, glm::vec3 rayPoint, SceneNode *& collided, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 p_trans);

	void setMaterial( Material *material );

	Material *m_material;
	Primitive *m_primitive;
};
