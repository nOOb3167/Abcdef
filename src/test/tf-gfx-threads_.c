#include <src/test/tf-gfx-threads.h>

NX_GET_NEW (tf_gfx_threads);

GObject *
_tf_gfx_threads_new (void)
{
  TfGfxThreads *self;
  GObject *par;

  self = TF_GFX_THREADS (nx_get_new ());
  par = G_OBJECT (self);

  self->threads = g_hash_table_new_full (g_int_hash, g_int_equal,
                                         g_free, g_free);

  return G_OBJECT (self);
}

TfGfxThreads *
tf_gfx_threads_get_instance (void)
{
  static TfGfxThreads *tgt = NULL;
  if (NULL == tgt)
      tgt = TF_GFX_THREADS (_tf_gfx_threads_new ());
  g_xassert (tgt);
  return tgt;
}

void
tf_gfx_threads_m_init (TfGfxThreadsM *tm, GThread *thread, MTfThread *data)
{
  tm->thread = thread;
  tm->data = data;
}

void
tf_gfx_threads_add (TfGfxThreads *self, enum TfThreadEnum id, GThread *thread, MTfThread *data)
{
  TfGfxThreadsM *tm;
  enum TfThreadEnum *key;

  tm = g_malloc0 (sizeof (*tm));
  tf_gfx_threads_m_init (tm, thread, data);

  /* FIXME: Warning, assumes sizeof TfThreadEnum == sizeof int or something */
  key = g_malloc0 (sizeof (*key));
  *key = id;

  g_hash_table_insert (self->threads, key, tm);
}

/**
 * The resulting TfGfxThreadsM is owned by TfGfxThreads / object lifetime.
 * Therefore no refcount or memory management necessary as far as I can tell,
 * may be a normal non-gobject struct.
 */
TfGfxThreadsM *
tf_gfx_threads_get (TfGfxThreads *self, enum TfThreadEnum id)
{
  gint key;
  TfGfxThreadsM *value;

  key = id;
  value = g_hash_table_lookup (self->threads, &key);
  g_xassert (value);

  return value;
}
