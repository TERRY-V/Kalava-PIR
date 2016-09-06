cmake_minimum_required(VERSION 2.8)
# set(CMAKE_VERBOSE_MAKEFILE ON)
set(appsrc recgservice.cpp pir.cpp soapC.cpp soapClient.cpp soapServer.cpp stdsoap2.cpp
	../../common/qdir.cc
	../../common/qfile.cc
	../../common/qlogger.cc)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(recgservice ${appsrc})
# add_definitions("-g -DDEBUG -D__multi_thread -D__VERBOSE_MODE -D__USE_SET_TITLE")
add_definitions("-O3 -D__multi_thread -D__VERBOSE_MODE -D__USE_SET_TITLE")
target_link_libraries(recgservice pthread)
