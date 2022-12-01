#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include "GLFW/glfw3.h"

#include "GL/glew.h"

#include "shader.h"


#define DEFAULT_SCREEN_WIDTH 1000
#define DEFAULT_SCREEN_HEIGHT 1000
#define SCROLL_SENSITIVITY 0.025f
#define DEFAULT_MOVEMENT_X 0.001f
#define DEFAULT_MOVEMENT_Y 0.001f
#define DEFAULT_ZOOM 0.f
#define DEFAULT_XMIN -2.0
#define DEFAULT_XMAX 0.47
#define DEFAULT_YMIN -1.12
#define DEFAULT_YMAX 1.12

#define MAX_ITERATIONS 100

void glfw_error_callback(int error_code, const char *description);
void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height);
void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void check_keys(GLFWwindow *window);
void cleanup();

GLFWwindow *window;
int wwidth  = DEFAULT_SCREEN_WIDTH;
int wheight = DEFAULT_SCREEN_HEIGHT;

float xmin = DEFAULT_XMIN;
float xmax = DEFAULT_XMAX;
float ymin = DEFAULT_YMIN;
float ymax = DEFAULT_YMAX;
float zoom = DEFAULT_ZOOM;
float basex = DEFAULT_XMAX - DEFAULT_XMIN;
float basey = DEFAULT_YMAX - DEFAULT_YMIN;

int main()
{
    if (glfwInit() == GLFW_FALSE) {
        fprintf(stderr, "ERROR: Failed to load GLFW!\n");
        exit(1);
    }
    atexit(cleanup);
    glfwSetErrorCallback(glfw_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Mandelbrot OpenGL", NULL, NULL);
    if (window == NULL ) {
        fprintf(stderr, "ERROR: Failed to create window!\n");
        exit(1);
    }
    glfwSetFramebufferSizeCallback(window, glfw_framebuffer_resize_callback);
    glfwSetScrollCallback(window, glfw_scroll_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    {
        GLenum res = glewInit();
        if(res != GLEW_OK) {
            fprintf(stderr, "ERROR: Failed to load OpenGL Extensions!\n");
            exit(1);
        }
    }
    {
        int gl_ver_major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
        int gl_ver_minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
        printf("INFO: OpenGL Version %d.%d\n", gl_ver_major, gl_ver_minor);
    }

    GLuint program;
    int ret = shader_new(&program, "shaders/quad.vert", "shaders/mandelbrot.frag", NULL);
    if (ret == 1) {
        exit(1);
    }

    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLint uiters  = glGetUniformLocation(program, "iterations");
    GLint uwidth  = glGetUniformLocation(program, "width");
    GLint uheight = glGetUniformLocation(program, "height");
    GLint uxmin   = glGetUniformLocation(program, "xmin");
    GLint uxmax   = glGetUniformLocation(program, "xmax");
    GLint uymin   = glGetUniformLocation(program, "ymin");
    GLint uymax   = glGetUniformLocation(program, "ymax");

    glUniform1i(uiters, MAX_ITERATIONS);

    glClearColor(0.18, 0.18, 0.18, 1.0);

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);


        glUniform1i(uwidth, wwidth);
        glUniform1i(uheight, wheight);
        glUniform1f(uxmin, xmin);
        glUniform1f(uxmax, xmax);
        glUniform1f(uymin, ymin);
        glUniform1f(uymax, ymax);


        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 2);

        glfwSwapBuffers(window);

        check_keys(window);
        glfwPollEvents();
    }

    return 0;
}

void glfw_error_callback(int error_code, const char *description)
{
    (void) error_code;
    fprintf(stderr, "ERROR: %s\n", description);
}

void glfw_framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
    (void)window;

    ymin *= (float)wwidth / width;
    ymax *= (float)wwidth / width;
    xmin *= (float)wheight / height;
    xmax *= (float)wheight / height;

    basey *= (float)wwidth / width;
    basex *= (float)wheight / height;

    wwidth = width;
    wheight = height;
    glViewport(0, 0, width, height);
}

void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;

    zoom += (float)yoffset * SCROLL_SENSITIVITY;

    float centerx = (xmax + xmin) / 2.f;
    float centery = (ymax + ymin) / 2.f;
    float sizex = powf(2, -zoom) * basex;
    float sizey = powf(2, -zoom) * basey;

    xmin = centerx - sizex / 2;
    xmax = centerx + sizex / 2;
    ymin = centery - sizey / 2;
    ymax = centery + sizey / 2;
}

void check_keys(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    // moving around the set
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        xmin += DEFAULT_MOVEMENT_X * powf(2, -zoom);
        xmax += DEFAULT_MOVEMENT_X * powf(2, -zoom);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        xmin -= DEFAULT_MOVEMENT_X * powf(2, -zoom);
        xmax -= DEFAULT_MOVEMENT_X * powf(2, -zoom);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ymin += DEFAULT_MOVEMENT_Y * powf(2, -zoom);
        ymax += DEFAULT_MOVEMENT_Y * powf(2, -zoom);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ymin -= DEFAULT_MOVEMENT_Y * powf(2, -zoom);
        ymax -= DEFAULT_MOVEMENT_Y * powf(2, -zoom);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        double yoffset = 10.0 * SCROLL_SENSITIVITY;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
           yoffset *= -1; 
        
        glfw_scroll_callback(window, 0.0, yoffset);
    }
    
}

void cleanup()
{
    glfwDestroyWindow(window);
    printf("INFO: Destroyed window.\n");
    glfwTerminate();
    printf("INFO: Terminated GLFW.\n");
    printf("Exiting application...\n");
}