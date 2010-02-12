#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <iostream>

#include "color.h"

namespace glot {
	
	enum layout_opt { 
		X_LIN 		= 0,
		Y_LIN 		= 0,
		CARTESIAN = 0,
		X_LOG 		= 1,
		Y_LOG 		= 2,
		POLAR 		= 4 };
	
	/** \brief primitive: the parent class of all rendered objects
	  *
		* This class is the parent of all the primitives
		* that get drawn to the screen.
		*
		* \sa shader_primitive
	  */
	class primitive {
	
		public:
		
			/** The color of the the primitive */
			color c;
			
			/** Default constructor
			  */
			primitive(short int layout_opt = X_LIN | Y_LIN | CARTESIAN ) : c(color(0, 0, 0, 1)), pc(0), pr(0), layout(layout_opt) {};
			
			/** Constructor
			  * \param col - the color of the primitive 
			  *
			  * Initializes the color of the primitive and the
			  * program GLenum
			  */
			primitive(const color& col, short int layout_opt = X_LIN | Y_LIN | CARTESIAN ) : c(col), pc(0), pr(0), layout(layout_opt) {};
			
			/** Destructor
			  * 
			  * Virtual classes need virtual destructors
			  */
			virtual ~primitive() {};
			
			/** Display-list generator
			  */
			virtual void dl_gen(float minx, float maxx, float miny, float maxy) = 0;
			
			/** Shader program handle
			  * 
			  * Some primitives specify their own shader programs
			  * and the container class, grapher, will use each one's
			  * self-specified program when it's called.
			  */
			GLenum pc;
			GLenum pr;
			
			void set_param(const char* param, float value) {
				glUseProgram(pc);
				location = glGetUniformLocation(pc, param);
				glUniform1f(location, value);
				//std::cout << param << " (" << location << ") set to " << value << " in program " << pr << std::endl;
			}

		protected:
			
			GLint location;
			short int layout;
		
		private:
	
	};

}

#endif
