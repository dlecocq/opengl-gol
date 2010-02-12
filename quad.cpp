#ifndef QUAD_CPP
#define QUAD_CPP

#include <iostream>
#include <fstream>

#include "quad.h"

using namespace glot;
using namespace std;

void quad::dl_gen(const screen& s) {

	glBegin(GL_QUADS);
		
	for (int i = 0; i < 4; ++i) {
		glVertex3f(points[i].x, points[i].y, 0.0);
	}
	
	glEnd();
}

#endif
