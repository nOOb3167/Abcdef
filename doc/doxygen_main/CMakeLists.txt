ADD_CUSTOM_TARGET(doxygen1
	COMMAND "${DOXYGEN_EXECUTABLE}"
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
	COMMENT "Running doxygen for Abcdef.")

ADD_CUSTOM_TARGET(doxygen
	COMMENT "Running doxygen to generate documentation.")

ADD_DEPENDENCIES(doxygen doxygen1)
