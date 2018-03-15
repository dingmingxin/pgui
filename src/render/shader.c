#include "opengl.h"
#include "shader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

static bool
check_compile_error(GLuint s) {
	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(s, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(s, 512, NULL, infoLog);
		printf("ERROR:shader: vertex compile failed:%s\n", infoLog);
	}
	if (!success) {
		return false;
	}
	return true;
}

void 
shader_init() {

}

void 
shader_program(int prog, const char *fs, const char *vs, int texture, const char ** texture_uniform_name) {

}

void 
shader_unload_all() {
}

void 
shader_use(const struct shader *s) {
	glUserProgram(s->glid);
}

void 
shader_uniform_property(struct shader *s, int loc, enum UNIFORM_FORMAT format, const float *v) {
	switch(format) {
		case UNIFORM_FLOAT1:
			{
				glUniform1f(loc, v[0]);
				break;
			}
		case UNIFORM_FLOAT2:
			{
				glUniform2f(loc, v[0], v[1]);
				break;
			}
		case UNIFORM_FLOAT3:
			{
				glUniform3f(loc, v[0], v[1], v[2]);
				break;
			}
		case UNIFORM_FLOAT4:
			{
				glUniform4f(loc, v[0], v[1], v[2], v[3]);
				break;
			}
		case UNIFORM_FLOAT33:
			{
				glUniformMatrix3fv(loc, 1, GL_FALSE, v);
				break;
			}
		case UNIFORM_FLOAT44:
			{
				glUniformMatrix4fv(loc, 1, GL_FALSE, v);
				break;
			}
		default:
			assert(0);

	}
}

int 
shader_uniform_location(struct shader *s, const char *name) {
	int loc = glGetUniformLocation(s->glid, name);
	return loc;
}



