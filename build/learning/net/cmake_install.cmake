# Install script for directory: /home/wlle/files/codes/learning/learning/net

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblearning_net.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblearning_net.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblearning_net.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/wlle/files/codes/learning/build/lib/liblearning_net.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblearning_net.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblearning_net.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblearning_net.so"
         OLD_RPATH "/home/wlle/files/codes/learning/build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liblearning_net.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/learning/net" TYPE FILE FILES
    "/home/wlle/files/codes/learning/learning/net/acceptor.h"
    "/home/wlle/files/codes/learning/learning/net/buffer.h"
    "/home/wlle/files/codes/learning/learning/net/callback.h"
    "/home/wlle/files/codes/learning/learning/net/channel.h"
    "/home/wlle/files/codes/learning/learning/net/connector.h"
    "/home/wlle/files/codes/learning/learning/net/eventLoop.h"
    "/home/wlle/files/codes/learning/learning/net/eventLoopThread.h"
    "/home/wlle/files/codes/learning/learning/net/eventLoopThreadPool.h"
    "/home/wlle/files/codes/learning/learning/net/inetAddress.h"
    "/home/wlle/files/codes/learning/learning/net/poller.h"
    "/home/wlle/files/codes/learning/learning/net/socket.h"
    "/home/wlle/files/codes/learning/learning/net/socketOps.h"
    "/home/wlle/files/codes/learning/learning/net/tcpClient.h"
    "/home/wlle/files/codes/learning/learning/net/tcpConnection.h"
    "/home/wlle/files/codes/learning/learning/net/tcpServer.h"
    "/home/wlle/files/codes/learning/learning/net/timer.h"
    "/home/wlle/files/codes/learning/learning/net/timerHeap.h"
    "/home/wlle/files/codes/learning/learning/net/timerQueue.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/wlle/files/codes/learning/build/learning/net/http/cmake_install.cmake")
endif()

