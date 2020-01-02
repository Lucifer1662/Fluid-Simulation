#pragma once

#include <GL\glew.h>
#include <string>
#include <memory>

using std::string;

GLuint CreateShader(GLenum type, string source);

unsigned int CreateProgram(const string& vsCode, const string& fsCode);
unsigned int CreateProgram(const string& vsCode, const string& gsCode, const string& fsCode);
unsigned int CreateProgram(const string & computeCode);


struct _Program {
	unsigned int program;
	_Program(int typeId, bool load);
	~_Program();
};

typedef std::shared_ptr<_Program> Program;