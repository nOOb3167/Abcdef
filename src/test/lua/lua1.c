#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>

int
main (int argc, char **argv)
{
  lua_State *L;

  L = luaL_newstate ();

  return EXIT_SUCCESS;
}
