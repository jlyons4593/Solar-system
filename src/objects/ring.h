// Joseph Lyons 11/11/2023
// ring.h
#ifndef RING_H
#define RING_H

#include "glad.h"
#include "wrapper_glfw.h"
#include <glm/glm.hpp>

class Ring {
private:
    glm::vec3 colour;
    GLfloat radius, length;
    GLuint definition;
    GLuint ringBufferObject, ringNormals, ringColours, ringElementbuffer;
    GLuint numberOfVertices;

    GLuint attribute_v_coord;
    GLuint attribute_v_normal;
    GLuint attribute_v_colours;

    void defineVertices();

public:
    Ring();
    Ring(glm::vec3 c);
    ~Ring();

    void makeRing();
    void drawRing(int drawmode);
};

#endif

