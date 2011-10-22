CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(Test C CXX)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/AlInc.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Unfuck.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/PkgInc.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/Gob2Inc.cmake)
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/DoxygenInc.cmake)

ALLEGRO_INCLUDE(image primitives)

PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0 gthread-2.0 cairo cairo-png)
PKG_CHECK_MODULES(COGL REQUIRED cogl-2.0)
PKG_CHECK_MODULES(LUA52 REQUIRED lua52)

UNFUCK(GTK2_CFLAGS_OTHER)
UNFUCK(GTK2_LDFLAGS_OTHER)

UNFUCK_SEP("${UNFUCKED_GTK2_CFLAGS_OTHER} ${COGL_CFLAGS}" CMAKE_C_FLAGS)
UNFUCK_SEP("${UNFUCKED_GTK2_LDFLAGS_OTHER} ${COGL_LDFLAGS}" CMAKE_LD_FLAGS)

SET(AI_INC_DIRS "C:\\tmp2\\assimp\\inst_git\\include\\assimp")
SET(AI_LNK_DIRS "C:\\tmp2\\assimp\\inst_git\\lib")

INCLUDE_DIRECTORIES(
 ${CMAKE_SOURCE_DIR}
 ${CMAKE_SOURCE_DIR}/src
 ${CMAKE_BINARY_DIR}
 ${ALLEGRO_INCLUDE_DIRS}
 ${GTK2_INCLUDE_DIRS}
 ${COGL_INCLUDE_DIRS}
 ${AI_INC_DIRS}
 ${LUA52_INCLUDE_DIRS}
 )

LINK_DIRECTORIES(
 ${GTK2_LIBRARY_DIRS}
 ${COGL_LIBRARY_DIRS}
 ${AI_LNK_DIRS}
 ${LUA52_LIBRARY_DIRS}
 )

#-mthreads required on mingw for mtsafe exception handling, just saying

#ADD_DEFINITIONS(-DX_ERROR_ASSERT_INSTEAD_OF_STACK_TRACE)
ADD_DEFINITIONS(-DX_ERROR_STACK_TRACE_INSTEAD_OF_ASSERT)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

#Warning see about source file properties only applying to added from same directory
#FILE(GLOB XSRC_FILES "src/*.c")
#SET_SOURCE_FILES_PROPERTIES(${XSRC_FILES}
# PROPERTIES COMPILE_FLAGS "-Wstrict-prototypes")

ADD_SUBDIRECTORY(src/mgob)
ADD_SUBDIRECTORY(src/test)
ADD_SUBDIRECTORY(src/test/lua)
ADD_SUBDIRECTORY(src)

ADD_SUBDIRECTORY(doc)

GOB2IZE(src/ab-cd.gob)
GOB2IZE(src/ab-cd-alt.gob)

ADD_EXECUTABLE(MGob
 MGob.c
 src/error.c
 src/ab-cd.c src/ab-cd-alt_funcs.c src/ab-cd-alt.c)
TARGET_LINK_LIBRARIES(MGob ${GTK2_LIBRARIES})

ADD_EXECUTABLE(Main Main.cpp)
TARGET_LINK_LIBRARIES(Main ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES})

GOB2IZE(src/mai-model.gob)
GOB2IZE(src/mai-anim.gob)
GOB2IZE(src/mai-node.gob)
GOB2IZE(src/mai-node-anim.gob)
GOB2IZE(src/mai-anim-instance.gob)
GOB2IZE(src/mai-bone.gob)
GOB2IZE(src/mai-info-win.gob)

GOB2IZE(src/test/m-tfmsg.gob)
GOB2IZE(src/test/m-tfmsg-heartbeat.gob)
GOB2IZE(src/test/m-tfmsg-fbup.gob)
GOB2IZE(src/test/m-tfmsg-redraw.gob)
GOB2IZE(src/test/m-tfthread.gob)
GOB2IZE(src/test/m-tfthreadal.gob)
GOB2IZE(src/test/m-tfthreadallegro.gob)
GOB2IZE(src/test/m-tfthreadcogl.gob)
GOB2IZE(src/test/tf-gfx-threads.gob)

GOB2IZE(src/test/lua/tf-act.gob)

ADD_LIBRARY(nx_ai STATIC
 ${CMAKE_SOURCE_DIR}/src/gfx_lib_setup.c
 ${CMAKE_SOURCE_DIR}/src/error.c
 ${CMAKE_SOURCE_DIR}/src/ai_example.c
 ${CMAKE_SOURCE_DIR}/src/array_macro.c
 ${CMAKE_SOURCE_DIR}/src/misc.c
 ${CMAKE_BINARY_DIR}/src/mai-model.c
 ${CMAKE_SOURCE_DIR}/src/mai-model_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-anim.c
 ${CMAKE_SOURCE_DIR}/src/mai-anim_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-node.c
 ${CMAKE_SOURCE_DIR}/src/mai-node_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-node-anim.c
 ${CMAKE_SOURCE_DIR}/src/mai-node-anim_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-anim-instance.c
 ${CMAKE_SOURCE_DIR}/src/mai-anim-instance_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-bone.c
 ${CMAKE_SOURCE_DIR}/src/mai-bone_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-info-win.c
 ${CMAKE_SOURCE_DIR}/src/mai-info-win_funcs.c)

ADD_EXECUTABLE(Nxt src/Nxt.c)
TARGET_LINK_LIBRARIES(Nxt nx_ai ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)

INSTALL(TARGETS Nxt DESTINATION ".")

INCLUDE_DIRECTORIES(src/test/model_skin)
ADD_EXECUTABLE(sr
 src/test/model_skin/sr_main.c
 src/test/model_skin/sr.c
 src/test/model_skin/nx_mat.c)
TARGET_LINK_LIBRARIES(sr nx_ai ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)
SET_TARGET_PROPERTIES(sr PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

ADD_EXECUTABLE(sr_p
 src/test/model_skin/sr_p.c
 src/test/model_skin/sr.c
 src/test/model_skin/nx_mat.c)
TARGET_LINK_LIBRARIES(sr_p nx_ai ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)
SET_TARGET_PROPERTIES(sr_p PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

ADD_EXECUTABLE(sr_mt
 src/test/model_skin/sr_mt.c
 src/test/model_skin/sr.c
 src/test/model_skin/nx_mat.c
 src/test/m-tfmsg.c src/test/m-tfmsg_funcs.c
 src/test/m-tfmsg-heartbeat.c src/test/m-tfmsg-heartbeat_funcs.c
 src/test/m-tfmsg-fbup.c src/test/m-tfmsg-fbup_.c
 src/test/m-tfthread.c src/test/m-tfthread_.c
 src/test/m-tfthreadal.c src/test/m-tfthreadal_.c
 src/test/m-tfthreadallegro.c src/test/m-tfthreadallegro_.c
 src/test/m-tfthreadcogl.c src/test/m-tfthreadcogl_.c
 src/test/tf-gfx-threads.c src/test/tf-gfx-threads_.c)
TARGET_LINK_LIBRARIES(sr_mt nx_ai ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)
SET_TARGET_PROPERTIES(sr_mt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

ADD_EXECUTABLE(mt_yield EXCLUDE_FROM_ALL
 src/test/m-tfthread.c
 src/test/m-tfthreadal.c
 src/test/m-tfthreadcogl.c
 src/test/model_skin/mt_yield.c)
TARGET_LINK_LIBRARIES(mt_yield ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} opengl32)
SET_TARGET_PROPERTIES(mt_yield PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

ADD_EXECUTABLE(rec
 src/test/model_skin/rec.c)
TARGET_LINK_LIBRARIES(rec ${GTK2_LIBRARIES})
SET_TARGET_PROPERTIES(rec PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

ADD_EXECUTABLE(lua1
 src/test/lua/lua1.c
 src/test/lua/tf-act.c src/test/lua/tf-act_.c)
TARGET_LINK_LIBRARIES(lua1 nx_ai ${GTK2_LIBRARIES} ${LUA52_LIBRARIES})
SET_TARGET_PROPERTIES(lua1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
