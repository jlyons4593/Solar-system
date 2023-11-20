#include "glad.h"
// #include "ring.h"
#include <thread>
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>
#include <stack>
#include <chrono>
/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

// Include headers for our objects
#include "sphere.h"

#ifndef UNIVERSE_CPP
#define UNIVERSE_CPP
enum ShaderMode{
  OREN_NAYAR=0,
  FRAG=1,
  POS=2,
};
extern GLuint modelID[3], normalmatrixID[3], colourmodeID[3], attenuationmodeID[3], viewID[3], projectionID[3], emitmodeID[3];
extern ShaderMode current_shader;
extern GLuint drawmode, colourmode, attenuationmode, emitmode;
extern GLfloat mercury_angle, venus_angle, mars_angle, earth_angle, jupiter_angle, saturn_angle, uranus_angle, neptune_angle, moon_angle, ganymede_angle, titan_angle;
extern GLfloat universe_speed, moon_speed, titan_speed, ganymede_speed;
extern glm::vec3 earth_colour, mars_colour, venus_colour, mercury_colour, jupiter_colour, uranus_colour, saturn_colour, neptune_colour, moon_colour, titan_colour, ganymede_colour, neptune_ring_colour;
namespace Universe{

 void init_planet(std::stack<glm::mat4> model, glm::mat4 view, float scale_factor, glm::vec3 translation_vector, Sphere sphere, GLfloat angle_y){
   glm::mat3 normalmatrix;

  model.top() = rotate(model.top(), -glm::radians(angle_y), glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
  
	model.push(model.top());
	{
		model.top() = translate(model.top(), translation_vector);
		model.top() = scale(model.top(), glm::vec3(scale_factor / 3.f, scale_factor/ 3.f, scale_factor / 3.f));//scale equally in all axis																						
		// Recalculate the normal matrix and send the model and normal matrices to the vertex shader																					
    // Recalculate the normal matrix and send to the vertex shader																								
		glUniformMatrix4fv(modelID[current_shader], 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(glm::mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_shader], 1, GL_FALSE, &normalmatrix[0][0]);

		sphere.drawSphere(drawmode); // Draw our sphere
	}

	model.pop();

}
void init_star(std::stack<glm::mat4> model, glm::mat4 view, float scale_factor, glm::mat4 projection, GLuint starID, Sphere star)
{
    glm::mat3 normalmatrix;
  
  	// Define the light position and transform by the view matrix
  
    glm::vec4 starpos = view *  glm::vec4(0, 0, 0, 1.0);
  
  	glUniform1ui(colourmodeID[current_shader], colourmode);
  	glUniform1ui(attenuationmodeID[current_shader], attenuationmode);
  	glUniformMatrix4fv(viewID[current_shader], 1, GL_FALSE, &view[0][0]);
  	glUniformMatrix4fv(projectionID[current_shader], 1, GL_FALSE, &projection[0][0]);
  	glUniform4fv(starID, 1, value_ptr(starpos));
  
  	/* Draw a small sphere in the lightsource position to visually represent the light source */
  	model.push(model.top());
  	{
  		model.top() = scale(model.top(), glm::vec3(0.2f, 0.2f, 0.2f)); 
     // make a small sphere
  	 // Recalculate the normal matrix and send the model and normal matrices to the vertex shader							 // Recalculate the normal matrix and send to the vertex shader																					
  		glUniformMatrix4fv(modelID[current_shader], 1, GL_FALSE, &(model.top()[0][0]));
  		normalmatrix = transpose(glm::inverse(glm::mat3(view * model.top())));
  		glUniformMatrix3fv(normalmatrixID[current_shader], 1, GL_FALSE, &normalmatrix[0][0]);
  
  		/* Draw our lightposition sphere  with emit mode on*/
  		emitmode = 1;
  		glUniform1ui(emitmodeID[current_shader], emitmode);
  		star.drawSphere(drawmode);
  		emitmode = 0;
  		glUniform1ui(emitmodeID[current_shader], emitmode);
  	}
  	model.pop();
    
  }


void init_moon(std::stack<glm::mat4> model, glm::mat4 view, float scale_factor, glm::vec3 translation_vector, Sphere sphere, GLfloat angle_y, GLfloat moon_angle, GLfloat distance){
  glm::mat3 normalmatrix;

  model.top() = rotate(model.top(), -glm::radians(angle_y), glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	model.top() = translate(model.top(), translation_vector);

	model.push(model.top());
	{

    model.top() = rotate(model.top(), -glm::radians(-1 * moon_angle), glm::vec3(0, 1, 0));
    model.top() = translate(model.top(),glm::vec3(-distance, 0, 0));
		model.top() = scale(model.top(), glm::vec3(scale_factor / 3.f, scale_factor/ 3.f, scale_factor / 3.f));
//    scale equally in all axis																						
// Recalculate the normal matrix and send the model and normal matrices to the vertex shader																					
// Recalculate the normal matrix and send to the vertex shader																								
		glUniformMatrix4fv(modelID[current_shader], 1, GL_FALSE, &(model.top()[0][0]));
		normalmatrix = transpose(inverse(glm::mat3(view * model.top())));
		glUniformMatrix3fv(normalmatrixID[current_shader], 1, GL_FALSE, &normalmatrix[0][0]);

		sphere.drawSphere(drawmode); // Draw our sphere
	}
	model.pop();

}
// Handles timer for rotation of planets
static void increment_planet_spins()
{
  while(true){
    mercury_angle += 1.6f * universe_speed;
    venus_angle += 0.64f * universe_speed;
    earth_angle += 0.4f * universe_speed;
    mars_angle += 1.0f * universe_speed;
    jupiter_angle += 0.18f * universe_speed;
    saturn_angle += 0.066f * universe_speed;
    neptune_angle += 0.012f * universe_speed;
    uranus_angle += 0.02f * universe_speed;
    moon_angle += moon_speed * universe_speed;
    titan_angle += titan_speed * universe_speed;
    ganymede_angle += ganymede_speed * universe_speed;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}
// Setting all of the planet colours
void set_planet_colours(){
  earth_colour = glm::vec3(0.2f, 0.5f, 0.8f);
  mars_colour = glm::vec3(0.9f, 0.3f, 0.2f);
  mercury_colour = glm::vec3(0.7f, 0.7f, 0.7f);
  jupiter_colour = glm::vec3(1.0f, 0.8f, 0.2f);
  saturn_colour = glm::vec3(1.0f, 0.8f, 0.2f);
  uranus_colour = glm::vec3(0.3f, 0.7f, 0.6f);
  neptune_colour = glm::vec3(0.1f, 0.1f, 0.7f);
  venus_colour = glm::vec3(1.0f, 0.6f, 0.2f);
  moon_colour = glm::vec3(0.5f, 0.5f, 0.5f);
  titan_colour = glm::vec3(0.16f, 0.2f, 0.35f);
  ganymede_colour = glm::vec3(0.45f, 0.55f, 0.65f);
  neptune_ring_colour = glm::vec3(0.5f, 0.7f, 1.0f);
}

// void init_ring(std::stack<glm::mat4> model, glm::mat4 view, float scale_factor, glm::vec3 translation_vector, Ring ring, GLfloat angle_y){
//   glm::mat3 normalmatrix;
//
// 	model.top() = scale(model.top(), glm::vec3(model_scale, model_scale, model_scale));//scale equally in all axis
// 	model.top() = rotate(model.top(), -glm::radians(angle_x), glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
//   model.top() = rotate(model.top(), -glm::radians(angle_y), glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
// 	model.top() = rotate(model.top(), -glm::radians(angle_z), glm::vec3(0, 0, 1)); //rotating in clockwise direction around z-axis
//   
// 	model.push(model.top());
// 	{
// 		model.top() = translate(model.top(), translation_vector);
// 		model.top() = scale(model.top(), glm::vec3(scale_factor / 3.f, scale_factor/ 3.f, scale_factor / 3.f));//scale equally in all axis																						
// 		// Recalculate the normal matrix and send the model and normal matrices to the vertex shader																					
//     // Recalculate the normal matrix and send to the vertex shader																								
// 		glUniformMatrix4fv(modelID, 1, GL_FALSE, &(model.top()[0][0]));
// 		normalmatrix = transpose(inverse(glm::mat3(view * model.top())));
// 		glUniformMatrix3fv(normalmatrixID, 1, GL_FALSE, &normalmatrix[0][0]);
//
// 		ring.drawRing(drawmode);// Draw our sphere
// 	}
//
// 	model.pop();
//
// }
}

#endif
