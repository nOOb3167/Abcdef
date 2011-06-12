#Vararg, extra allegro components / modules.
MACRO(ALLEGRO_INCLUDE)

IF(NOT DEFINED ALLEGRO_ROOT_DIR OR NOT DEFINED ALLEGRO_DEBUG)
 IF(NOT DEFINED ALLEGRO_ROOT_DIR)
  SET(ALLEGRO_ROOT_DIR "XXXSETME-NOTFOUND" CACHE PATH "Rootdir")
 ENDIF()
 IF(NOT DEFINED ALLEGRO_DEBUG)
  SET(ALLEGRO_DEBUG FALSE CACHE BOOL "Debug")
 ENDIF()
 MESSAGE(FATAL_ERROR "ALLEGRO_ROOT_DIR or ALLEGRO_DEBUG not set, do that now")
ENDIF()

#Add SOURCE_DIR/cmake to search path (FindAllegro.cmake should be in there)
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake")
FIND_PACKAGE(Allegro REQUIRED COMPONENTS ${ARGV})

IF(NOT ALLEGRO_FOUND)
 MESSAGE(FATAL_ERROR "Allegro not found")
ENDIF()

ENDMACRO()
