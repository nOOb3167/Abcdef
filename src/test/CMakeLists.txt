INCLUDE_DIRECTORIES(context_switch)
ADD_EXECUTABLE(csw
 context_switch/csw.c
 ${CMAKE_SOURCE_DIR}/src/gfx_lib_setup.c
 ${CMAKE_SOURCE_DIR}/src/error.c)
TARGET_LINK_LIBRARIES(csw ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)
SET_TARGET_PROPERTIES(csw PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

INCLUDE_DIRECTORIES(model_anim)
ADD_EXECUTABLE(ma
 model_anim/ma.c
 ${CMAKE_SOURCE_DIR}/src/gfx_lib_setup.c
 ${CMAKE_SOURCE_DIR}/src/error.c
 ${CMAKE_SOURCE_DIR}/src/ai_example.c
 ${CMAKE_SOURCE_DIR}/src/array_macro.c
 ${CMAKE_SOURCE_DIR}/src/misc.c
 ${CMAKE_BINARY_DIR}/src/mai-model.c
 ${CMAKE_BINARY_DIR}/src/mai-anim.c
 ${CMAKE_SOURCE_DIR}/src/mai-anim_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-node.c
 ${CMAKE_SOURCE_DIR}/src/mai-node_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-node-anim.c
 ${CMAKE_SOURCE_DIR}/src/mai-node-anim_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-anim-instance.c
 ${CMAKE_BINARY_DIR}/src/mai-bone.c)
TARGET_LINK_LIBRARIES(ma ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)
SET_TARGET_PROPERTIES(ma PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

INCLUDE_DIRECTORIES(model_anim)
ADD_EXECUTABLE(ms
 model_skin/ms.c
 ${CMAKE_SOURCE_DIR}/src/gfx_lib_setup.c
 ${CMAKE_SOURCE_DIR}/src/error.c
 ${CMAKE_SOURCE_DIR}/src/ai_example.c
 ${CMAKE_SOURCE_DIR}/src/array_macro.c
 ${CMAKE_SOURCE_DIR}/src/misc.c
 ${CMAKE_BINARY_DIR}/src/mai-model.c
 ${CMAKE_BINARY_DIR}/src/mai-anim.c
 ${CMAKE_SOURCE_DIR}/src/mai-anim_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-node.c
 ${CMAKE_SOURCE_DIR}/src/mai-node_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-node-anim.c
 ${CMAKE_SOURCE_DIR}/src/mai-node-anim_funcs.c
 ${CMAKE_BINARY_DIR}/src/mai-anim-instance.c
 ${CMAKE_BINARY_DIR}/src/mai-bone.c)
TARGET_LINK_LIBRARIES(ms ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)
SET_TARGET_PROPERTIES(ms PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
