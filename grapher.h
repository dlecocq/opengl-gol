#ifndef GRAPHER_H
#define GRAPHER_H

#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include <list>
#include <map>

#include "shader_primitive.h"
#include "scalar_field.h"
#include "stopwatch.h"
#include "color.h"

using namespace std;

namespace glot {
	
	/** Enumeration for display options
	  *
	  * Bitwise or these to set the display options
	  */
	enum display_opt { AXES_OFF = 0, 
		GRID_OFF = 0,
		AXES_ON = 1,
		GRID_ON = 2 };

	/** Enumeration for keyboard action options
	  *
	  * Bitwise or these to set the keyboard action options
	  */
	enum keyboard_opt { ZOOM_KEYS_OFF = 0,
		AXES_KEYS_OFF = 0,
		GRID_KEYS_OFF = 0,
		QUIT_KEYS_OFF = 0,
		ZOOM_KEYS_ON = 1,
		AXES_KEYS_ON = 2,
		GRID_KEYS_ON = 4,
		QUIT_KEYS_ON = 8 };

	/** \brief grapher: an interactive plotter display
	  *
		* You might think of this class as a container for
		* all the graphing primitives you plan to plot
		*
		* \sa p_curve
		* \sa point
		* \sa curve
	  */
	class grapher {
		
		public:

			/**	\brief Keyboard event handler typedef
			  *	\param key - unsigned char
			  *	\param x - GLint x coordinate
			  * \param y - GLint y coordinate
			  *
			  * A keyboard event handler accepts a key and x, y coordinates
			  */
			typedef void (*keyboard_function)(unsigned char key, GLint x, GLint y);

			/** \brief Click even handler typedef
			  * \param button - GLint button pressed
			  * \param x - GLint x coordinate
			  * \param y - GLint y coordinate
			  *
			  * See set_click_function(...) for more details
			  */
			typedef void (*click_function)(GLint button, GLint x, GLint y);
			
			/** \brief The idle function typedef
			  *
			  * See set_idle_funciton(...) for more details
			  */
			typedef void (*idle_function)(void);
		
			/** \brief Initialize the grapher
			  * \param argc - same as argc used for OpenGL initialization
			  * \param argv - same as argv used for OpenGL initialization
			  * \param options - startup options
			  * \param k_options - the default keyboard actions options
			  *
			  * Use a bitwise or to select startup options:
			  * AXES_ON, AXES_OFF
			  * GRID_ON, GRID_OFF
			  * X_LIN, X_LOG (linear x scale or logarithmic)
			  * Y_LIN, Y_LOG (linear y scale or logarithmic)
			  */
			static int initialize(int argc, char ** argv);
		
			/** \brief Enter the OpenGL main loop after initialization
			  *
			  *	In general, you will set up your event handlers, main
			  * code, etc., and then when you've gotten everything in
			  * place, you call grapher::run() to start the program's
			  * OpenGL portion.
			  */
			static void run();
		
			/**	\brief User-requested redraw
			  *	
			  *	If you create an event handler that will make some
			  * changes to the graph, you can make those changes and
			  * then request a redraw with this function.
			  */
			static void redraw();
			
			static GLint get_width()  { return width;  };
			static GLint get_height() { return height; };
			static void get_pixels(char* values);
			
			static primitive* pde;

		private:
		
			/**	\brief Initialize OpenGL
			  */
			static int init_open_gl();
			
			static void init_framebuffer();
			
			static int check_framebuffer();
		
			static void calculate();
		
			/**	\brief Display the plot
			  *
			  *	This is the grapher's event handler it registers with
			  *	OpenGL to display the plot.  It iterates over all its
			  *	primitives and displays them in turn.
			  */
			static void display();
		
			/**	\brief Reshape function
			  *	\param w - new width
			  *	\param h - new height
			  *
			  *	This is the grapher's reshape event handler is registers
			  *	with OpenGL.
			  */
			static void reshape(int w, int h);
		
			/** \brief Keyboard event function
			  *	\param key - the key depressed
			  *	\param x - the x screen coordinate of the mouse
			  *	\param y - the y screen coordinate of the mouse
			  *
			  * This encapsulates certain default key behavior (like
			  *	zooming in or zooming out), and is what grapher registers
			  * with OpenGL to handle keyboard events.  If the user
			  *	defines their own keyboard event handler, it will execute
			  * that function AFTER it's done.
			  */
			static void keyboard(unsigned char key, int x, int y);
			
			static GLuint fb;
			static GLuint tex;
			static GLuint render;
			static GLuint depth_rb;
			
			static int width;
			static int height;
			
			static bool initialized;

	};

}

#endif
