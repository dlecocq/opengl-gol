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
	
	// Initialize bounds
	scr.minx = scr.miny = -1;
	scr.maxx = scr.maxy = 1;
	
	// Initialize GLUT
	glutInit(&argc, argv);
	
	// Set the color mode (double with alpha)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	
	// Width and height of plot
	scr.width  = width  = 700;
	scr.height = height = 700;
	// Set the window size and position
	glutInitWindowSize(scr.width, scr.height);
	glutInitWindowPosition(0, 0);
	
	// Title the window
	glutCreateWindow("Glot");
	
	// Initialize OpenGL
	init_open_gl();
	
	// Register callback functions with GLUT
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//glutIdleFunc(idle);
	
	// Determine the axes and grid
	axes_dl = axes_dl_gen();
	grid_dl = grid_dl_gen();
	
	glewInit();
	/*
	if (glewIsSupported("GL_VERSION_2_1"))
		printf("Ready for OpenGL 2.1\n");
	else {
		printf("OpenGL 2.1 not supported\n");
		exit(1);
	}
	*/
	
	if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader &&
				GLEW_EXT_geometry_shader4 && GLEW_EXT_framebuffer_object &&
				GLEW_ARB_texture_float)) {
		printf("Not totally ready :( \n");
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
	
	// Render buffer
	/*
	glGenRenderbuffersEXT(1, &depth_rb);
  glBindRenderbufferEXT(GL_RENDERBUFFER, depth_rb);
  glRenderbufferStorageEXT(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
	*/
	
	glGenFramebuffersEXT(1, &fb);
  glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, fb);
	//glFramebufferTexture2DEXT(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ping, 0);
  //glFramebufferRenderbufferEXT(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);

  // Check framebuffer completeness at the end of initialization.
  //check_framebuffer();

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
	pde->set_param("wtime", wall.time());
	
	glUseProgram(pde->pc);
	pde->dl_gen(scr);
	
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
	
	//glClear(GL_COLOR_BUFFER_BIT);
	
	scr.time = wall.time();
	
	calculate();
	
	//if (framecount == 0) {
		map<primitive*, GLint>::iterator it = primitives.begin();
		// For every curve, ...
		for (it = primitives.begin(); it != primitives.end(); ++it) {
		
			glColor3f(it->first->c.r, it->first->c.g, it->first->c.b);
			// Draw the damned thing
			glUseProgram(it->first->pc);
			//glCallList(it->second);
			it->first->dl_gen(scr);
		}
	
		// Bind the display framebuffer to render the results
		glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, render);
	
		// I don't understand why I have to use this pr, and can't use 0 :-/
		glUseProgram(pde->pr);
		//glUseProgram(0);
		pde->dl_gen(scr);
	//}
	
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
		case '+':
			width *= 2;
			pde->set_param("width", width);
			height *= 2;
			pde->set_param("height", height);
			break;
		case '-':
			width /= 2;
			pde->set_param("size", width);
			height /= 2;
			pde->set_param("height", height);
			break;
		case 'a':
			if (keyboard_options & AXES_KEYS_ON) {
				display_options = AXES_ON ^ display_options;
				glutPostRedisplay();
			}
			break;
		case 'g':
			if (keyboard_options & GRID_KEYS_ON) {
				display_options = GRID_ON ^ display_options;
				glutPostRedisplay();
			}
			break;
		case 'q':
			if (keyboard_options & QUIT_KEYS_ON) {
				cout << "Quitting." << endl;
				exit(0);
				break;
			}
		case 'd':
			glutPostRedisplay();
			break;
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

// Our mouse funciton, registered with OpenGL
void grapher::mouse(int button, int state, int x, int y) {
	
	// If it's a mouse button being pressed down,
	if (state == GLUT_DOWN) {
		/** Set the x and y coordinates of the mouse where
		  * it is now, so that when it comes up, we can check
		  * if it has moved or not.  If not, then we raise a
		  * click event.
		  */
		startx = x;
		starty = y;
	} else if (state == GLUT_UP) {
		// See if the mouse has moved since it was pressed
		double dx = get_x_coord(startx) - get_x_coord(x);
		double dy = get_y_coord(starty) - get_y_coord(y);
		
		// If it has moved,
		if (dx != 0 || dy != 0) {
			// then translate the plot by the amount moved
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			scr.minx += dx;
			scr.maxx += dx;
			scr.miny += dy;
			scr.maxy += dy;
			glOrtho(scr.minx, scr.maxx, scr.miny, scr.maxy, -10, 0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			startx = starty = -1;
			refresh_dls();
			glutPostRedisplay();
		} else if (user_click_function) {
			/** Otherwise (if the mouse has not moved), raise
			  * a mouse click event.  If the user has defined
			  * a click handler, call it.
			  */
			user_click_function(button, x, y);
		}
	}
}

void grapher::idle() {
	if (user_idle_function != NULL) {
		user_idle_function();
	}
}

void grapher::motion(int x, int y) {
	double dx = get_x_coord(startx) - get_x_coord(x);
	double dy = get_y_coord(starty) - get_y_coord(y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(scr.minx + dx, scr.maxx + dx, scr.miny + dy, scr.maxy + dy, -10, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void grapher::set_keyboard_function(keyboard_function k) {
	user_keyboard_function = k;
}

void grapher::set_click_function(click_function c) {
	user_click_function = c;
}

void grapher::set_idle_function(idle_function i) {
	glutIdleFunc(i);
}

void grapher::zoom(double scale) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double diff = (scr.maxx - scr.minx) * scale / 2.0;
	double mid = (scr.maxx + scr.minx) / 2.0;
	scr.minx = mid - diff;
	scr.maxx = mid + diff;
	
	diff = (scr.maxy - scr.miny) * scale / 2.0;
	mid = (scr.maxy + scr.miny) / 2.0;
	scr.miny = mid - diff;
	scr.maxy = mid + diff;
	
	glOrtho(scr.minx, scr.maxx, scr.miny, scr.maxy, -10, 0);
	//glViewPort()
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	refresh_dls();
	glutPostRedisplay();
}

double grapher::get_x_coord(GLint x) {
	return scr.minx + (scr.maxx - scr.minx) * ((double)x / scr.width);
}

double grapher::get_y_coord(GLint y) {
	return scr.maxy - (scr.maxy - scr.miny) * (double)y / scr.height;
}

GLint grapher::axes_dl_gen() {
	GLint dl = glGenLists(1);
	
	glNewList(dl, GL_COMPILE);
	
		glColor4d(0.0, 0.0, 0.0, 1.0);
		
		glBegin(GL_LINES);
		
			glVertex3d(0, scr.miny, 0);
			glVertex3d(0, scr.maxy, 0);
			glVertex3d(scr.minx, 0, 0);
			glVertex3d(scr.maxx, 0, 0);
		
		glEnd();
	
	glEndList();
	
	return dl;
}

GLint grapher::grid_dl_gen() {
	GLint dl = glGenLists(1);
	
	glNewList(dl, GL_COMPILE);
	
		glColor4d(0.0, 0.0, 0.0, 0.14);
	
		glBegin(GL_LINES);
		
			for ( int i = (int)scr.miny; i <= (int)scr.maxy; ++i) {
				glVertex3d(scr.minx, i, 1);
				glVertex3d(scr.maxx, i, 1);
			}
	
			for ( int i = (int) scr.minx; i <= scr.maxx; ++i) {
				glVertex3d(i, scr.miny, 1);
				glVertex3d(i, scr.maxy, 1);
			}
		
		glEnd();
		
	glEndList();
	
	return dl;
}

void grapher::add(primitive& p) {
	primitives[&p] = glGenLists(1);
	glNewList(primitives[&p], GL_COMPILE);
		(&p)->dl_gen(scr);
	glEndList();
}

void grapher::remove(primitive& p) {
	glDeleteLists(primitives[&p], 1);
	primitives.erase(&p);
}

void grapher::refresh_dls() {
	
	axes_dl = axes_dl_gen();
	grid_dl = grid_dl_gen();
	
	map<primitive*, GLint>::iterator it;
	for (it = primitives.begin(); it != primitives.end(); ++it) {
		glDeleteLists(it->second, 1);
		primitives[it->first] = glGenLists(1);
		glNewList(primitives[it->first], GL_COMPILE);
			(*it->first).dl_gen(scr);
		glEndList();
	}
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

//*
double grapher::x_plot_coord(double x) {
	if (X_LOG & display_options) {
		return pow(10,x);
	} else {
		return x;
	}
}

double grapher::x_screen_coord(double x) {
	if (X_LOG & display_options) {
		return log10(abs(x));
	} else {
		return x;
	}
}

double grapher::y_plot_coord(double y) {
	if (Y_LOG & display_options) {
		return pow(10, y);
	} else {
		return y;
	}
}

double grapher::y_screen_coord(double y) {
	if (Y_LOG & display_options) {
		return log10(abs(y));
	} else {
		return y;
	}
}
//*/

void grapher::refresh() {
	// Delete this!
	refresh_dls();
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

GLint grapher::get_width() {
	return scr.width;
}

GLint grapher::get_height() {
	return scr.height;
}

void grapher::get_pixels(char* values) {
	glReadPixels(0, 0, scr.width, scr.height, GL_RGB, GL_UNSIGNED_BYTE, values);
}

// Static member variable definition
// It's terribly ugly, I know
screen grapher::scr;
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
