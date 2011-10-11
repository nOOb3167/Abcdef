#include <allegro5/allegro.h>
#include <src/test/m-tfthread.h>
#include <src/test/m-tfthreadallegro.h>

NX_GET_NEW (m_tfthreadallegro);

GObject *
m_tfthreadallegro_new (void)
{
  MTfThreadAllegro *self;
  MTfThread *par;

  self = M_TFTHREADALLEGRO (nx_get_new ());
  par = M_TFTHREAD (self);

  self->qu_in = g_async_queue_new ();
  g_xassert (self->qu_in);

  self->inited = FALSE;

  return G_OBJECT (self);
}

void
m_tfthreadallegro_init_inthread (MTfThreadAllegro *self)
{
  g_xassert (FALSE == self->inited);

  _allegro_setup (&self->disp);

  self->inited = TRUE;
}

void
m_tfthreadallegro_send_ (MTfThread *self, MTfMsg * msg)
{
  MTfThreadAllegro *this;
  this = M_TFTHREADALLEGRO (self);

  /* You CAN send messages without inthread initialization.
   * Race condition if guard enabled. */
  //g_xassert (this->inited);

  g_object_ref (msg);
  g_async_queue_push (this->qu_in, msg);
}

void
m_tfthreadallegro_event_loop_enter (MTfThreadAllegro *self)
{
  g_xassert (self->inited);

  while (TRUE)
    {
      MTfMsg *msg;
      msg = M_TFMSG (g_async_queue_pop (self->qu_in));

      _m_tfthreadallegro_process_incoming_one (self, msg);
    }
}

void
_m_tfthreadallegro_process_incoming_one (MTfThreadAllegro *self, MTfMsg *msg)
{
  switch (msg->msg_type)
  {
  case TF_MSGTYPE_REDRAW:
    {
      g_xassert (0);
      break;
    }

  case TF_MSGTYPE_FBUP:
    {
      _m_tfthreadallegro_display_transfer (self->disp, M_TFMSG_FBUP (msg));
      break;
    }

  default:
      g_xassert (("EEH MAJI, UNHANDLED MESSAGE TYPE", FALSE));
      break;
  }
}

void
_m_tfthreadallegro_display_transfer (ALLEGRO_DISPLAY *disp, MTfMsgFbUp *mm)
{
  static ALLEGRO_BITMAP *bmp;

  if (bmp == NULL)
      bmp = al_create_bitmap (640, 480);

  {
      ALLEGRO_LOCKED_REGION *rgn;
      rgn = al_lock_bitmap (bmp,
                            ALLEGRO_PIXEL_FORMAT_BGR_888, ALLEGRO_LOCK_WRITEONLY);
      char *rdata;
      rdata = rgn->data;
      for (gint i = 0; i < 480; ++i, rdata += rgn->pitch)
        {
          memcpy (rdata, &mm->tex_data[i*640*3], 640*3);
        }
      al_unlock_bitmap (bmp);
  }

  al_set_target_backbuffer (disp);
  al_draw_bitmap (bmp, 0, 0, 0);
  al_flip_display ();
}
