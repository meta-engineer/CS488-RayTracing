// Fall 2019

#pragma once

#include <glm/glm.hpp>

#define RAYEPS 0.00005

class Primitive {
public:
  virtual ~Primitive();
  
  virtual bool rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans);
  
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual bool rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans);
};

class Cube : public Primitive {
public:
  virtual ~Cube();
  virtual bool rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

  // ignores trans (ray coords should be in world coords)
  virtual bool rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans);
private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

  // ignores trans (ray coords should be in world coords)
  virtual bool rayInterceptPrim(glm::vec3 rayOrigin, glm::vec3 rayPoint, glm::vec3 & intercept, glm::vec3 & normal, glm::mat4 trans);
private:
  glm::vec3 m_pos;
  double m_size;
};

