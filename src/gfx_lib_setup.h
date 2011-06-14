#ifndef GFXLIBSETUP_H_
#define GFXLIBSETUP_H_

#ifdef __cplusplus
extern "C" {
#endif

struct context_holder
{
    int state; // 0 = Invalid; 1 = Allegro; 2 = Cogl
    ALLEGRO_DISPLAY *display;
    HDC hdc;
    HGLRC hglrc;
};

void
gfx_lib_setup (void);

void
context_cogl_allegro (void);

void
context_allegro_cogl (void);

#ifdef __cplusplus
}
#endif

#endif /* GFXLIBSETUP_H_ */
