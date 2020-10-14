// Fall 2019

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    // intercepts ray with self and (recursively) children
    //     returns true if there is interception
    //     intercepted node is stored in collided and
    //     closest intercept to rayOrigin is stored in intercept
    // *should accept optional max distance?
    virtual bool rayInterceptScene(glm::vec3 rayOrigin, glm::vec3 rayPoint, 
			SceneNode *& collided, glm::vec3 & intercept, glm::vec3 & normal);
    virtual bool rayInterceptHeirScene(glm::vec3 rayOrigin, glm::vec3 rayPoint, 
			SceneNode *& collided, glm::vec3 & intercept, glm::vec3 & normal,
			glm::mat4 p_trans);

private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
