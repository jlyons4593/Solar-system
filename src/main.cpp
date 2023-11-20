
// Joseph Lyons 12/11/2023
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
#include <thread>

// Include headers for our objects
#include "sphere.h"
#include "ring.h"

#include "universe.cpp"
#include "inputHandler.cpp"
#include "fileReader.cpp"

/* Define buffer object indices */
GLuint elementbuffer;

std::array<GLuint,3> program;		/* Identifier for the shader prgoram */
GLuint vao;			/* Vertex array (Containor) object. This is the index of the VAO that will be the container for our buffer objects */

// Colour mode, emitmode and attenuation mode variables to allow switching between modes
GLuint emitmode, colourmode, attenuationmode;

/* Position and view globals */
GLfloat earth_angle, mars_angle, venus_angle, mercury_angle, jupiter_angle, uranus_angle, saturn_angle, neptune_angle, moon_angle, ganymede_angle, titan_angle;
glm::vec3 earth_colour, mars_colour, venus_colour, mercury_colour, jupiter_colour, uranus_colour, saturn_colour, neptune_colour, moon_colour, titan_colour, ganymede_colour, neptune_ring_colour;
GLuint drawmode;			// Defines drawing mode of sphere as points, lines or filled polygons
GLuint numlats, numlongs;	//Define the resolution of the sphere object
GLfloat speed, earth_speed;				// movement increment
glm::mat4 view;

ShaderMode current_shader;
/* Uniforms*/
GLuint modelID[3], viewID[3], projectionID[3], sunposID[3], normalmatrixID[3], colourmodeID[3], emitmodeID[3], attenuationmodeID[3];
GLfloat aspect_ratio;		/* Aspect ratio of the window defined in the reshape callback*/
GLuint numspherevertices;
bool moons, fromTop;
/* Global instances of our objects */
Sphere sun, earth_sphere, earth_moon, mars_sphere, venus_sphere, mercury_sphere, jupiter_sphere, ganymede_moon, uranus_sphere, saturn_sphere, titan_moon, neptune_sphere;
GLfloat pitch, yaw;
float universe_speed, moon_speed, titan_speed, ganymede_speed;
glm::vec3 modelRotation = glm::vec3(0.f, 0.f, 0.f);

/*
This function is called before entering the main rendering loop.
Use it for all your initialisation stuff
*/
void init(GLWrapper *glw)
{
	/* Set the object transformation directheir values tion.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));*/
  current_shader = ShaderMode::OREN_NAYAR;
  moons = true, fromTop = false;
  
  yaw = -90.f;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	speed = 0.05f;
  universe_speed = 1.f, moon_speed = 2.0f, titan_speed = 1.5f, ganymede_speed = 1.f;
	aspect_ratio = 1.3333f;
	colourmode = 1, emitmode = 0, attenuationmode = 1; // Attenuation is on by default
	numlats = 40,	numlongs = 40;		// Number 0,0,f longitudes in our sphere
  // sets planet colours
  Universe::set_planet_colours();
	// Generate index (name) for one vertex array object
	glGenVertexArrays(1, &vao);

	// Create the vertex array object and make it current
	glBindVertexArray(vao);

	/* Load and build the vertex and fragment shaders */
	try
	{
    program[0] = glw->LoadShader("shaders/fraglight.vert","shaders/fraglight_oren_nayar.frag");
		program[1] = glw->LoadShader("shaders/fraglight.vert", "shaders/fraglight.frag");
    program[2] = glw->LoadShader("shaders/poslight.vert", "shaders/poslight.frag");
	}
	catch (std::exception &e)
	{
    std::cout << "Caught exception: " << e.what() << std::endl;
    std::cin.ignore();
		exit(0);
	}

	/* Define uniforms to send to vertex shaders */
  for(int i=0; i<3; i++){

	  modelID[i] = glGetUniformLocation(program[i], "model");
	  colourmodeID[i] = glGetUniformLocation(program[i], "colourmode");
	  emitmodeID[i] = glGetUniformLocation(program[i], "emitmode");
	  attenuationmodeID[i] = glGetUniformLocation(program[i], "attenuationmode");
	  viewID[i] = glGetUniformLocation(program[i], "view");
	  projectionID[i] = glGetUniformLocation(program[i], "projection");
    sunposID[i] = glGetUniformLocation(program[i], "sunpos");
    normalmatrixID[i] = glGetUniformLocation(program[i], "normalmatrix");
    }

    /* create our planet spheres */
    sun.makeSphere(numlats, numlongs);
    earth_sphere.makeSphere(numlats, numlongs, earth_colour);
    mars_sphere.makeSphere(numlats, numlongs, mars_colour);
    mercury_sphere.makeSphere(numlats, numlongs, mercury_colour);
    venus_sphere.makeSphere(numlats, numlongs, venus_colour);
    jupiter_sphere.makeSphere(numlats, numlongs, jupiter_colour);
    saturn_sphere.makeSphere(numlats, numlongs, saturn_colour);
    neptune_sphere.makeSphere(numlats, numlongs, neptune_colour);
    uranus_sphere.makeSphere(numlats, numlongs, uranus_colour);
    earth_moon.makeSphere(numlats, numlongs, moon_colour);
    ganymede_moon.makeSphere(numlats, numlongs, ganymede_colour);
    titan_moon.makeSphere(numlats, numlongs, titan_colour);

  }
    
    
  /* Called to update the display. Note that this function is called in the event loop in the wrapper
     class because we registered display as a callback function */
void display()
{
	/* Define the background colour */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* Clear the colour and frame buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Enable depth test  */
	glEnable(GL_DEPTH_TEST);

	/* Make the compiled shader program current */
	glUseProgram(program[current_shader]);

	// Define our model transformation in a stack and 
	// push the identity matrix onto the stack
  std::stack<glm::mat4> model;
	model.push(glm::mat4(1.0f));


	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  glm::mat4 projection = glm::perspective(glm::radians(30.0f), aspect_ratio, 0.1f, 100.0f);

    model.top() = rotate(model.top(), glm::radians(modelRotation.x), glm::vec3(1.f, 0.f, 0.f));
    model.top() = rotate(model.top(), glm::radians(modelRotation.y), glm::vec3(0.f, 1.f, 0.f));
    model.top() = rotate(model.top(), glm::radians(modelRotation.z), glm::vec3(0.f, 0.f, 1.f));

	// Camera matrix
  view = lookAt(
    camera_position,
    camera_position + camera_front,
    camera_up
		);

  // Drawing the sun
  Universe::init_star(model, view, 0.4f, projection, sunposID[current_shader] , sun);

  // old init_ring function call
  // Universe::init_ring(model, view, 0.25f, glm::vec3(-x - 0.6f, 0, 0), ring, neptune_angle);

	// Drawing the planets 
  Universe::init_planet(model, view, 0.05f, glm::vec3(-0.25f, 0, 0), mercury_sphere, mercury_angle);
  Universe::init_planet(model, view, 0.16f, glm::vec3(-0.35f, 0, 0), venus_sphere, venus_angle);
  Universe::init_planet(model, view, 0.2f, glm::vec3(-0.6f, 0, 0), earth_sphere, earth_angle);
  Universe::init_planet(model, view, 0.08f, glm::vec3(-0.75f, 0, 0), mars_sphere, mars_angle);
  Universe::init_planet(model, view, 0.35f, glm::vec3(-1.05f, 0, 0), jupiter_sphere, jupiter_angle);
  Universe::init_planet(model, view, 0.3f, glm::vec3(-1.45f, 0, 0), saturn_sphere, saturn_angle);
  Universe::init_planet(model, view, 0.22f, glm::vec3(-1.75f, 0, 0), uranus_sphere, uranus_angle);
  Universe::init_planet(model, view, 0.25f, glm::vec3(-2.0f, 0, 0), neptune_sphere, neptune_angle);
  
  // Drawing moons if enabled
if (moons){
  Universe::init_moon(model, view, 0.04, glm::vec3(-0.6f, 0, 0), earth_moon, earth_angle, moon_angle, 0.1f);
  Universe::init_moon(model, view, 0.07, glm::vec3(-1.05f, 0, 0), ganymede_moon, jupiter_angle, ganymede_angle,0.2f);
  Universe::init_moon(model, view, 0.09, glm::vec3(-1.45f, 0, 0), titan_moon, saturn_angle, titan_angle, 0.15f);
}

	glDisableVertexAttribArray(0);
	glUseProgram(0);

}  
/* Entry point of program */
int main(int argc, char* argv[])
{
  std::thread planet_timer(Universe::increment_planet_spins);
  GLWrapper *glw = new GLWrapper(1024, 768, "Position light example");;

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
    std::cout << "Failed to initialize GLAD. Exiting." << std::endl;
		return -1;
	}

	glw->setRenderer(display);
	glw->setKeyCallback(InputHandler::keyCallback);
	glw->setReshapeCallback(InputHandler::reshape);
  glw->setMouseCallback(InputHandler::mouse_callback);
	/* Output the OpenGL vendor and version */
	glw->DisplayVersion();
  
  std::string filename = "../src/keyCommands.txt"; 

  // Call the function to read and output the file content
  FileReader::readAndOutputFile(filename);
	init(glw);

	glw->eventLoop();
  planet_timer.join();
	delete(glw);
	return 0;
}

