
#include <src/test/lua/tf-act.h>

NX_GET_NEW (tf_act);

GObject *
tf_act_new (lua_State *L)
{
  TfAct *self;
  GObject *par;

  self = TF_ACT (nx_get_new ());
  par = G_OBJECT (self);

  tf_act_init_d (self, L);

  return G_OBJECT (self);
}

void
tf_act_init_d (TfAct *self, lua_State *L)
{
    /**
     * From the Lua manual:
     * There is no explicit function to close or to destroy a thread.
     * Threads are subject to garbage collection, like any Lua object. 
     *
     * First it's like: "Yay, GC is great!"
     * But then how does it track references/use?
     * I believe I definitely need to keep a reference somewhere tracked
     * by lua, the stack (No) or some Table. (In C Registry?).
     * (Some subtable in the C Registry, use luaL_ref)
     */
    lua_State *thr;

    self->qu = g_queue_new ();
    self->L = L;

    {
      thr = lua_newthread (L);
      g_xassert (thr);

      tf_act_gcref_thread (self, thr);

      lua_pop (L, 1);
    }

    self->cr = thr;
}

void
tf_act_actcr_ensure (TfAct *self)
{
  _tf_act_creg_subtable_ensure_s (self, "ActCr");
}

void
tf_act_gcref_thread (TfAct *self, lua_State *thread)
{
  /**
   * Plan:
   * Ensure Registry key "ActCr" exists, and is a table.
   * Push said table on stack.
   * Push thread on stack.
   * (Warning: lua_pushthread pushes on that threads stack.
   *           So xmove.
   * Use luaL_ref.
   */
  int ref;

  tf_act_actcr_ensure (self);

  {
    lua_pushvalue (self->L, LUA_REGISTRYINDEX);
    {
      lua_pushthread (thread);
      lua_xmove (thread, self->L, 1);
    }
    ref = luaL_ref (self->L, -2);
    lua_pop (self->L, 1);
  }

  g_xassert (LUA_REFNIL != ref);

  {    
    lua_pushvalue (self->L, LUA_REGISTRYINDEX);
    
    {
      lua_pushthread (thread);
      lua_xmove (thread, self->L, 1);
    }
    lua_rawseti (self->L, -2, ref);
    lua_pop (self->L, 1);
  }
}

void
tf_act_cr_cfunc (TfAct *self, lua_CFunction mf)
{
  lua_pushcfunction (self->L, mf);
  lua_xmove (self->L, self->cr, 1);
}

void
tf_act_cr_lfunc_s (TfAct *self, const char *fstr)
{
  int err;

  err = luaL_loadstring (self->L, fstr);

  if (LUA_OK != err)
    {
      printf ("ERR: %s\n", lua_tostring (self->L, -1));
      g_xassert (FALSE);
    }

  lua_xmove (self->L, self->cr, 1);
}

void
tf_act_cr_resume_argless (TfAct *self)
{
  int st, err;

  /**
   * Since this is resume_argless, nothing should be on the cr thread stack.
   * Except on the first run, then the status is LUA_OK, and a function
   * (The coroutine's func) should be on the stack.
   * Finishing execution should get an error code, yielding a LUA_YIELD.
   */
  st = lua_status (self->cr);
  if (LUA_OK == st)
    g_xassert (1 == lua_gettop (self->cr));
  else
    g_xassert (0 == lua_gettop (self->cr));

  err = lua_resume (self->cr, 0);

  if (LUA_OK != err && LUA_YIELD != err)
    {
      printf ("ERR: %s\n", lua_tostring (self->L, -1));
      g_xassert (FALSE);
    }

  /**
   * Since the yield/return isn't supposed to return anything either,
   * just drop all return values.
   * Make a resume_argless_stdyield I guess.
   */
  lua_settop (self->cr, 0);
}

void
_tf_act_creg_subtable_ensure_s (TfAct *self, const char *key)
{
  lua_pushvalue (self->L, LUA_REGISTRYINDEX);
  lua_pushstring (self->L, key);
  lua_rawget (self->L, -2);

  if (lua_isnil (self->L, -1))
    {
      lua_pushvalue (self->L, LUA_REGISTRYINDEX);
      lua_pushstring (self->L, key);
      lua_newtable (self->L);
      lua_rawset (self->L, -3);

      lua_pop (self->L, 1);
    }

  lua_pop (self->L, 1);
}
