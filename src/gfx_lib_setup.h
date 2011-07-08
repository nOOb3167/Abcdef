#ifndef GFXLIBSETUP_H_
#define GFXLIBSETUP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cogl/cogl.h>

struct ALLEGRO_DISPLAY;


struct context_holder
{
    int state; // 0 = Invalid; 1 = Allegro; 2 = Cogl
    ALLEGRO_DISPLAY *display;
    HDC hdc;
    HGLRC hglrc;
};

struct context_fbstate
{
    ALLEGRO_DISPLAY *display;
    CoglHandle *offscreen;
    CoglHandle *texture;
    int width;
    int height;
};

struct fbstate_data
{
    ALLEGRO_DISPLAY *display;
    char *data;
    int size;
    int width;
    int height;
};

void
gfx_lib_setup (void);

void
context_cogl_allegro (void);

void
context_allegro_cogl (void);

void
context_switch_allegro (void);

void
context_switch_cogl (void);

struct fbstate_data
fbstate_get_data (void);

#ifdef __cplusplus
}
#endif

#endif /* GFXLIBSETUP_H_ */
