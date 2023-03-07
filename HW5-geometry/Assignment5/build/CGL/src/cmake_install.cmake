# Install script for directory: /Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/build/CGL/src/libCGL.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCGL.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCGL.a")
    execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCGL.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/CGL" TYPE FILE FILES
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/CGL.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/vector2D.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/vector3D.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/vector4D.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/matrix3x3.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/matrix4x4.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/quaternion.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/complex.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/color.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/osdtext.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/viewer.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/base64.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/tinyxml2.h"
    "/Users/liuzhixuan/Documents/CSC4140/Homework/HW5/118010202_HW5/Assignment5/CGL/src/renderer.h"
    )
endif()

