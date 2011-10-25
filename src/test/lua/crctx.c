#include <stdlib.h>
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <assert.h>
#include <glib.h>
#include <glib-object.h>
#include <src/error.h>

int
main (int argc, char **argv)
{
  lua_State *L;
  int ret;

  g_type_init ();

  L = luaL_newstate ();
  luaL_openlibs (L);

  lua_State *L2;

  int derpgc (lua_State *L)
  {
    printf ("DERPGC\n");
    return 0;
  }

  {
    /**
     * Create a thread and assure no metatable is associated by default
     * as expected.
     */
    L2 = lua_newthread (L);
    g_xassert (0 == lua_getmetatable (L, -1));

    {
      /**
       * Set a metatable for the newly created thread.
       */
      lua_newtable (L);
      lua_pushstring (L, "__gc");
      lua_pushcfunction (L, derpgc);
      lua_rawset (L, -3);
      g_xassert (lua_isthread (L, -2));
      lua_setmetatable (L, -2);
    }

    {
      /**
       * Also test whether two separate threads really share the one metatable.
       */
      g_xassert (lua_isthread (L, -1));
      g_xassert (0 != lua_getmetatable (L, -1));

      lua_newthread (L);
      g_xassert (0 != lua_getmetatable (L, -1));

      g_xassert (lua_istable (L, -1) && lua_istable (L, -3));
      g_xassert (lua_compare (L, -1, -3, LUA_OPEQ));

      lua_pop (L, 1);
      lua_pop (L, 1);
    }

    lua_pop (L, 1);
  }

  lua_gc (L, LUA_GCCOLLECT, NULL);

  lua_newuserdata (L, 32);
  lua_newtable (L);
  lua_pushstring (L, "__gc");
  lua_pushcfunction (L, derpgc);
  lua_rawset (L, -3);
  g_xassert (lua_isuserdata (L, -2));
  lua_setmetatable (L, -2);
  lua_pop (L, 1);
  lua_gettop (L);

  lua_gc (L, LUA_GCCOLLECT, NULL);

  /**
   * FFFFFF
   * My head hurts apparently we are not deemed worthy of
   * __gc being supported on thread objects.
   *
   * I recommend using the C Registry luaL_ref mechanism to hold the cr.
   * And using weak pointers with checking inside all userdata.
   * When it is time to kill it, (For example when its TfAct is killed)
   * NULL the weak pointer.
   * ->Too complicated?
   * Because a reference is always held with luaL_ref, cr will not die
   * before TfAct, but if any references were still left somewhere,
   * it could die never/long after TfAct.
   * The weak pointer stuff would allow to check / error out on uses,
   * that extend past TfAct lifetime, (Shouldn't happen = bug)
   * but not much/nothing else.
   * (Map ctx lifetime to TfAct lifetime)
   */

  return EXIT_SUCCESS;
}
