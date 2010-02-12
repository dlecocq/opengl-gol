#ifndef QUAD_H
#define QUAD_H

#include <GL/glew.h>
#include <string>
#include <cmath>

#include "primitive.h"
#include "color.h"

namespace glot {

	/** \brief curve: a curve to plot
	  *
		* This class takes a function that accepts a double
		* and returns a double and does its best to plot it
		* in the grapher.
		*
		* \sa grapher
		* \sa p_curve
	  */
	class quad : public primitive {
	
		public:
			
			struct point {
				float x, y;
			};
			
			/** Color variable
			  *
			  * This is the color the curve is supposed to
			  * take on.
			  */
			color c;
		
			/** Constructor
			  *	\param func - the function to render
			  *	\param col - the color of the curve
			  */
			quad(point * pts, const color& col, short int layout_opt = X_LIN | Y_LIN) : primitive(col, layout_opt), points(NULL) {
				points = (point*) malloc(4 * sizeof(point));
				for (int i = 0; i < 4; ++i) {
					points[i].x = pts[i].x;
					points[i].y = pts[i].y;
				}
			};
			
			void dl_gen(const screen& s);
		
		private:
			
			point * points;
	
	};
	
}

#endif
