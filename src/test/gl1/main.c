#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include <assert.h>

#define xassert(exp) do { ((exp)?0:++(*((char *)0x00000010))); } while (0)

#include <src/test/gl1/vshd_src.h>

int check_gl_error (void)
{
  GLenum err;
  err = glGetError ();
  if (GL_NO_ERROR != err) printf ("GLERROR: %x\n", err);
  xassert (GL_NO_ERROR == err);
}

int
make_fbo (void)
{
  check_gl_error ();

  GLenum err;
  
  GLuint tex;
  GLuint tex2;

  glGenTextures (1, &tex);
  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_2D, tex);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
		640, 480, 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		NULL);

  glGenTextures (1, &tex2);
  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_2D, tex2);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
		640, 480, 0,
		GL_RGBA, GL_UNSIGNED_BYTE,
		NULL);

  GLuint fbo;

  glGenFramebuffersEXT (1, &fbo);
  glBindFramebufferEXT (GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2DEXT (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			     GL_TEXTURE_2D, tex, 0);

  glFramebufferTexture2DEXT (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
			     GL_TEXTURE_2D, tex2, 0);

  xassert (GL_FRAMEBUFFER_COMPLETE ==
	   glCheckFramebufferStatusEXT (GL_FRAMEBUFFER));

  check_gl_error ();

  GLint status;

  GLuint vshd;
  GLuint fshd;

  vshd = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vshd, 1, vshd_src, NULL);
  glCompileShader (vshd);
  
  glGetShaderiv (vshd, GL_COMPILE_STATUS, &status);
  xassert (GL_TRUE == status);

  fshd = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fshd, 1, fshd_src, NULL);
  glCompileShader (fshd);
  
  glGetShaderiv (fshd, GL_COMPILE_STATUS, &status);
  xassert (GL_TRUE == status);

  GLuint prog;

  check_gl_error ();

  prog = glCreateProgram ();
  glAttachShader (prog, vshd);
  glAttachShader (prog, fshd);  
  glLinkProgram (prog);
  
  glGetProgramiv (prog, GL_LINK_STATUS, &status);
  xassert (GL_TRUE == status);

  check_gl_error ();

  GLint cat0_loc;

  cat0_loc = glGetAttribLocation (prog, "cat0");
  xassert (-1 != cat0_loc);

  check_gl_error ();

  GLuint ab;

  GLubyte *abbuf;
  GLfloat *abbuf_f;
  abbuf = malloc (4*4 * 3);
  abbuf_f = (GLfloat *)abbuf;
  xassert (abbuf);

  abbuf_f[0] = 0.0f;
  abbuf_f[1] = 0.0f;
  abbuf_f[2] = 0.0f;
  abbuf_f[3] = 1.0f;
  abbuf_f[4] = 100.0f;
  abbuf_f[5] = 0.0f;
  abbuf_f[6] = 0.0f;
  abbuf_f[7] = 1.0f;
  abbuf_f[8] = 50.0f;
  abbuf_f[9] = 100.0f;
  abbuf_f[10] = 0.0f;
  abbuf_f[11] = 1.0f;

  glGenBuffers (1, &ab);
  glBindBuffer (GL_ARRAY_BUFFER, ab);
  glBufferData (GL_ARRAY_BUFFER,
		4*4 * 3, abbuf,
		GL_STREAM_DRAW);

  glVertexAttribPointer (cat0_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glEnableVertexAttribArray (0);

  check_gl_error ();

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fbo);

  {
    glPushAttrib (GL_VIEWPORT_BIT | GL_TRANSFORM_BIT);
    glViewport (0, 0, 640, 480);
    glClearColor (0, 1, 0, 1);
    glClear (GL_COLOR_BUFFER_BIT);

    {
      glMatrixMode (GL_PROJECTION);
      glPushMatrix ();
      glLoadIdentity ();
      glOrtho (0, 256, 256, 0, -1, 1);
  
      GLenum draw_buffers[] = {GL_COLOR_ATTACHMENT0,
			       GL_COLOR_ATTACHMENT1};
      glDrawBuffers (2, draw_buffers);
      glUseProgram (prog);

      glDrawArrays (GL_TRIANGLES, 0, 3);

      glMatrixMode (GL_PROJECTION);
      glPopMatrix ();
    }

    glPopAttrib ();
  }

  glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

  check_gl_error ();

  glClearColor (0, 0, 1, 1);
  glClear (GL_COLOR_BUFFER_BIT);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glEnable (GL_TEXTURE_2D);
  glUseProgram (0);

  glBindTexture(GL_TEXTURE_2D, tex);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(0, 0);
  glTexCoord2f(1, 0); glVertex2f(100, 0);
  glTexCoord2f(1, 1); glVertex2f(100, 100);
  glTexCoord2f(0, 1); glVertex2f(0, 100);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, tex2);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0); glVertex2f(0, 150);
  glTexCoord2f(1, 0); glVertex2f(100, 150);
  glTexCoord2f(1, 1); glVertex2f(100, 250);
  glTexCoord2f(0, 1); glVertex2f(0, 250);
  glEnd();
}

int
main (int argc, char **argv)
{
  al_init ();

  al_set_new_display_option (ALLEGRO_RED_SIZE, 8,  ALLEGRO_REQUIRE);
  al_set_new_display_option (ALLEGRO_GREEN_SIZE, 8, ALLEGRO_REQUIRE);
  al_set_new_display_option (ALLEGRO_BLUE_SIZE, 8, ALLEGRO_REQUIRE);
  al_set_new_display_option (ALLEGRO_ALPHA_SIZE, 8, ALLEGRO_REQUIRE);
  al_set_new_display_option (ALLEGRO_AUX_BUFFERS, 4, ALLEGRO_REQUIRE);

  al_set_new_display_flags (ALLEGRO_WINDOWED | ALLEGRO_OPENGL);

  ALLEGRO_DISPLAY *disp;
  disp = al_create_display (640, 480);
  xassert (disp);

  al_set_target_backbuffer (disp);

  ALLEGRO_BITMAP *bmp;
  bmp = al_create_bitmap (640, 480);
  xassert (bmp);

  printf ("OPENGL %x\n", al_get_opengl_version ());

  GLint glvar;
  /* Query GL_MAX_DRAW_BUFFERS, GL_MAX_COLOR_ATTACHMENTS */
  glGetIntegerv (GL_MAX_DRAW_BUFFERS, &glvar);
  xassert (4 <= glvar);
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &glvar);
  xassert (4 <= glvar);

  make_fbo ();

  al_flip_display ();

  al_rest (2);

  return EXIT_SUCCESS;
}
