#ifndef MANDELBROT_H
#define MANDELBROT_H

#ifdef __cplusplus
extern "C" {
#endif

// This function returns 0 on success or 1 on failure
// For error information see the standard error stream
// 
// Parameters:
// (*shader) - [out] - the pointer to the shader that is created
// (*vertex_path) - [in] - the path to the vertex shader
// (*fragment_path) - [in] - the path to the fragment shader
// (*geometry_path) - [in] - the path to the geometry shader, optional, pass NULL if not needed
int shader_new(unsigned int *shader, const char *vertex_path, const char *fragment_path, const char *geometry_path);

#ifdef __cplusplus
}
#endif

#endif // MANDELBROT_H