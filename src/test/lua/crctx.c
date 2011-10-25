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

  return EXIT_SUCCESS;
}
