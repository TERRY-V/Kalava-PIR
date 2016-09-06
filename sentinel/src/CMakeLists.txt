cmake_minimum_required(VERSION 2.8)
# set(CMAKE_VERBOSE_MAKEFILE ON)
set(appsrc serlaunch.cpp soapC.cpp soapServer.cpp stdsoap2.cpp
	../../common/qdir.cc
	../../common/qfile.cc
	../../common/qlogger.cc)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(serlaunch ${appsrc})
add_definitions("-D__multi_thread -D__USE_SET_TITLE")
target_link_libraries(serlaunch pthread)
