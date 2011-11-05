/**
 * ERR LET ME GET THIS RIGHT:
 * ARE YOU REALLY TELLING ME THE SHADER COMPILER
 * CAN USE STATIC ANALYSIS AND OPTIMISE ATTRIBUTE VARIABLES OUT
 * SO glGetAttribLocation FAILS?
 * THIS IS INSANE, RETARDED, THEN INSANE AGAIN.
 *
 * Also don't use custom attributes and fixed pipeline together
 * (If one of a vertex/fragment shader is not present, that counts as ffp!)
 */

const GLchar *vshd_src[] = {"\
attribute vec4 cat0;   \
\
void main(void)\
{\
  gl_Position = gl_ModelViewProjectionMatrix * cat0;\
}\
"};

const GLchar *fshd_src[] = {"\
void main()\
{\
  gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);\
  gl_FragData[1] = vec4(1.0, 1.0, 0.0, 1.0);\
}\
"};
