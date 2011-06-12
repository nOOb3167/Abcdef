# - Find allegro
#
#  In:
#  ALLEGRO_ROOT_DIR - Cache variable: Set to whatever CMAKE_INSTALL_PREFIX was when 'make install'-ing allegro.
#  ALLEGRO_DEBUG - Cache variable: Set to True for the debug libraries.
#
#  Out:
#  ALLEGRO_INCLUDE_DIRS - where to find allegro5/allegro.h, etc.
#  ALLEGRO_LIBRARIES - List of libraries to link with using allegro.
#  ALLEGRO_FOUND - True if allegro found.
#
#  Specifying components as FIND_PACKAGE(Allegro COMPONENTS image font audio)
#  will cause searching for those as well, will be appended to ALLEGRO_LIBRARIES.

#Using FIND_PACKAGE_HANDLE_STANDARD_ARGS below.
INCLUDE(FindPackageHandleStandardArgs)

#List modnames
SET(ALLEGRO_MODNAMES acodec audio color dialog font image main memfile physfs primitives ttf)

SET(ALLEGRO_DEBUG_SUFIX "")
SET(ALLEGRO_LIBRARIES "")
SET(ALLEGRO_INCLUDE_DIRS "")

#Mark some of the cache entries as advanced, these will be created if you use FIND_PACKAGE with COMPONENTS.
#Tried MARK_AS_ADVANCED(ALLEGRO_${name}_LIBRARY) (Commented out somewhere below) but that did not work.
#No idea, just bruteforce I guess.
FOREACH(ALLEGRO_MODNAME IN LISTS ALLEGRO_MODNAMES)
 IF(NOT DEFINED "ALLEGRO_${ALLEGRO_MODNAME}_LIBRARY")
  SET("ALLEGRO_${ALLEGRO_MODNAME}_LIBRARY" "XXX-NOTFOUND" CACHE PATH "Internal really")
  MARK_AS_ADVANCED("ALLEGRO_${ALLEGRO_MODNAME}_LIBRARY")
 ENDIF()
ENDFOREACH()
MARK_AS_ADVANCED(ALLEGRO_LIBRARY ALLEGRO_INCLUDE_DIR)

#Debugmode libraries have an extra "-debug" suffix
IF(ALLEGRO_DEBUG)
  SET(ALLEGRO_DEBUG_SUFIX "-debug")
ENDIF()

#These finds allegro's header and main library
FIND_PATH(ALLEGRO_INCLUDE_DIR allegro5/allegro.h
  HINTS "${ALLEGRO_ROOT_DIR}/include")
FIND_LIBRARY(ALLEGRO_LIBRARY "allegro${ALLEGRO_DEBUG_SUFIX}"
  HINTS "${ALLEGRO_ROOT_DIR}/lib")

#This function finds an addon library, to be invoked in the FOREACH below.
#Assume the naming convention is: For eg. acodec: allegro_acodec.
#If passed 'name' then after the invocation ALLEGRO_'name'_LIBRARY should be set if found.
#With ALLEGRO_DEBUG search naming convention is acodec: allegro_acodec-debug
FUNCTION(_ALLEGRO_FIND_COMPONENT_LIBRARY name)
  FIND_LIBRARY("ALLEGRO_${name}_LIBRARY" "allegro_${name}${ALLEGRO_DEBUG_SUFIX}"
  HINTS "${ALLEGRO_ROOT_DIR}/lib")
ENDFUNCTION()

#Start filling in ALLEGRO_LIBRARIES, starting with the main library
IF(ALLEGRO_LIBRARY)
  LIST(APPEND ALLEGRO_LIBRARIES ${ALLEGRO_LIBRARY})
ELSE()
 MESSAGE(FATAL_ERROR "FindAllegro: Allegro's core not found")
ENDIF()

#Fill ALLEGRO_LIBRARIES with the addon libraries
FOREACH(_COMPONENT ${Allegro_FIND_COMPONENTS})
  _ALLEGRO_FIND_COMPONENT_LIBRARY(${_COMPONENT})
  IF("ALLEGRO_${_COMPONENT}_LIBRARY")
    #No idea why this would not work the way i'd imagine.
	#MARK_AS_ADVANCED(FORCE ALLEGRO_${_COMPONENT}_LIBRARY)
    LIST(APPEND ALLEGRO_LIBRARIES "${ALLEGRO_${_COMPONENT}_LIBRARY}")
  ELSE()
	MESSAGE(FATAL_ERROR "FindAllegro: Requested component ${_COMPONENT} not found.")
  ENDIF()
ENDFOREACH()

#Herp
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALLEGRO DEFAULT_MSG
  ALLEGRO_INCLUDE_DIR ALLEGRO_LIBRARY)

#Fill ALLEGRO_INCLUDE_DIRS, there is only one anyway
IF (ALLEGRO_FOUND)
  SET(ALLEGRO_INCLUDE_DIRS ${ALLEGRO_INCLUDE_DIR})
ENDIF()
