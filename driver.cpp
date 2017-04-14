#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include "grapher.h"

using namespace std;
using namespace glot;

int main(int argc, char ** argv) {
	
	grapher::initialize(argc, argv);

	scalar_field sfp;
	grapher::pde = &sfp;
	grapher::run();
	
	return 0;
};
