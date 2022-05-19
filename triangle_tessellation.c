// This is only showing the outline of the triangle
//


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
/* #include <GL/glext.h> */
#include <math.h>

#include "glextloader.h"
#include "file.h"
#include "gl_compile_errors.h"

#define WIDTH   800
#define HEIGHT  600

GLuint compile_shaders(void);

const char *tessellation_vert_shader = "shaders/tessellation.vert";
const char *tessellation_frag_shader = "shaders/tessellation.frag";
const char *tessellation_control_shader = "shaders/tessellation.tesc";
const char *tessellation_evaluation_shader = "shaders/tessellation.tese";

int main() {
    LoadGLExtensions(); 
    Display *display = XOpenDisplay(NULL);

    XVisualInfo *visual = glXChooseVisual(display, 0, (int[]){
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    });
    if (!visual) {
        fprintf(stderr, "Unable to choose visual\n");
        exit(1);
    }
    printf("Visual ID: %ld\n", visual->visualid);

    GLXContext gl_context = glXCreateContext(display, visual, 0, True);
    if (!gl_context) {
        fprintf(stderr, "Unable to create GL context\n");
        exit(1);
    }

    Window window = XCreateSimpleWindow(
        display,
        XDefaultRootWindow(display),    // parent
        0, 0,                           // x, y
        WIDTH, HEIGHT,                  // width, height
        0,                              // border width
        0x00000000,                     // border color
        0x00000000                      // background color
    );

    glXMakeCurrent(display, window, gl_context);

    XStoreName(display, window, "opengl");

    XSelectInput(display, window, KeyPressMask|KeyReleaseMask);

    XMapWindow(display, window);

    GLuint vertex_shader;
    GLuint frag_shader;
    GLuint control_shader;
    GLuint evaluation_shader;
    GLuint program;
    GLuint vertex_array_object;

    const char *vert_shader_source = read_file(tessellation_vert_shader);
    const char *frag_shader_source = read_file(tessellation_frag_shader);
    const char *control_shader_source = read_file(tessellation_control_shader);
    const char *evaluation_shader_source = read_file(tessellation_evaluation_shader);

    // compiles vert shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vert_shader_source, NULL);
    glCompileShader(vertex_shader);
    checkCompileErrors(vertex_shader, "VERTEX");

    // compiles frag shader
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_shader_source, NULL);
    glCompileShader(frag_shader);
    checkCompileErrors(frag_shader, "FRAGMENT");

    // tessellation control shader
    control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(control_shader, 1, &control_shader_source, NULL);
    glCompileShader(control_shader);
    checkCompileErrors(control_shader, "TESS CONTROL");

    // tessellation evaluation shader
    evaluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(evaluation_shader, 1, &evaluation_shader_source, NULL);
    glCompileShader(evaluation_shader);
    checkCompileErrors(evaluation_shader, "TESS EVALUATION");

    // create program
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, control_shader);
    glAttachShader(program, evaluation_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    // delete shaders because they are part of the program now
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);
    glDeleteShader(control_shader);
    glDeleteShader(evaluation_shader);

    // init vertext arrays
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    bool quit = false;
    while (!quit) {
        while (XPending(display) > 0) {
            XEvent event = {0};
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                if (keysym == XK_Escape) {
                    quit = true;
                }
            }
        }

        const GLfloat color[] = {0.3f,0.5f,0.75f,1.0f };

        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(program);

        glDrawArrays(GL_PATCHES, 0, 3);
        printf("OPENGL ERROR CODE: %d \n", glGetError());


        // takes the 
        glXSwapBuffers(display, window);
    }

    XCloseDisplay(display);
}
