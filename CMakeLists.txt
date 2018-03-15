cmake_minimum_required (VERSION 3.3)

project (pgui)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
cmake_policy(SET CMP0015 NEW)

set(PGUI_SRC_DIR "src")
set(PGUI_TEST_DIR "test")

list(APPEND COMMON_LIBS "") #create empty list

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	include_directories(/usr/local/include /usr/include)
	link_directories(/usr/local/lib /usr/lib64 /usr/lib)
	#find_library(GL_LIB GL HINTS /usr/local/lib )
	#find_library(GLFW_LIB glfw HINTS /usr/local/lib)
	#find_library(GLEW_LIB GLEW HINTS /usr/lib64)
	#set(LINK_LIBS ${GL_LIB} ${GLEW_LIB} ${GLFW_LIB})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	include_directories(/opt/local/include)
	link_directories(/opt/local/lib)

	#find libs
	find_package(OpenGL)
	find_package(GLEW)
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLFW REQUIRED glfw3)

	if(OPENGL_FOUND)
		list(APPEND COMMON_LIBS ${OPENGL_LIBRARIES})
	endif()
	if(GLEW_FOUND)
		list(APPEND COMMON_LIBS ${GLEW_LIBRARIES})
	endif()
	if(GLFW_FOUND)
		list(APPEND COMMON_LIBS ${GLFW_LIBRARIES})
	endif()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/${PGUI_TEST_DIR}")

## pgui_test begin
set(test_source
	${PGUI_TEST_DIR}/test_pgui_table.c
	${PGUI_SRC_DIR}/pgui_table.c
	${PGUI_SRC_DIR}/pgui_str.c
	)
add_executable("pgui_test" ${test_source})
target_compile_definitions("pgui_test" PRIVATE BIN_PATH="${CMAKE_CURRENT_BINARY_DIR}/bin/")
set_target_properties("pgui_test" PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/")

## pgui_test end

## editor is a special test
## editor begin
set(target_name "editor")
set(target_link_libs ${LINK_LIBS})
set(editor_source
	${PGUI_SRC_DIR}/editor/main.c
	)
add_executable(${target_name} ${editor_source})
target_include_directories(${target_name} PUBLIC "${PGUI_SRC_DIR}/editor")
set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin/")
target_link_libraries(${target_name} ${target_link_libs})
## editor end
