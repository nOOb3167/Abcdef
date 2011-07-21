GOB2IZE(nx-base.gob)
GOB2IZE(nx-derived.gob)

ADD_EXECUTABLE(mgob2 main.c
 ${CMAKE_CURRENT_BINARY_DIR}/nx-base.c
 ${CMAKE_CURRENT_BINARY_DIR}/nx-derived.c)
TARGET_LINK_LIBRARIES(mgob2 ${GTK2_LIBRARIES})
