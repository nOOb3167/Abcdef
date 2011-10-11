#include <src/test/tf-gfx-threads.h>

NX_GET_NEW (tf_gfx_threads);


static gpointer
tf_init_allegro_timer (gpointer data);

static gpointer
tf_init_cogl (gpointer data);

static gpointer
tf_init_allegro (gpointer data);


gpointer
tf_init_allegro_timer (gpointer data)
{
  MTfThreadAl *mtta;
  mtta = M_TFTHREADAL (data);

  m_tfthreadal_start_timer (mtta);
  m_tfthreadal_event_loop_enter (mtta);

  return NULL;
}

gpointer
tf_init_cogl (gpointer data)
{
  MTfThreadCogl *mtta;
  mtta = M_TFTHREADCOGL (data);

  m_tfthreadcogl_init_inthread (mtta);
  m_tfthreadcogl_event_loop_enter (mtta);

  return NULL;
}

gpointer
tf_init_allegro (gpointer data)
{
  MTfThreadAllegro *mtta;
  mtta = M_TFTHREADALLEGRO (data);

  m_tfthreadallegro_init_inthread (mtta);
  m_tfthreadallegro_event_loop_enter (mtta);

  return NULL;
}

GObject *
_tf_gfx_threads_new (void)
{
  TfGfxThreads *self;
  GObject *par;

  self = TF_GFX_THREADS (nx_get_new ());
  par = G_OBJECT (self);

  self->mmutex = g_mutex_new ();

  self->threads = g_hash_table_new_full (g_int_hash, g_int_equal,
                                         g_free, g_free);

  _tf_gfx_threads_add_default (self);

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
tf_gfx_threads_m_init (TfGfxThreadsM *tm,
                       gpointer (*func)(gpointer arg1),
                       MTfThread *data)
{
  tm->started = FALSE;
  tm->func = func;
  tm->thread = NULL;
  tm->data = data;
}

void
_tf_gfx_threads_add_default (TfGfxThreads *self)
{
  MTfThreadAl *mtta;
  MTfThreadCogl *mttc;
  MTfThreadAllegro *mttallegro;

  mtta = M_TFTHREADAL (m_tfthreadal_new ());
  mttc = M_TFTHREADCOGL (m_tfthreadcogl_new ());
  mttallegro = M_TFTHREADALLEGRO (m_tfthreadallegro_new ());

  void nothing (MTfThreadCogl *c, gpointer d) {printf ("HELLO\n");}
  m_tfthreadcogl_connect__sig_heartbeat (mttc, nothing, NULL);
  m_tfthreadcogl_sig_heartbeat (mttc);

  tf_gfx_threads_add (self,
                      TF_THREAD_ALLEGRO_TIMER,
                      tf_init_allegro_timer,
                      M_TFTHREAD (mtta));
  tf_gfx_threads_add (self,
                      TF_THREAD_COGL,
                      tf_init_cogl,
                      M_TFTHREAD (mttc));
  tf_gfx_threads_add (self,
                      TF_THREAD_ALLEGRO,
                      tf_init_allegro,
                      M_TFTHREAD (mttallegro));  
}

void
tf_gfx_threads_add (TfGfxThreads *self, enum TfThreadEnum id,
    gpointer (*func1)(gpointer arg1), MTfThread *data)
{
  TfGfxThreadsM *tm;
  gint *key;

  g_mutex_lock (self->mmutex);
  {

    tm = g_malloc0 (sizeof (*tm));
    tf_gfx_threads_m_init (tm, func1, data);

    /* FIXME: Warning, assumes sizeof TfThreadEnum == sizeof int or something */
    key = g_malloc0 (sizeof (*key));
    *key = id;

    g_hash_table_insert (self->threads, key, tm);
  }
  g_mutex_unlock (self->mmutex);
}

/**
 * Start all currently added by creating the actual thread
 * using g_thread_create, then mark started.
 */
void
tf_gfx_threads_start (TfGfxThreads *self)
{
  void
  starter (gpointer key, gpointer value, gpointer data)
  {
    TfGfxThreadsM *value_;
    value_ = value;

    if (TRUE == value_->started)
        return;

    GThread *thread;
    thread = g_thread_create (value_->func, value_->data,
                              TRUE, NULL);
  }

  void
  truerizer (gpointer key, gpointer value, gpointer data)
  {
    TfGfxThreadsM *value_;
    value_ = value;

    if (TRUE == value_->started)
        return;

    value_->started = TRUE;
  }

  g_mutex_lock (self->mmutex);
  {
    g_hash_table_foreach (self->threads, starter, NULL);
    g_hash_table_foreach (self->threads, truerizer, NULL);
  }
  g_mutex_unlock (self->mmutex);
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

  /**
   * But I forgot why.
   * I believe getting a non-started thread there is no GThread yet so
   * a get is impossible or would have to be implemented to fail if called
   * on an unstarted thread.
   * I need / want / race condition / whatever, it is useful to have this function
   * succeed and provide access to the MTfThread member of TfGfxThreadsM,
   * so that functionality got split to tf_gfx_threads_get_data.
   */
  g_xassert (("WARNING DO NOT USE", 0));

  g_mutex_lock (self->mmutex);
  {
    key = id;
    value = g_hash_table_lookup (self->threads, &key);
    g_xassert (value);
  }
  g_mutex_unlock (self->mmutex);

  return value;
}

/**
 * Unreffing the result is caller's responsibility.
 */
MTfThread *
tf_gfx_threads_get_data (TfGfxThreads *self, enum TfThreadEnum id)
{
  gint key;
  TfGfxThreadsM *value;

  g_mutex_lock (self->mmutex);
  {
    key = id;
    value = g_hash_table_lookup (self->threads, &key);
    g_xassert (value);
    g_object_ref (value->data);
  }
  g_mutex_unlock (self->mmutex);

  return value->data;
}
