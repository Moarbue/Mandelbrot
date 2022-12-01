#include "shader.h"
#include "GL/glew.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

// Size of each input chunk to be
// read and allocate for.
#ifndef  READ_FILE_CHUNK
#define  READ_FILE_CHUNK  262144
#endif

#define  READ_FILE_OK          0  // Success 
#define  READ_FILE_INVALID    -1  // Invalid parameters
#define  READ_FILE_ERROR      -2  // Stream error
#define  READ_FILE_TOOMUCH    -3  // Too much input
#define  READ_FILE_NOMEM      -4  // Out of memory


// This function returns one of the READ_FILE_ constants above.
// If the return value is zero == READ_FILE_OK, then:
//   (*dataptr) points to a dynamically allocated buffer, with
//   (*sizeptr) chars read from the file.
//   The buffer is allocated for one extra char, which is NUL,
//   and automatically appended after the data.
// Initial values of (*dataptr) and (*sizeptr) are ignored.
int read_file(FILE *in, char **dataptr, size_t *sizeptr)
{
    char  *data = NULL, *temp;
    size_t size = 0;
    size_t used = 0;
    size_t n;

    /* None of the parameters can be NULL. */
    if (in == NULL || dataptr == NULL || sizeptr == NULL)
        return READ_FILE_INVALID;

    /* A read error already occurred? */
    if (ferror(in))
        return READ_FILE_ERROR;

    while (1) {

        if (used + READ_FILE_CHUNK + 1 > size) {
            size = used + READ_FILE_CHUNK + 1;

            /* Overflow check. Some ANSI C compilers
               may optimize this away, though. */
            if (size <= used) {
                free(data);
                return READ_FILE_TOOMUCH;
            }

            temp = (char *)realloc(data, size);
            if (temp == NULL) {
                free(data);
                return READ_FILE_NOMEM;
            }
            data = temp;
        }

        n = fread(data + used, 1, READ_FILE_CHUNK, in);
        if (n == 0)
            break;

        used += n;
    }

    if (ferror(in)) {
        free(data);
        return READ_FILE_ERROR;
    }

    temp = (char *)realloc(data, used + 1);
    if (temp == NULL) {
        free(data);
        return READ_FILE_NOMEM;
    }
    data = temp;
    data[used] = '\0';

    *dataptr = data;
    *sizeptr = used;

    return READ_FILE_OK;
}


int shader_new(unsigned int *shader, const char *vertex_path, const char *fragment_path, const char *geometry_path) {
    // if shader pointer is NULL return
    if (!shader) {
        fprintf(stderr, "ERROR: shader_new() - Output parameter \'shader\' can't be NULL\n");
        return 1;
    }

    // *** vertex shader creation
    FILE *vert_in = fopen(vertex_path, "rb");     
    // Check if file stream was openend correctly
    if (vert_in == NULL) {
        fprintf(stderr, "ERROR: shader_new() - Failed to open vertex_path. Provided path was: %s\n", vertex_path);
        return 1;
    }
    
    // vertex shader code
    char *vertex_code; size_t file_size;
    int ret = read_file(vert_in, &vertex_code, &file_size);
    fclose(vert_in);
    
    // Check for reading errors
    switch (ret) {
        case READ_FILE_OK:
            fprintf(stdout, "INFO: Read file content from \'%s\' successfully.\n", vertex_path);
            break;
        case READ_FILE_ERROR:
            fprintf(stderr, "ERROR: shader_new() - A read error occured while reading from vertex shader file %s\n", vertex_path);
            return 1;
        case READ_FILE_TOOMUCH:
            fprintf(stderr, "ERROR: shader_new() - Vertex shader file %s is too big\n", vertex_path);
            return 1;
        case READ_FILE_NOMEM:
            fprintf(stderr, "ERROR: shader_new() - Ran out of memory while reading from vertex shader file %s\n", vertex_path);
            return 1;
    }

    // *** fragment shader creation
    FILE *frag_in = fopen(fragment_path, "rb");     
    // Check if file stream was openend correctly
    if (frag_in == NULL) {
        fprintf(stderr, "ERROR: shader_new() - Failed to open fragment_path. Provided path was: %s\n", fragment_path);
        return 1;
    }
    
    // fragment shader code
    char *fragment_code;
    ret = read_file(frag_in, &fragment_code, &file_size);
    fclose(frag_in);
    
    // Check for reading errors
    switch (ret) {
        case READ_FILE_OK:
            fprintf(stdout, "INFO: Read file content from \'%s\' successfully.\n", fragment_path);
            break;
        case READ_FILE_ERROR:
            fprintf(stderr, "ERROR: shader_new() - A read error occured while reading from fragment shader file %s\n", fragment_path);
            return 1;
        case READ_FILE_TOOMUCH:
            fprintf(stderr, "ERROR: shader_new() - Fragment shader file %s is too big\n", fragment_path);
            return 1;
        case READ_FILE_NOMEM:
            fprintf(stderr, "ERROR: shader_new() - Ran out of memory while reading from fragment shader file %s\n", fragment_path);
            return 1;
    }

    // geometry shader code
    char *geometry_code;
    if (geometry_path != NULL) {
        FILE *geometry_in = fopen(geometry_path, "rb");     
        // Check if file stream was openend correctly
        if (geometry_in == NULL) {
            fprintf(stderr, "ERROR: shader_new() - Failed to open geometry_path. Provided path was: %s\n", geometry_path);
            return 1;
        }
        
        ret = read_file(geometry_in, &geometry_code, &file_size);
        fclose(geometry_in);
        
        // Check for reading errors
        switch (ret) {
            case READ_FILE_OK:
                fprintf(stdout, "INFO: Read file content from \'%s\' successfully.\n", geometry_path);
                break;
            case READ_FILE_ERROR:
                fprintf(stderr, "ERROR: shader_new() - A read error occured while reading from geometry shader file %s\n", geometry_path);
                return 1;
            case READ_FILE_TOOMUCH:
                fprintf(stderr, "ERROR: shader_new() - Geometry shader file %s is too big\n", geometry_path);
                return 1;
            case READ_FILE_NOMEM:
                fprintf(stderr, "ERROR: shader_new() - Ran out of memory while reading from geometry shader file %s\n", geometry_path);
                return 1;
        }
    }

    int success;
    // Create vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const char * const *)&vertex_code, NULL);
    glCompileShader(vertex_shader);

    // Check for compile errors
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        fprintf(stderr, "ERROR: shader_new() - Failed to compile vertex shader. Provided file: %s\n", vertex_path);
        return 1;
    }
    fprintf(stdout, "INFO: Created vertex shader.\n");

    // Create fragment shader
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char * const *)&fragment_code, NULL);
    glCompileShader(fragment_shader);

    // Check for compile errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        fprintf(stderr, "ERROR: shader_new() - Failed to compile fragment shader. Provided file: %s\n", fragment_path);
        return 1;
    }
    fprintf(stdout, "INFO: Created fragment shader.\n");

    // *** geometry shader creation
    unsigned int geometry_shader;
    if (geometry_path != NULL) {

        // Create geometry shader
        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_shader, 1, (const char * const *)&geometry_code, NULL);
        glCompileShader(geometry_shader);

        // Check for compile errors
        glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            fprintf(stderr, "ERROR: shader_new() - Failed to compile geometry shader. Provided file: %s\n", geometry_path);
            return 1;
        }
        fprintf(stdout, "INFO: Created geometry shader.\n");
    }

    // *** shader program creation
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    if (geometry_path != NULL) glAttachShader(program, geometry_shader);
    glLinkProgram(program);

    // Check for linking errors
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        fprintf(stderr, "ERROR: shader_new() - Failed to link shader program.\nNote: Check shader sources for errors!\n");
        return 1;
    }
    fprintf(stdout, "INFO: Linked shaders to program.\n");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    if (geometry_path != NULL) glDeleteShader(geometry_shader);

    *shader = program;

    return 0;
}