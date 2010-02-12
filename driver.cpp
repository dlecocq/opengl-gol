#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include "grapher.h"

using namespace std;
using namespace glot;

int counter = 0;

color c(1.0, 0.0, 0.0, 0.5);

int main(int argc, char ** argv) {
	
	grapher::initialize(argc, argv, X_LIN | Y_LIN | AXES_ON | GRID_ON);
	
	color col(1.0, 0, 0);

	scalar_field sfp;
	grapher::pde = &sfp;
	grapher::set_idle_function(grapher::redraw);
	grapher::run();
	
	return 0;
};
