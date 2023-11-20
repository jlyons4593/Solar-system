// Joseph Lyons 11/11/2023
// #include <ring.h>
//   
// using namespace glm;
// using namespace std;
//   
// const float PI = 3.141592653589f;  /* pi */
//
// #include <iostream>
//
//
// Ring::Ring() : Ring(glm::vec3(1.f, 1.f, 1.f), 0.5f, 1.0f, 100)
// {
// }
//
// Ring::Ring(glm::vec3 c, GLfloat innerRad, GLfloat outerRad, GLuint def) : colour(c), innerRadius(innerRad), outerRadius(outerRad), definition(def)
// {
//     attribute_v_coord = 0;
//     attribute_v_colours = 1;
//     attribute_v_normal = 2;
//
//     numberOfVertices = definition * 4;
//
//     makeRing();
// }
//
// Ring::~Ring()
// {
//     glDeleteBuffers(1, &ringBufferObject);
//     glDeleteBuffers(1, &ringNormals);
//     glDeleteBuffers(1, &ringColours);
//     glDeleteBuffers(1, &ringElementbuffer);
// }
//
// void Ring::makeRing()
// {
//     defineVertices();
//
//     GLuint pindices[4 * 101]; // Enough for two triangle fans and a triangle strip
//     for (int i = 0; i < 101; i++)
//     {
//         pindices[i] = i;
//         pindices[i + 101] = i + 101;
//         pindices[i + 202] = i;
//         pindices[i + 303] = i + 101;
//     }
//
//     glGenBuffers(1, &this->ringElementbuffer);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ringElementbuffer);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pindices), pindices, GL_STATIC_DRAW);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
// }
//
// void Ring::defineVertices()
// {
//     vec3 vertices[402];
//     vec3 normals[402];
//     vec3 colour[402];
//
//     // number of vertices for each circle
//     GLuint numVertices = definition;
//
//     // define vertices for the outer circle
//     for (GLuint i = 0; i < numVertices; i++)
//     {
//         GLfloat theta = (2 * PI) / numVertices * i;
//
//         GLfloat x = outerRadius * cos(theta);
//         GLfloat y = 0.0f;
//         GLfloat z = outerRadius * sin(theta);
//
//         vertices[i] = vec3(x, y, z);
//         normals[i] = vec3(0.0, 1.0, 0.0);
//         colour[i] = this->colour;
//     }
//
//     // define vertices for the inner circle
//     for (GLuint i = 0; i < numVertices; i++)
//     {
//         GLfloat theta = (2 * PI) / numVertices * i;
//
//         GLfloat x = innerRadius * cos(theta);
//         GLfloat y = 0.0f;
//         GLfloat z = innerRadius * sin(theta);
//
//         vertices[numVertices + i] = vec3(x, y, z);
//         normals[numVertices + i] = vec3(0.0, -1.0, 0.0);
//         colour[numVertices + i] = this->colour;
//     }
//
//     /* Create the vertex buffer for the ring */
//     glGenBuffers(1, &this->ringBufferObject);
//     glBindBuffer(GL_ARRAY_BUFFER, this->ringBufferObject);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//     glGenBuffers(1, &this->ringNormals);
//     glBindBuffer(GL_ARRAY_BUFFER, ringNormals);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(normals), &normals[0], GL_STATIC_DRAW);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//     /* Store the colours in buffer objects */
//     glGenBuffers(1, &this->ringColours);
//     glBindBuffer(GL_ARRAY_BUFFER, ringColours);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(colour), &colour[0], GL_STATIC_DRAW);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
// }
//
// void Ring::drawRing(int drawmode)
// {
//     glEnableVertexAttribArray(attribute_v_coord);
//     glBindBuffer(GL_ARRAY_BUFFER, ringBufferObject);
//     glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
//
//     glEnableVertexAttribArray(attribute_v_colours);
//     glBindBuffer(GL_ARRAY_BUFFER, this->ringColours);
//     glVertexAttribPointer(attribute_v_colours, 3, GL_FLOAT, GL_FALSE, 0, 0);
//
//     glEnableVertexAttribArray(attribute_v_normal);
//     glBindBuffer(GL_ARRAY_BUFFER, ringNormals);
//     glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
//
//     glPointSize(3.f);
//
//     if (drawmode == 1)
//         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//     else
//         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//     if (drawmode == 2)
//     {
//         glDrawArrays(GL_POINTS, 0, numberOfVertices);
//     }
//     else
//     {
//         int numfanvertices = definition;
//         int numsidevertices = numfanvertices * 2;
//         int side_offset = numberOfVertices * 2;
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ringElementbuffer);
//
//         glDrawElements(GL_TRIANGLE_FAN, numfanvertices, GL_UNSIGNED_INT, (GLvoid *)0);
//         glDrawElements(GL_TRIANGLE_FAN, numfanvertices, GL_UNSIGNED_INT, (GLvoid *)(numfanvertices * sizeof(GLuint)));
//         glDrawElements(GL_TRIANGLE_STRIP, side_offset, GL_UNSIGNED_INT, (GLvoid *)(numsidevertices * sizeof(GLuint)));
//     }
// }
