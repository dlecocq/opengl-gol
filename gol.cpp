#ifndef SCALAR_FIELD_CPP
#define SCALAR_FIELD_CPP

#include <iostream>
#include <fstream>

#include "gol.h"

using namespace glot;

void scalar_field::dl_gen(float minx, float maxx, float miny, float maxy) {
	glBegin(GL_QUADS);
		//*
		glTexCoord2f(0, 0);
			glVertex3f(minx, miny, -1);
		glTexCoord2f(0, 1);
			glVertex3f(minx, maxy, -1);
		glTexCoord2f(1, 1);
			glVertex3f(maxx, maxy, -1);
		glTexCoord2f(1, 0);
			glVertex3f(maxx, miny, -1);
		//*/
		/*
		glTexCoord2f(0, 0);
			glVertex3f(0, 0, 0);
		glTexCoord2f(0, 1);
			glVertex3f(0, 1, 0);
		glTexCoord2f(1, 1);
			glVertex3f(1, 1, 0);
		glTexCoord2f(1, 0);
			glVertex3f(1, 0, 0);
		//*/
	glEnd();
}

void scalar_field::gen_shader() {
	string program = read_file("shaders/scalar.calc.frag");

	GLhandleARB frag = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar * ff = program.c_str();

	glShaderSource(frag, 1, &ff, NULL);

	glCompileShader(frag);

	pc = glCreateProgram();
	
	glAttachShader(pc,frag);
	
	glLinkProgram(pc);
	
	cout << "Fragment shader: ";
	printShaderInfoLog(frag);
	
	cout << "Program: ";
	printProgramInfoLog(pc);
	
	glDeleteShader(frag);
	
	// Generate the render shader

	frag = glCreateShader(GL_FRAGMENT_SHADER);

	ff = read_file("shaders/scalar.render.frag").c_str();

	glShaderSource(frag, 1, &ff, NULL);

	glCompileShader(frag);

	pr = glCreateProgram();
	
	glAttachShader(pr,frag);
	
	glLinkProgram(pr);
	
	cout << "Fragment shader: ";
	printShaderInfoLog(frag);
	
	cout << "Program: ";
	printProgramInfoLog(pr);
	
	glDeleteShader(frag);
}

#endif
