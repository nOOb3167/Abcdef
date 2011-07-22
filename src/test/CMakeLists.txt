ADD_EXECUTABLE(csw
 context_switch/csw.c
 ${CMAKE_SOURCE_DIR}/src/gfx_lib_setup.c
 ${CMAKE_SOURCE_DIR}/src/error.c)
TARGET_LINK_LIBRARIES(csw ${ALLEGRO_LIBRARIES} ${GTK2_LIBRARIES} ${COGL_LIBRARIES} opengl32 assimp)
