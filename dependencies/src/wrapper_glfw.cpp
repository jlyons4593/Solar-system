/**
  wrapper_glfw.cpp
  Modified from the OpenGL GLFW example to provide a wrapper GLFW class
  and to include shader loader functions to include shaders as text files
  Iain Martin August 2022
  */

#include "wrapper_glfw.h"
#include "GLFW/glfw3.h"

/* Inlcude some standard headers */

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>

using namespace std;

/* Constructor for wrapper objects */
GLWrapper::GLWrapper(int width, int height, const char *title) {

	this->width = width;
	this->height = height;
	this->title = title;
	this->fps = 60;
	this->running = true;
	this->renderer = NULL;

	/* Initialise GLFW and exit if it fails */
	if (!glfwInit()) 
	{
		cout << "Failed to initialize GLFW." << endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef DEBUG
	glfwOpenWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	window = glfwCreateWindow(width, height, title, 0, 0);
	if (!window){
		cout << "Could not open GLFW window." << endl;
		cout << glfwGetError(NULL) << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/* Obtain an OpenGL context and assign to the just opened GLFW window */
	glfwMakeContextCurrent(window);

	/* Initialise GLLoad library. You must have obtained a current OpenGL */
	 // glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD - exiting" << std::endl;
		glfwTerminate();
		return;
	}

	/* Can set the Window title at a later time if you wish*/
	glfwSetWindowTitle(window, title);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
	glfwSetInputMode(window, GLFW_STICKY_KEYS, true);

	glEnable(GL_MULTISAMPLE);
}


/* Terminate GLFW on destruction of the wrapper objects */
GLWrapper::~GLWrapper()
{
	glfwTerminate();
}

/* Returns the GLFW window handle, required to call GLFW functions outside this class */
GLFWwindow* GLWrapper::getWindow()
{
	return window;
}


/*
 * Print OpenGL Version details
 */
void GLWrapper::DisplayVersion()
{
	/* One way to get OpenGL version*/
	int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MAJOR_VERSION, &minor);
	cout << "OpenGL Version = " << major << "." << minor << endl;
	
	/* A more detailed way to the version strings*/
	cout << "Vender: " << glGetString(GL_VENDOR) << endl;
    cout << "Version:" << glGetString(GL_VERSION) << endl;
	cout << "Renderer:" << glGetString(GL_RENDERER) << endl;
}


/*
GLFW_Main function normally starts the windows system, calls any init routines
and then starts the event loop which runs until the program ends
*/
int GLWrapper::eventLoop()
{
  // std::thread spawn thread to call timer for incrementing planets direction
	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Call function to draw your graphics
		renderer();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


/* Register an error callback function */
void GLWrapper::setErrorCallback(void(*func)(int error, const char* description))
{
	glfwSetErrorCallback(func);
}

/* Register a display function that renders in the window */
void GLWrapper::setRenderer(void(*func)()) {
	this->renderer = func;
}

/* Register a callback that runs after the window gets resized */
void GLWrapper::setReshapeCallback(void(*func)(GLFWwindow* window, int w, int h)) {
	glfwSetFramebufferSizeCallback(window, func);
}


/* Register a callback to respond to keyboard events */
void GLWrapper::setKeyCallback(void(*func)(GLFWwindow* window, int key, int scancode, int action, int mods))
{
	glfwSetKeyCallback(window, func);
}
void GLWrapper::setMouseCallback(void(*func)(GLFWwindow* window, double xpos, double ypos)){
  glfwSetCursorPosCallback(window, func);
}

/* Build shaders from strings containing shader source code */
GLuint GLWrapper::BuildShader(GLenum eShaderType, const string &shaderText)
{
  GLuint shader = glCreateShader(eShaderType);
  const char *strFileData = shaderText.c_str();
  glShaderSource(shader, 1, &strFileData, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		// Output the compile errors
		
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType)
		{
			case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
			case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
			case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		cerr << "Compile error in " << strShaderType << "\n\t" << strInfoLog << endl;
		delete[] strInfoLog;

		throw runtime_error("Shader compile exception");
	}

	return shader;
}

/* Read a text file into a string*/
string GLWrapper::readFile(const char *filePath)
{
	string content;
	ifstream fileStream(filePath, ios::in);

	if (!fileStream.is_open()) {
		cerr << "Could not read file " << filePath << ". File does not exist." << endl;
		return "";
	}

	string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

/* Load vertex and fragment shader and return the compiled program */
GLuint GLWrapper::LoadShader(const char *vertex_path, const char *fragment_path)
{
	GLuint vertShader, fragShader;

	// Read shaders
	string vertShaderStr = readFile(vertex_path);
	string fragShaderStr = readFile(fragment_path);

	GLint result = GL_FALSE;
	int logLength;

	vertShader = BuildShader(GL_VERTEX_SHADER, vertShaderStr);
	fragShader = BuildShader(GL_FRAGMENT_SHADER, fragShaderStr);

	cout << "Linking program" << endl;
	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	cout << &programError[0] << endl;

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

/* Load vertex and fragment shader and return the compiled program */
GLuint GLWrapper::BuildShaderProgram(string vertShaderStr, string fragShaderStr)
{
	GLuint vertShader, fragShader;
	GLint result = GL_FALSE;

	try
	{
		vertShader = BuildShader(GL_VERTEX_SHADER, vertShaderStr);
		fragShader = BuildShader(GL_FRAGMENT_SHADER, fragShaderStr);
	}
	catch (exception &e)
	{
		cout << "Exception: " << e.what() << endl;
		throw runtime_error("BuildShaderProgram() Build shader failure. Abandoning");
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		cerr << "Linker error: " << strInfoLog << endl;

		delete[] strInfoLog;
		throw runtime_error("Shader could not be linked.");
	}
	
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}
