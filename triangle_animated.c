#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <math.h>

#include "glextloader.h"
#include "file.h"
#include "gl_compile_errors.h"

#define WIDTH   800
#define HEIGHT  600

GLuint compile_shaders(void);

const char *triangle_vert_shader = "shaders/triangle_animated.vert";
const char *triangle_frag_shader = "shaders/triangle_animated.frag";

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
    GLuint program;
    GLuint vertex_array_object;

    const char *vert_shader_source = read_file(triangle_vert_shader);
    const char *frag_shader_source = read_file(triangle_frag_shader);

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

    // create program
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);

    // delete shaders because they are part of the program now
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);

    // init vertext arrays
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

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


        const GLfloat color[] = {1.0f,0.5f,0.5f,1.0f };
        glClearBufferfv(GL_COLOR, 0, color);

        glUseProgram(program);

        time_t now = time(0);

        GLfloat attrib[] = {
          ((float)sin(now) * 0.5f),
          ((float)cos(now) * 0.6f),
          0.0f, 1.0f
        };

        GLfloat tri_color[] = {1.0, 1.0, 0.5, 0.0};

        // this updates the "in" variable
        // in shaders/triangle.vert
        glVertexAttrib4fv(0, attrib);
        glVertexAttrib4fv(1, tri_color);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        // takes the 
        glXSwapBuffers(display, window);
    }

    XCloseDisplay(display);
}
