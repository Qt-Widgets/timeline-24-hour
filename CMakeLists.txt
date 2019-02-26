cmake_minimum_required(VERSION 3.5)

project(Timeline)
SET(PROGRAM_NAME "Timeline")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

SET(CMAKE_CONFIGURATION_TYPES Debug;Release)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4")
SET(CMAKE_CXX_STANDART 14)
SET(CMAKE_CXX_STANDART_REQUIRED ON)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin CACHE PATH "Runtime output dir")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Library output dir")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib CACHE PATH "Library output dir")

SET(CMAKE_PREFIX_PATH $ENV{QTDIR})
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
SET(CMKAE_INCLUDE_CURRENT_DIR ON)

#set(Qt5_DIR "" CACHE PATH "")
find_package(Qt5 COMPONENTS Widgets Core GUI REQUIRED)


add_executable(${PROJECT_NAME} WIN32 
	main.cpp
	mainwindow.h
	mainwindow.cpp
	timelinewidget.h
	timelinewidget.cpp
	timelinecontrol.h
	timelinecontrol.cpp
)

target_link_libraries(${PROJECT_NAME}
	Qt5::Widgets Qt5::Core Qt5::Gui)
	
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
		   COMMAND ${CMAKE_COMMAND} -E copy_if_different
		   $<TARGET_FILE:Qt5::Core>
		   $<TARGET_FILE:Qt5::Gui>
		   $<TARGET_FILE:Qt5::Widgets>
		   $<TARGET_FILE_DIR:${PROJECT_NAME}>)