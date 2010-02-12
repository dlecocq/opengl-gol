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
			static int initialize(int argc, char ** argv, short int options = AXES_ON | GRID_ON | X_LIN | Y_LIN, short int k_options = ZOOM_KEYS_ON | AXES_KEYS_ON | GRID_KEYS_ON | QUIT_KEYS_ON);
		
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

			/** \brief Add a primitive to the plot
			  * \param p - the primitive to add
			  * 
			  * Add any primitive that inherits from the primitive class
			  * to the plot.  The only requirement is that dl_gen makes
			  * the required OpenGL calls to render the geometry.
			  */
			static void add(primitive& p);
			
			/** \brief Remove a primitive from the plot
			  * \param p - the primitive to remove
			  * 
			  * Remove a previously-added primitive from the plot
			  */
			static void remove(primitive& p);
				
			/** \brief Set up a keyboard event handler
			  *	\param k - the function you'd like to handle key events
			  *
			  *	If you'd like to set up a function to handle key events,
			  * you can register them with this function.  NOTE: there are
			  * default behaviors for certain keys that are not overridden
			  * here.  For example, '+' zooms in, but if you use that
			  * key as well, both a zoom and your operation will take
			  * place.
			  *
			  * The idea behind this is that these default behaviors are
			  * not the programmer's responsibility to code up as well.
			  * They are interface freebies.
			  *
			  *	Bitwise or options together from the set:
			  * ZOOM_KEYS_ON / ZOOM_KEYS_OFF
			  * AXES_KEYS_ON / AXES_KEYS_OFF
			  * GRID_KEYS_ON / GRID_KEYS_OFF
			  */
			static void set_keyboard_function(keyboard_function k);
		
			/**	\brief Set up a click event handler
			  *	\param c - the function you'd like to handle click events
			  *
			  * If you'd like to set up a function to handle click events
			  * (which are when a user presses down and then releases a
			  * button at the same spot), you can register it with this
			  * function.  NOTE: this is different from a motion function.
			  */
			static void set_click_function(click_function c);
			
			/** \brief Set the idle function handler
			  *
			  * Animation is done by setting the idle_function handler,
			  * and this provides the interface.  This might be deprecated
			  * in favor of an option that just allows the user to turn
			  * on animation, which would consist of simply setting
			  * OpenGL's idle function to grapher's display function.
			  */
			static void set_idle_function(idle_function i);
			
			static GLint get_width()  { return width;  };
			static GLint get_height() { return height; };
			static void get_pixels(char* values);
			
			static primitive* pde;

		private:
			
			/** \brief Refresh all the display-lists
			  *
			  * Refreshes all the display lists.  This function is a 
			  * candidate for deprecation because increasingly, the
			  * primitives offload their work onto the graphics card
			  * and so the number of static primitives they generate
			  * is rather small.  Furthermore, it's reached a point
			  * where compilation of display lists may actually be 
			  * slowing down performance.
			  */
			static void refresh();
		
			/**	\brief Transform a y coordinate
			  *	\param y - the y coordinate to transform
			  *
			  * This grapher gives the option of using linear or 
			  * logarithmic scales, and so this function transforms
			  * a real (linear) y coordinate into the coordinate
			  * appropriate for this plot.  If it's linear, it will
			  * remain linear, but if the plot has a logarithmic y
			  *	scale, it will be transformed by log10(abs(y)).
			  */
			static double y_coord_transform(double y);
		
			/**	\brief Transform a x coordinate
			  *	\param x - the x coordinate to transform
			  *
			  * This grapher gives the optiopn of using linear or
			  *	logarithmic scales, and so this function transforms
			  * a world x coordinate into the correct plot x
			  * coordinate.  For example, if X_LOG is active in the
			  *	display options, then it will transform x to be 
			  * exp(10, x).
			  */
			static double x_coord_transform(double x);
			
			//*/
		
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
			
			static void idle();
		
			/**	\brief Regenerate all display lists
			  *
			  *	If the plot moves, or for whatever reason you need to 
			  * recalculate all the display lists, you can call this
			  * function.
			  */
			static void refresh_dls();

			/** \brief The display options for the grapher
			  *	
			  *	It's a bitwise or'ing of the options AXES_ON / AXES_OFF,
			  * GRID_ON / GRID_OFF, etc.
			  */
			static short int display_options;
		
			/** \brief The default keyboard actions options
			  *
			  * It's a bitwise or'ing of the options ZOOM_KEYS_ON /
			  *	ZOOM_KEYS_OFF, AXES_KEYS_ON / AXES_KEYS_OFF,
			  *	GRID_KEYS_ON / GRID_KEYS_OFF
			  */
			static short int keyboard_options;

			/** The display list for the axes */
			static GLint axes_dl;
			/** The display list for the grid */
			static GLint grid_dl;

			/** For generating click vs. mouse events */
			static int startx;
			static int starty;
		
			/** The user-defined keyboard-event fuction */
			static keyboard_function user_keyboard_function;
		
			/** The user-defined click-event function */
			static click_function user_click_function;
			
			static idle_function user_idle_function;
		
			/** All the primitives */
			static map<primitive*, GLint> primitives;
			
			static stopwatch timer;
			static stopwatch wall;
			
			static int framecount;
			
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
