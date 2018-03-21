#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <soil/SOIL.h>
#include <math.h>
#include <memory.h>
#include <string>
#include <sstream>

#include "../include/texture.hpp"
#include "../include/heightmap.hpp"
#include "../include/camera.hpp"
#include "../include/sky.hpp"
#include "../include/environment.hpp"
#include "../include/fire.hpp"
#include "tdogl/Program.h"


#define PI 3.14159265
#define RadToAngle 180/PI

// DEBUG
#include <iostream>
using namespace std;

//Global variables
//camera object
Camera* camera;
//environment
Environment* environment;
//fire
BillboardFire* billBoardFire;
SpriteFire* spriteFire;
//mouse position
int mouseX = 0, mouseY = 0;
//middle of the screen
int midX=430, midY=320;
//menu
bool menu = false;
//for flying around
float setHeight = 0.f;
// OGL shader programs
tdogl::Program* phongProgram = nullptr;
tdogl::Program* unifColorProgram = nullptr;
tdogl::Program* fireShader = nullptr;
std::vector<tdogl::Program*> allShaders;
std::vector<tdogl::Program*> shadersWithLight;
//gamma correction
bool gamma = false;

void drawAxis(float nullX, float nullY, float nullZ)
{
	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glTranslatef(nullX, nullY, nullZ);
	glLineWidth(10.0f);
	glBegin(GL_LINES);	
	//x axis RED
	GLfloat diffuse_and_ambient[] = { 1,0,0,1 };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse_and_ambient);
	glColor3f(1.0f, 0.f, 0.f);  glVertex3f(0.f, 0.f, 0.f);	glVertex3f(3.0f, 0.f, 0.f);
	//y axis GREEN
	diffuse_and_ambient[0] = 0;
	diffuse_and_ambient[1] = 1;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse_and_ambient);
	glColor3f(0.0f, 1.0f, 0.f);	glVertex3f(0.f, 0.f, 0.f);	glVertex3f(0.0f, 3.0f, 0.f);
	//z axis BLUE
	diffuse_and_ambient[1] = 0;
	diffuse_and_ambient[2] = 1;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse_and_ambient);
	glColor3f(0.0f, 0.f, 1.f);	glVertex3f(0.f, 0.f, 0.f);	glVertex3f(0.0f, 0.f, 3.f);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glPopMatrix();
}

HeightMapLoader *heightMap;

int g_time = 0;
int fpsCounter = 0;
double fpsSum = 0.0;
double currentFPS = 0.0;
int timeSinceLastUpdate = 0;
/// calculates fps every frame
double calcFps() 
{
	int now = glutGet(GLUT_ELAPSED_TIME);
	int passedTime = (now - g_time);
	double fps = 1 / (passedTime / 1000.0);
	g_time = now;

	fpsSum += fps;
	timeSinceLastUpdate += passedTime;
	fpsCounter++;
	return fps;
}

/// prints fps every 0.5 seconds
void printFps()
{
	//////////// OpenGL stuff ////////////
	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); 
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	//////////// OpenGL stuff ////////////

	glColor3f(1.f, 0.f, 0.f);
	glRasterPos2f(0.6f, -0.9f);

	calcFps();

	if (timeSinceLastUpdate > 500)
	{
		currentFPS = fpsSum / fpsCounter;
		fpsCounter = 0;
		fpsSum = 0.0;
		timeSinceLastUpdate = 0;
	}

    std::stringstream fpsStream;
    fpsStream << "fps: " << currentFPS;
    std::string fpsString = fpsStream.str();

	for (int i = 0; i < strlen(fpsString.c_str()); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, fpsString[i]);
	}

	//////////// OpenGL stuff ////////////
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
	//glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();	
	//////////// OpenGL stuff ////////////
}

GLenum error;
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//reset transformations
	glLoadIdentity();

	camera->updateCamera();

	//float scale = heightMap->getScale();
	float maxHeight = heightMap->getMaxHeight();

	heightMap->drawTerrain();
	environment->update();

	//////////////////// Fire ////////////////////
	billBoardFire->drawFire();
	spriteFire->drawFire();
	//////////////////// Fire ////////////////////

	printFps();

	//check errors	
	error = glGetError();
	while (error != GL_NO_ERROR)
	{
		std::cout << error << std::endl;
		error = glGetError();
	}
	//std::cout << "error check done" << std::endl;

	glFlush();
	glutSwapBuffers();	
}

void reshape(GLsizei width, GLsizei height)
{
	cout << "Window size: " << width << "x" << height << endl;
	glViewport(0, 0, width, height);
	midX = width / 2;
	midY = height / 2;
	camera->setAspectRatio((float) width / (float) height);
}

void mouseHandler(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void motionHandler(int x, int y)
{
	//rotation around the Y axis
	float rotX = (x - mouseX) / 200.0f;
	camera->addRotationInRadian(rotX);

	//changing horizon
	float rotY = (y - mouseY)/200.f;
	camera->changeHorizonInRadian(rotY);

	//update positions, dont let the cursor leave the window
	if (x<=(midX/3) || x>=(midX * 1.5))
	{
		mouseX = midX;
		glutWarpPointer(midX, y);
	}
	else
	{
		mouseX = x;
	}	
	if (y <= (midY / 3) || y >= (midY * 1.5))
	{
		mouseY = midY;
		glutWarpPointer(x, midY);
	}
	else
	{
		mouseY = y;
	}
}

void cleanUp() {	
	//camera->~Camera();
	//environment->~Environment();
	heightMap->~HeightMapLoader();
	//enable key repeat GLOBALLY
	glutSetKeyRepeat(GLUT_KEY_REPEAT_DEFAULT);
}

//speed of camera
float speed = 1.f;
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		camera->setForwardMovement();
		break;
	case 's':
		camera->setBackwardMovement();
		break;
	case 'd':
		camera->setRightMovement();
		break;
	case 'a':
		camera->setLeftMovement();
		break;
	case ' ':
		camera->setUpwardMovement();
		break;
	case 'x':
		camera->setDownwardMovement();
		break;
	case 'g':
		if (gamma)
		{
			glDisable(GL_FRAMEBUFFER_SRGB);
			gamma = false;
		}
		else
		{
			glEnable(GL_FRAMEBUFFER_SRGB);
			gamma = true;
		}
		break;
	case 43:
		// + key
		environment->changeAmbientLight(0.1f);
		break;
	case 45:
		// - key
		environment->changeAmbientLight(-0.1f);
		break;
	case 'm':
		environment->toggleMoonlight();
		break;
	case 27:
		cleanUp();
		exit(0);
		break;
	}
}
void keyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		camera->setForwardMovement();
		break;
	case 's':
		camera->setBackwardMovement();
		break;
	case 'd':
		camera->setRightMovement();
		break;
	case 'a':
		camera->setLeftMovement();
		break;
	case ' ':
		camera->setUpwardMovement();
		break;
	case 'x':
		camera->setDownwardMovement();
		break;
	}
}

void specialFunctionKeys(int key, int x, int y)
{
	if (key == 1)
	{
		if (menu)
		{
			menu = false;
			cout << "exit" << endl;
		}
		else
		{
			menu = true;
			cout << "enter" << endl;
		}
	}
}

std::string resourcePath(std::string fileName)
{
	char executablePath[1024] = { '\0' };
	DWORD charsCopied = GetModuleFileName(NULL, executablePath, 1024);
	if (charsCopied > 0 && charsCopied < 1024)
	{
		std::cout << std::string(executablePath) + "\\..\\.. \\.. \\src\\" + fileName << std::endl;
		return std::string(executablePath) + "\\..\\.. \\.. \\src\\" + fileName;
		//return std::string(executablePath) + "\\..\\" + fileName;
	}
	else
		throw std::runtime_error("GetModuleFileName failed a bit");
}
void loadShaders() {
	std::cout << "Loading shaders ..." << std::endl;
	std::vector<tdogl::Shader> shaders;
	shaders.push_back(tdogl::Shader::shaderFromFile(("phong.vs"), GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(("phong.fs"), GL_FRAGMENT_SHADER));
	phongProgram = new tdogl::Program(shaders);
	allShaders.push_back(phongProgram);
	shadersWithLight.push_back(phongProgram);

	shaders.clear();
	shaders.push_back(tdogl::Shader::shaderFromFile(("unifColor-Phong.vs"), GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(("unifColor-Phong.fs"), GL_FRAGMENT_SHADER));
	unifColorProgram = new tdogl::Program(shaders);
	allShaders.push_back(unifColorProgram);
	shadersWithLight.push_back(unifColorProgram);

	shaders.clear();
	shaders.push_back(tdogl::Shader::shaderFromFile(("fireShader.vs"), GL_VERTEX_SHADER));
	shaders.push_back(tdogl::Shader::shaderFromFile(("fireShader.fs"), GL_FRAGMENT_SHADER));
	fireShader = new tdogl::Program(shaders);
	allShaders.push_back(fireShader);

	std::cout << "Done." << endl;
}
void loadObjects() {
	std::cout << "Load objects... " << std::endl;
	//Loadheightmap
	heightMap = new HeightMapLoader("terrain6_256.png", phongProgram);
	//init cam, sets the current time
	camera = new Camera(heightMap, shadersWithLight, fireShader);
	camera->setAspectRatio(((float)midX * 2) / ((float)midY * 2));
	//set up environment
	environment = new Environment();
	environment->initialize(heightMap, camera, unifColorProgram, shadersWithLight);
	//set up fire
	glm::vec3 firePosition1(125, heightMap->getHeight(125, 125), 125);
	billBoardFire = new BillboardFire(fireShader, camera, firePosition1, true, 2.f);
	glm::vec3 firePosition2(127, heightMap->getHeight(127, 125), 125);
	spriteFire = new SpriteFire(fireShader, camera, firePosition2, 2.f);
}
void initialize()
{
	cout << "Init ..." << endl;
	
	//Alpha functions
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.5f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glClearColor(1.0, 1.0, 1.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	//reset the matrix
	glLoadIdentity();
	//hide cursor
	glutSetCursor(GLUT_CURSOR_NONE);
	//disable key repeat GLOBALLY
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	//gamma correction
	//glEnable(GL_FRAMEBUFFER_SRGB);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	camera->startTimer();
}

/**
* Main function
*/
int main(int argc, char* argv[])
{
	
	cout << "Init GLUT ..." << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	cout << "Init window ..." << endl;
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-(midX*2))/2, (glutGet(GLUT_SCREEN_HEIGHT) - (midY * 2)) / 2);
	glutInitWindowSize(midX*2, midY*2);

	cout << "Create window ..." << endl;
	int window = glutCreateWindow("GLUT Window");
	glutSetWindow(window);

	// initialise GLEW
	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("glewInit failed");

	// GLEW throws some errors, so discard all the errors so far
	while (glGetError() != GL_NO_ERROR) {}

	loadShaders();
	loadObjects();

	// set mouseX and mouseY
	mouseHandler(0, 0, midX, midY);
	glutWarpPointer(midX, midY);
	initialize();

	std::cout << "Rendering... " << std::endl;
	
	//callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseHandler);
	glutPassiveMotionFunc(motionHandler);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialFunctionKeys);
	glutIdleFunc(display);

	//loop
	glutMainLoop();

	return 0;
}
