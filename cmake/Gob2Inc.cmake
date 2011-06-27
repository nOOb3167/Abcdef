IF(EXISTS "${CMAKE_BINARY_DIR}/PathsCustomInclude.cmake")
 INCLUDE("${CMAKE_BINARY_DIR}/PathsCustomInclude.cmake")
ENDIF()

#Gob2 related

IF(NOT DEFINED GOB2_EXECUTABLE)
 IF(DEFINED PCI_GOB2_EXECUTABLE)
  SET(GOB2_EXECUTABLE "${PCI_GOB2_EXECUTABLE}" CACHE PATH "Gob2 executable")
 ELSE()
  SET(GOB2_EXECUTABLE "XXXSETME-NOTFOUND" CACHE PATH "Gob2 executable")
  MESSAGE(FATAL_ERROR "PLEASE SET OR DEFAULT GOB2_EXECUTABLE")
 ENDIF()
ENDIF()

#GOBFILE relative to CMAKE_CURRENT_SOURCE_DIR
FUNCTION(GOB2IZE GOBFILE)
 #Example GOB2IZE(src/ab-cd.gob) from CMAKE_SOURCE_DIR
 #ABSOLUTE CMAKE_SOURCE_DIR/src/ab-cd.gob
 #PATH src
 #NAME_WE ab-cd
 #ADD_EXECUTABLE(BLAH, src/ab-cd.c)
 GET_FILENAME_COMPONENT(GOBABS "${GOBFILE}" ABSOLUTE)
 GET_FILENAME_COMPONENT(GOBPATH "${GOBFILE}" PATH)
 GET_FILENAME_COMPONENT(GOBNAME "${GOBFILE}" NAME_WE)
 IF(NOT IS_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${GOBPATH}")
  MESSAGE(FATAL_ERROR "GOB2IZE: CURRENT_BINARY_DIR/GOBPATH not a directory.\
  Assure CMakeLists.txt in gob src dir.\
  And ADD_SUBDIRECTORY called.")
 ENDIF()
 #"Relative path in OUTPUT interpreted relative to the build dir corresponding to the current source dir"
 ADD_CUSTOM_COMMAND(OUTPUT "${GOBPATH}/${GOBNAME}.c"
  COMMAND "${GOB2_EXECUTABLE}" "--no-touch" "${GOBABS}"
  IMPLICIT_DEPENDS
  DEPENDS "${GOBFILE}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${GOBPATH}"
  COMMENT "GOB2 '${GOBFILE}'"
  VERBATIM)
ENDFUNCTION()
