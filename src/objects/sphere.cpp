/* sphere.cpp
 Example class to create a generic sphere objects
 Resolution can be controlled by settign nuber of latitudes and longitudes
 Iain Martin November 2016
 Adapted by Joseph Lyons in an attempt to create Ring shapes
*/

#include "sphere.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>  // This header provides the glm::pi<float>() function

/* I don't like using namespaces in header files but have less issues with them in
seperate cpp files */
using namespace std;

/* Define the vertex attributes for vertex positions and normals.
Make these match your application and vertex shader
You might also want to add colours and texture coordinates */
Sphere::Sphere()
{
	attribute_v_coord = 0;
	attribute_v_colours = 1;
	attribute_v_normal = 2;
	numspherevertices = 0;		// We set this when we know the numlats and numlongs values in makeSphere
	drawmode = 0;
	sphereBufferObject = 0;
	sphereNormals = 0;
	sphereColours = 0;
	elementbuffer = 0;
	numlats = 0;
	numlongs = 0;
}

Sphere::~Sphere()
{
}


/* Make a sphere from two triangle fans (one at each pole) and triangle strips along latitudes */
/* This version uses indexed vertex buffers for both the fans at the poles and the latitude strips */
void Sphere::makeSphere(GLuint numlats, GLuint numlongs, glm::vec3 rgb)
{
	GLuint i, j;
	/* Calculate the number of vertices required in sphere */
	GLuint numvertices = 2 + ((numlats - 1) * numlongs);

	// Store the number of sphere vertices in an attribute because we need it later when drawing it
	numspherevertices = numvertices;
	this->numlats = numlats;
	this->numlongs = numlongs;

	// Create the temporary arrays to stro
	GLfloat* pVertices = new GLfloat[numvertices * 3];
	GLfloat* pColours = new GLfloat[numvertices * 4];

    makeUnitSphere(pVertices);

	/* Define colours as the x,y,z components of the sphere vertices */
	for (i = 0; i < numvertices; i++)
	{
		pColours[i * 4] = rgb.r;	// pVertices[i * 4];
		pColours[i * 4 + 1] = rgb.g;	// pVertices[i * 4 + 1];
		pColours[i * 4 + 2] = rgb.b;	// pVertices[i * 4 + 2];
		pColours[i * 4 + 3] = 1.f;	// pVertices[i * 4 + 3];
	}

	/* Generate the vertex buffer objects */
	glGenBuffers(1, &sphereBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 3, pVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the normals in a buffer objects */
	glGenBuffers(1, &sphereNormals);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 3, pVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Store the colours in a buffer objects */
	glGenBuffers(1, &sphereColours);
	glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* numvertices * 4, pColours, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Calculate the number of indices in our index array and allocate memory for it */
	GLuint numindices = ((numlongs * 2) + 2) * (numlats - 1) + ((numlongs + 2) * 2);
	GLuint* pindices = new GLuint[numindices];

	// fill "indices" to define triangle strips
	GLuint index = 0;		// Current index

	// Define indices for the first triangle fan for one pole
	for (i = 0; i < numlongs + 1; i++)
	{
		pindices[index++] = i;
	}
	pindices[index++] = 1;	// Join last triangle in the triangle fan

	GLuint start = 1;		// Start index for each latitude row
	for (j = 0; j < numlats - 2; j++)
	{
		for (i = 0; i < numlongs; i++)
		{
			pindices[index++] = start + i;
			pindices[index++] = start + i + numlongs;
		}
		pindices[index++] = start; // close the triangle strip loop by going back to the first vertex in the loop
		pindices[index++] = start + numlongs; // close the triangle strip loop by going back to the first vertex in the loop

		start += numlongs;
	}

	// Define indices for the last triangle fan for the south pole region
	for (i = numvertices - 1; i > (numvertices - numlongs - 2); i--)
	{
		pindices[index++] = i;
	}

	// Error check for index calculation
	if (index < (numindices-2))
	{
		pindices[index] = numvertices - 2;	// Tie up last triangle in fan
	}
	else
	{
		assert(false);
		std::cout << "Error in sphere definition, index overrun" << std::endl;
	}

	// Generate a buffer for the indices
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numindices * sizeof(GLuint), pindices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete [] pindices;
	delete [] pColours;
	delete [] pVertices;
}


/* Define the vertex positions for a sphere. The array of vertices must have previosuly
been created.
*/
void Sphere::makeUnitSphere(GLfloat *pVertices)
{
	GLfloat DEG_TO_RADIANS = 3.141592f / 180.f;
	GLuint vnum = 0;
	GLfloat x, y, z, lat_radians, lon_radians;

	/* Define north pole */
	pVertices[0] = 0; pVertices[1] = 0; pVertices[2] = 1.f;
	vnum++;

	GLfloat latstep = 180.f / numlats;
	GLfloat longstep = 360.f / numlongs;

	/* Define vertices along latitude lines */
	for (GLfloat lat = 90.f - latstep; lat > -90.f; lat -= latstep)
	{
		lat_radians = lat * DEG_TO_RADIANS;
		for (GLfloat lon = -180.f; lon < 180.f; lon += longstep)
		{
			lon_radians = lon * DEG_TO_RADIANS;

			x = cos(lat_radians) * cos(lon_radians);
			y = cos(lat_radians) * sin(lon_radians);
			z = sin(lat_radians);

			/* Define the vertex */
			pVertices[vnum * 3] = x; pVertices[vnum * 3 + 1] = y; pVertices[vnum * 3 + 2] = z;
			vnum++;
		}
	}
	/* Define south pole */
	pVertices[vnum * 3] = 0; pVertices[vnum * 3 + 1] = 0; pVertices[vnum * 3 + 2] = -1.f;
}

void Sphere::makeUnitRing(GLfloat *pVertices, GLfloat innerRadius, GLfloat outerRadius, GLuint numSegments)
{
    GLfloat DEG_TO_RADIANS = 3.141592f / 180.f;
    GLuint vnum = 0;
    GLfloat x, y, z;

    GLfloat angleStep = 360.f / numSegments;

    for (GLfloat angle = 0.f; angle < 360.f; angle += angleStep)
    {
        GLfloat radians = angle * DEG_TO_RADIANS;

        x = cos(radians);
        y = sin(radians);
        z = 0.f;

        // Scale the coordinates based on inner and outer radii
        x *= outerRadius;
        y *= outerRadius;

        // Define the vertex
        pVertices[vnum * 3] = x;
        pVertices[vnum * 3 + 1] = y;
        pVertices[vnum * 3 + 2] = z;

        vnum++;
    }
}

/* Draws the sphere form the previously defined vertex and index buffers */
void Sphere::drawSphere(int drawmode)
{
	GLuint i;

	/* Draw the vertices as GL_POINTS */
	glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
	glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute_v_coord);

	/* Bind the sphere normals */
	glEnableVertexAttribArray(attribute_v_normal);
	glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
	glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	/* Bind the sphere colours */
	glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
	glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(attribute_v_colours);

	glPointSize(3.f);

	// Enable this line to show model in wireframe
	if (drawmode == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (drawmode == 2)
	{
		glDrawArrays(GL_POINTS, 0, numspherevertices);
	}
	else
	{
		/* Bind the indexed vertex buffer */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		/* Draw the north pole regions as a triangle fan*/
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 2, GL_UNSIGNED_INT, (GLvoid*)(0));

		/* Calculate offsets into the indexed array. Note that we multiply offsets by 4
		because it is a memory offset, the indices are type GLuint which is 4-bytes */
		GLuint lat_offset_jump = (numlongs * 2) + 2;
		GLuint lat_offset_start = numlongs + 2;
		GLuint lat_offset_current = lat_offset_start * 4;

		/* Draw the triangle strips of latitudes */
		for (i = 0; i < numlats - 2; i++)
		{
			glDrawElements(GL_TRIANGLE_STRIP, numlongs * 2 + 2, GL_UNSIGNED_INT, (GLvoid*)lat_offset_current);
			lat_offset_current += (lat_offset_jump * 4);
		}
		/* Draw the south pole as a triangle fan */
		glDrawElements(GL_TRIANGLE_FAN, numlongs + 2, GL_UNSIGNED_INT, (char*)(lat_offset_current));
	}
}

void Sphere::makeRing(GLfloat innerRadius, GLfloat outerRadius, GLuint numSegments)
{
    // Clear existing ring vertices
    ringVertices.clear();

    // Generate ring vertices
    for (GLuint i = 0; i < numSegments; ++i)
    {
        GLfloat theta = (2.0f * glm::pi<float>() * i) / numSegments;
        GLfloat x = cos(theta);
        GLfloat y = sin(theta);

        // Outer vertex
        ringVertices.push_back(outerRadius * x);
        ringVertices.push_back(outerRadius * y);
        ringVertices.push_back(0.0f);

        // Inner vertex
        ringVertices.push_back(innerRadius * x);
        ringVertices.push_back(innerRadius * y);
        ringVertices.push_back(0.0f);
    }
}

void Sphere::drawRing(int drawmode)
{
    /* Draw the vertices as GL_POINTS */
    glBindBuffer(GL_ARRAY_BUFFER, sphereBufferObject);
    glVertexAttribPointer(attribute_v_coord, 3, GL_FLOAT, GL_FALSE, 0, ringVertices.data());
    glEnableVertexAttribArray(attribute_v_coord);

    // Assume the same normals and colors for simplicity
    glEnableVertexAttribArray(attribute_v_normal);
    glBindBuffer(GL_ARRAY_BUFFER, sphereNormals);
    glVertexAttribPointer(attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, sphereColours);
    glVertexAttribPointer(attribute_v_colours, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attribute_v_colours);

    glPointSize(3.f);

    // Enable this line to show model in wireframe
    if (drawmode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (drawmode == 2)
    {
        glDrawArrays(GL_POINTS, 0, ringVertices.size() / 3);
    }
    else
    {
        // Draw the ring as GL_TRIANGLE_STRIP
        glDrawArrays(GL_TRIANGLE_STRIP, 0, ringVertices.size() / 3);

        // Reset polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Disable vertex attributes after drawing
    glDisableVertexAttribArray(attribute_v_coord);
    glDisableVertexAttribArray(attribute_v_normal);
    glDisableVertexAttribArray(attribute_v_colours);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


