/* sphere.h
 Example class to create a generic sphere objects
 Resolution can be controlled by settign nuber of latitudes and longitudes
 Iain Martin October 2019
*/

#pragma once

#include "wrapper_glfw.h"
#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
	Sphere();
	~Sphere();

	void makeSphere(GLuint numlats, GLuint numlongs, glm::vec3 RGB=glm::vec3(1.f, 1.f, 1.f));
	void drawSphere(int drawmode);
  void makeRing(GLfloat innerRadius, GLfloat outerRadius, GLuint numSegments);
  void drawRing(int drawmode);
	// Define vertex buffer objects names (e.g as globals)
	GLuint sphereBufferObject;
	GLuint sphereNormals;
	GLuint sphereColours;
  GLuint sphereLightingEnabled;
	GLuint elementbuffer;

	GLuint attribute_v_coord;
	GLuint attribute_v_normal;
	GLuint attribute_v_colours;

	GLuint numspherevertices;
	GLuint numlats;
	GLuint numlongs;
	int drawmode;
  std::vector<GLfloat> sphereVertices;
  std::vector<GLuint> sphereIndices;
  std::vector<GLfloat> ringVertices;

private:
	void makeUnitSphere(GLfloat *pVertices);
  void makeUnitRing(GLfloat *pVertices, GLfloat innerRadius, GLfloat outerRadius, GLuint numSegments);
};
