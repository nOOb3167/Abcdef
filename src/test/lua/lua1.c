#include <stdlib.h>
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <assert.h>
#include <glib.h>

#include <src/test/lua/tf-act.h>

int
f1_c (lua_State *L)
{
  int ret;
  int ctx;

  printf ("Continuation\n");

  ret = lua_getctx (L, &ctx);
  assert (LUA_YIELD == ret);

  printf ("Ctx %d\n", ctx);
  
  return 0;
}

int
f1 (lua_State *L)
{
  printf ("DERP\n");
  return lua_yieldk (L, 0, 1234, f1_c);
}

int
main (int argc, char **argv)
{
  lua_State *L;
  lua_State *L2;
  int ret;

  g_type_init ();

  L = luaL_newstate ();
  L2 = lua_newthread (L);

  lua_pushcfunction (L2, f1);
  ret = lua_resume (L2, 0);

  if (LUA_YIELD != ret)
    {
      printf ("FAIL |%s|\n", lua_tostring (L2, -1));
      return EXIT_FAILURE;
    }

  ret = lua_resume (L2, 0);

  if (LUA_OK != ret)
    {
      printf ("FAIL |%s|\n", lua_tostring (L2, -1));
      return EXIT_FAILURE;
    }

  TfAct *ta;
  ta = TF_ACT (tf_act_new (L));

  tf_act_cr_cfunc (ta, f1);
  tf_act_cr_resume_argless (ta);
  tf_act_cr_resume_argless (ta);

  return EXIT_SUCCESS;
}
