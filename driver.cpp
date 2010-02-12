#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include "grapher.h"

using namespace std;
using namespace glot;

int counter = 0;

quad::point* points = (quad::point*) malloc(4 * sizeof(quad::point));

color c(1.0, 0.0, 0.0, 0.5);

void click_function(GLint button, GLint x, GLint y) {
	points[counter].x = grapher::get_x_coord(x);
	points[counter].y = grapher::get_y_coord(y);
	counter += 1;
	if (counter == 4) {
		cout << "Adding a new quad" << endl;
		quad * q = new quad(points, color(1.0, 0.0, 0.0));
		grapher::add(*q);
		counter = 0;
	}
}

int main(int argc, char ** argv) {
	
	grapher::initialize(argc, argv, X_LIN | Y_LIN | AXES_ON | GRID_ON);
	
	color col(1.0, 0, 0);

	scalar_field sfp;
	grapher::pde = &sfp;
	grapher::set_click_function(click_function);
	grapher::set_idle_function(grapher::redraw);
	grapher::run();
	
	return 0;
};
