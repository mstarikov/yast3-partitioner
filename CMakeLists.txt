cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(yast-partitioner)
add_executable(${PROJECT_NAME} "main.cpp")
#add_executable(${PROJECT_NAME} "manywidgets.cpp")


include_directories(/usr/include/yui)
target_link_libraries(${PROJECT_NAME} yui)
#set(LIBS ${LIBS} ${yui})
#target_include_directories(${CMAKE_SOURCE_DIR} /usr/include/yui)

#set(HEADER_FILES "YUI.h" "YWidgetFactory.h" "YDialog.h" "YLayoutBox.h" "YEvent.h")
