#http://www.cmake.org/cmake/help/syntax.html
#Section Variables
#  Says command("${Foo}") is same as command( "a b c" ) (Where Foo was a list a;b;c)
#  But as far as I can tell that a lie/outdated: Same as command(a b c) instead
#  So this macro: Convert a;b;c to "a b c"

FUNCTION(UNFUCK_SEP LISTVAL RESULTNAME)
    #Invocation example:
    #  #Quote to prevent list->multiargs split.
    #  UNFUCK_SEP("${GTK3_CFLAGS_OTHER}" UNFUCKED_GTK3_CFLAGS_OTHER)
    SET(UNFUCKED "")
    #Not ${LISTVAL], FOREACH LISTS argument is evaluated to get list already.
    FOREACH(FUCK_YOU IN LISTS LISTVAL)
        #Quote two expansions separated by space to conserve it.
        SET(UNFUCKED "${UNFUCKED} ${FUCK_YOU}")
    ENDFOREACH()
    SET(${RESULTNAME} ${UNFUCKED} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(UNFUCK VARNAME)
    #Invocation example:
    #  #Pass var name, no ${} / eval, quoting optional
    #  UNFUCK(GTK3_CFLAGS_OTHER)
    SET(UNFUCKED "")
    FOREACH(FUCK_YOU IN LISTS ${VARNAME})
        SET(UNFUCKED "${UNFUCKED} ${FUCK_YOU}")
    ENDFOREACH()
    SET("UNFUCKED_${VARNAME}" ${UNFUCKED} PARENT_SCOPE)
ENDFUNCTION()
