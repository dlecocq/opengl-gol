#ifndef GRAPHER_CPP
#define GRAPHER_CPP

#include <iostream>
#include <cmath>

#include "grapher.h"

using namespace glot;
using namespace std;

int grapher::initialize(int argc, char ** argv, short int options, short int k_options) {
	
	// Set up the options for display and keyboard
	display_options = options;
	keyboard_options = k_options;
	
	// Initialize GLUT
	glutInit(&argc, argv);
	
	// Set the color mode (double with alpha)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	
	// Width and height of plot
	width  = glutGet(GLUT_SCREEN_WIDTH);
	height = glutGet(GLUT_SCREEN_HEIGHT) - 100;
	// Set the window size and position
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	
	// Title the window
	glutCreateWindow("Game of Life");
	
	// Initialize OpenGL
	init_open_gl();
	
	// Register callback functions with GLUT
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	//glutIdleFunc(idle);
	
	glewInit();
	
	if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && 
				GLEW_EXT_framebuffer_object && GLEW_ARB_texture_float)) {
		printf("Not all necessary features are supported :( \n");
		exit(1);
	}
	
	//init_framebuffer();
	
	wall.start();
	
	return 0;
}

// Initialize OpenGL
int grapher::init_open_gl() {
	
	// Enable smoothness and blending
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	// Other smoothness and blending options
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	
	// Set the line width and point size
	glLineWidth(1.5);
	glPointSize(7);
	
	// Default color is white
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	return 0;
}

void grapher::init_framebuffer() {
	
	pde->set_param("width" , width);
	pde->set_param("height", height);
	
	float * data = (float *) malloc(width * height * sizeof(float) * 4);
	// Learned a new function - memset
	memset(data, 0.0, width * height * 4);
	srand(time(NULL));
	for (int i = 0; i < width * height; ++i) {
		// This should be random 0's and 1's
		data[i * 4] = floor(rand()/float(RAND_MAX) + 0.5);
	}
	
  // initialize color texture
  glGenTextures(1, &render);
  glBindTexture(GL_TEXTURE_2D, render);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, data);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, fb);
	
	glGenFramebuffersEXT(1, &fb);
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, fb);

	initialized = true;
}

int grapher::check_framebuffer() {
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_DRAW_FRAMEBUFFER);
	
	switch(status) {
		case GL_FRAMEBUFFER_COMPLETE:
			//cout << "Framebuffer attached successfully." << endl;
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			cout << "Framebuffer unsupported" << endl;
			exit(1);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			cout << "Incomplete Attachment" << endl;
			exit(1);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			cout << "Incomplete draw buffer" << endl;
			exit(1);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			cout << "Incomplete read buffer" << endl;
			exit(1);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			cout << "Incomplete multisample" << endl;
			exit(1);
			break;
		case GL_FRAMEBUFFER_UNDEFINED:
			cout << "Framebuffer undefined" << endl;
			exit(1);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			cout << "Incomplete missing attachment" << endl;
			exit(1);
			break;
		default:
			cout << "Uncertain failure.  Exiting" << endl;
			exit(1);
	}
	
	// Ideally, I'd like this to be more informative, but I'm in a rush
	return 0;
}

void grapher::calculate() {
	GLuint tmp;
	
	// Bind the framebuffer for rendering
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, fb);
	// And the texture you'll be rendering to
	glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render, 0);
	// Then bind the texture you'll be reading from
	glBindTexture(GL_TEXTURE_2D, tex);
	
	// Check to make sure that the framebuffer ran correctly
	check_framebuffer();
	
	glUseProgram(pde->pc);
	pde->dl_gen(-1, 1, -1, 1);
	
	// Swap the two textures for next iteration
	tmp = render;
	render = tex;
	tex = tmp;
}

// Our callback function registered with OpenGL
void grapher::display() {
	
	if (!initialized) {
		cout << "Initializing the framebuffer in display" << endl;
		init_framebuffer();
	}
	
	calculate();
	
	// Bind the display framebuffer to render the results
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, render);

	// I don't understand why I have to use this pr, and can't use 0 :-/
	glUseProgram(pde->pr);
	//glUseProgram(0);
	pde->dl_gen(-1, 1, -1, 1);
	
	glUseProgram(0);
		
	// Finish up
	glutSwapBuffers();

}

// Our reshaping function, registered with OpenGL
void grapher::reshape(int w, int h) {
	// Set the viewport
	glViewport(0, 0, w, h);
	
	// Re-calculate the draw lists if we've expanded the view
	
	// Redisplay the plot
	glLoadIdentity();
	glutPostRedisplay();
	//*/
}

// Our keyboard function registered with OpenGL
GLvoid grapher::keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'q':
			if (keyboard_options & QUIT_KEYS_ON) {
				cout << "Quitting." << endl;
				exit(0);
				break;
			}
	}
	
	/** If the user has defined their own keyboard event
	  * handler, execute it here, after we've completed
	  * our own.  They may turn on and off the default
	  * functionality by changing keyboard_options
	  */
	if (user_keyboard_function) {
		user_keyboard_function(key, x, y);
	}
}

void grapher::idle() {
	if (user_idle_function != NULL) {
		user_idle_function();
	}
}

void grapher::set_keyboard_function(keyboard_function k) {
	user_keyboard_function = k;
}

void grapher::set_idle_function(idle_function i) {
	glutIdleFunc(i);
}

double grapher::y_coord_transform(double y) {
	if (Y_LOG & display_options) {
		return log10(abs(y));
	} else {
		return y;
	}
}

double grapher::x_coord_transform(double x) {
	if (X_LOG & display_options) {
		return pow(10, x);
	} else {
		return x;
	}
}

void grapher::redraw() {
	if (framecount == 0) {
		timer.start();
	}
	glFinish();
	glutPostRedisplay();
	++framecount;
	if (framecount == 500) {
		framecount = 0;
		cout << 500 / timer.stop() << " fps" << endl;
	}
}

void grapher::run() {
	glutMainLoop();
}

void grapher::get_pixels(char* values) {
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, values);
}

// Static member variable definition
// It's terribly ugly, I know
short int grapher::display_options;
short int grapher::keyboard_options;
GLint grapher::axes_dl;
GLint grapher::grid_dl;
int grapher::startx;
int grapher::starty;
map<primitive*, GLint> grapher::primitives;
grapher::keyboard_function grapher::user_keyboard_function;
grapher::click_function grapher::user_click_function;
grapher::idle_function grapher::user_idle_function;
stopwatch grapher::timer;
stopwatch grapher::wall;

GLuint grapher::fb;
GLuint grapher::tex;
GLuint grapher::render;
GLuint grapher::depth_rb;
int grapher::width;
int grapher::height;

primitive* grapher::pde = NULL;

bool grapher::initialized = false;

int grapher::framecount;

#endif
