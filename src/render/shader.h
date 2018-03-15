#ifndef  SHADER_INC
#define  SHADER_INC

#include <stdint.h>
#include "common.h"

struct shader {
	GLuint glid
};

void shader_init();
void shader_program(int prog, const char *fs, const char *vs, int texture, const char ** texture_uniform_name);
void shader_unload_all();
void shader_use(const struct shader *s);
void shader_uniform_property(struct shader *s, int loc, enum UNIFORM_FORMAT format, const float *v);
int shader_uniform_location(struct shader *s, const char *name);

#endif   /* ----- #ifndef SHADER_INC  ----- */
