// Joseph Lyons 12/11/2023
#include <thread>
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#else
#pragma comment(lib, "glfw3.lib")
#endif
#pragma comment(lib, "opengl32.lib")

/* Include the header to the GLFW wrapper class which
   also includes the OpenGL extension initialisation*/
#include <wrapper_glfw.h>
#include <iostream>
#include <stack>
#include <chrono>
/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

// Include headers for our objects
#include "sphere.h"
#include "ring.h"
#include "universe.cpp"

extern GLfloat yaw, pitch;
extern GLfloat universe_speed, moon_speed, ganymede_speed, titan_speed;
extern bool moons, fromTop;
extern GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/
extern GLuint emitmode, colourmode, attenuationmode;
extern GLuint drawmode;
extern ShaderMode current_shader;

glm::vec3 camera_position = glm::vec3(0, 0, 6);
glm::vec3 camera_target = glm::vec3(0,0,0);
glm::vec3 camera_direction = glm::normalize(camera_position - camera_target);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
glm::vec3 camera_right = glm::normalize(glm::cross(up, camera_direction));
glm::vec3 camera_front = glm::vec3(0,0,-1);
glm::vec3 camera_up = glm::vec3(0,1,0);
glm::vec3 direction;

namespace InputHandler{

bool firstMouse = true;
GLfloat lastX = 400;
GLfloat lastY = 300;

const float CAMERA_MOVEMENT_SPEED = 0.1f;
const float CAMERA_XROTATION_SPEED = 0.5f;
const float CAMERA_YROTATION_SPEED = 0.05f; 

// function for updating the cameras vectors based on the yaw and pitch to point the camera in the correct direction
void updateCameraVectors()
{
    // Convert Euler angles to radians
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    // Calculate the new front vector
    glm::vec3 front;
    front.x = cos(yawRad) * cos(pitchRad);
    front.y = sin(pitchRad);
    front.z = sin(yawRad) * cos(pitchRad);

    // Update the camera's front vector
    camera_front = glm::normalize(front);
}
// Callback function for whenever a key is pressed to allow control of each part of the program
static void keyCallback(GLFWwindow* window, int key, int s, int action, int mods)
{
	/* Enable this call if you want to disable key responses to a held down key*/

// Camera movement keys
if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

// Check if the space key is released (action != GLFW_PRESS)
if (key == GLFW_KEY_SPACE && action != GLFW_PRESS) {
    // Check the current orientation (fromTop) and set the camera orientation accordingly
    if (!fromTop) {
        // Set the camera to a top-down view
        yaw = 0.0f;
        pitch = -90.0f;
        camera_position = glm::vec3(0.0f, 7.0f, 0.0f);
    } else {
        // Set the camera to a side view
        yaw = -90.0f;
        pitch = 0.0f;
        camera_position = glm::vec3(0.0f, 0.0f, 6.0f);
    }

    // Update the camera vectors based on the new orientation
    updateCameraVectors();

    // Toggle the fromTop flag to switch between top-down and side views
    fromTop = !fromTop;
}

// Move the camera based on key presses
if (key == 'W')
    camera_position += CAMERA_MOVEMENT_SPEED * camera_up; // Move camera up

if (key == 'S')
    camera_position -= CAMERA_MOVEMENT_SPEED * camera_up; // Move camera down

if (key == 'A')
    camera_position -= glm::normalize(glm::cross(camera_front, camera_up)) * CAMERA_MOVEMENT_SPEED; // Move camera left

if (key == 'D')
    camera_position += glm::normalize(glm::cross(camera_front, camera_up)) * CAMERA_MOVEMENT_SPEED; // Move camera right

// Move the camera forward and backward
if (key == GLFW_KEY_UP) 
    camera_position += CAMERA_MOVEMENT_SPEED * camera_front; // Move camera forward

if (key == GLFW_KEY_DOWN)
    camera_position -= CAMERA_MOVEMENT_SPEED * camera_front; // Move camera backward

// Universe Controls

if (key == '1') universe_speed += 0.05;
if (key == '2') universe_speed -= 0.05;

if (key == '3') moon_speed += 0.05;
if (key == '4') moon_speed -= 0.05;

if (key == '5') ganymede_speed += 0.05;
if (key == '6') ganymede_speed -= 0.05;

if (key == '7') titan_speed += 0.05;
if (key == '8') titan_speed -= 0.05;


if (key == '9' && action != GLFW_PRESS) moons = !moons;

	if (key == 'M' && action != GLFW_PRESS)
	{
		colourmode = !colourmode;
    std::cout << "colourmode=" << colourmode << std::endl;
	}

	/* Turn attenuation on and off */
	if (key == '.' && action != GLFW_PRESS)
	{
		attenuationmode = !attenuationmode;
    std::cout<<"Changing attenuation mode"<< std::endl;
	}

	/* Cycle between drawing vertices, mesh and filled polygons */
	if (key == ',' && action != GLFW_PRESS)
	{
		drawmode ++;
		if (drawmode > 2) drawmode = 0;
    std::cout<<"Changing draw mode"<< std::endl;
	}
  if (key == '/' && action != GLFW_PRESS ){
    current_shader= (ShaderMode)((current_shader+1)%3);
    std::cout<<"Changing shader mode"<< std::endl;
  }
}
// mouse callback function for changing the scene based on the placement of the mouse works out new position and view angle
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;
  
    // relative sensitivity of movement
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // Clamp the pitch to prevent the camera from flipping
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    // Calculate the new direction vector based on the updated yaw and pitch
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Normalize the direction vector to ensure it has unit length
    camera_front = glm::normalize(direction);
} 
/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	aspect_ratio = ((float)w / 640.f*4.f) / ((float)h / 480.f*3.f);
}
}
