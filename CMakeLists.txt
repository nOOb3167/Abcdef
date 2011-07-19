CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Test C CXX)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/AlInc.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Unfuck.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/PkgInc.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Gob2Inc.cmake)

ALLEGRO_INCLUDE(image primitives)

PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0 cairo cairo-png)
PKG_CHECK_MODULES(COGL REQUIRED cogl-2.0)

UNFUCK(GTK2_CFLAGS_OTHER)
UNFUCK(GTK2_LDFLAGS_OTHER)

UNFUCK_SEP("${UNFUCKED_GTK2_CFLAGS_OTHER} ${COGL_CFLAGS}" CMAKE_C_FLAGS)
UNFUCK_SEP("${UNFUCKED_GTK2_LDFLAGS_OTHER} ${COGL_LDFLAGS}" CMAKE_LD_FLAGS)

SET(AI_INC_DIRS "C:\\tmp2\\assimp\\inst\\include\\assimp")
SET(AI_LNK_DIRS "C:\\tmp2\\assimp\\inst\\lib")

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/src
 ${CMAKE_BINARY_DIR}
 ${ALLEGRO_INCLUDE_DIRS}
 ${GTK2_INCLUDE_DIRS}
 ${COGL_INCLUDE_DIRS}
 ${AI_INC_DIRS}
 )

LINK_DIRECTORIES(
 ${GTK2_LIBRARY_DIRS}
 ${COGL_LIBRARY_DIRS}
 ${AI_LNK_DIRS}
 )

ADD_DEFINITIONS(-DX_ERROR_ASSERT_INSTEAD_OF_STACK_TRACE)
#ADD_DEFINITIONS(-DX_ERROR_STACK_TRACE_INSTEAD_OF_ASSERT)

#Warning see about source file properties only applying to added from same directory
FILE(GLOB XSRC_FILES "src/*.c")
SET_SOURCE_FILES_PROPERTIES(${XSRC_FILES}
 PROPERTIES COMPILE_FLAGS "-Wstrict-prototypes")

ADD_SUBDIRECTORY(src)

GOB2IZE(src/ab-cd.gob)
GOB2IZE(src/ab-cd-alt.gob)
GOB2IZE(src/mai-node.gob)
GOB2IZE(src/mai-anim.gob)
GOB2IZE(src/mai-node-anim.gob)

ADD_EXECUTABLE(MGob MGob.c src/ab-cd.c src/ab-cd-alt_funcs.c src/ab-cd-alt.c)
TARGET_LINK_LIBRARIES(MGob ${GTK2_LIBRARIES})

ADD_EXECUTABLE(Main Main.cpp)
TARGET_LINK_LIBRARIES(Main ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES})

ADD_EXECUTABLE(Nxt src/Nxt.c src/gfx_lib_setup.c src/error.c src/ai_example.c
 src/mai-node.c src/mai-node_funcs.c
 src/mai-anim.c src/mai-anim_funcs.c
 src/mai-node-anim.c src/mai-node-anim_funcs.c)
TARGET_LINK_LIBRARIES(Nxt ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)

INSTALL(TARGETS Nxt DESTINATION ".")
