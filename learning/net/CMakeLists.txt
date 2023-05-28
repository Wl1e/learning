include(CheckFunctionExists)

check_function_exists(accept4 HAVE_ACCEPT4)
if(NOT HAVE_ACCEPT4)
  set_source_files_properties(socketOps.cpp PROPERTIES COMPILE_FLAGS "-DNO_ACCEPT4")
endif()

set(net_SRCS
  acceptor.cpp
  buffer.cpp
  channel.cpp
  connector.cpp
  eventLoop.cpp
  eventLoopThread.cpp
  eventLoopThreadPool.cpp
  inetAddress.cpp
  poller.cpp
  poller/epollPoller.cpp
  poller/pollPoller.cpp
  poller/selectPoller.cpp
  socket.cpp
  socketOps.cpp
  tcpClient.cpp
  tcpConnection.cpp
  tcpServer.cpp
  timerQueue.cpp
  )

add_library(learning_net SHARED ${net_SRCS})
target_link_libraries(learning_net learning_base)

install(TARGETS learning_net DESTINATION lib)

# set(HEADERS
#   buffer.h
#   callbackF.h
#   channel.h
#   endian.h
#   eventLoop.h
#   eventLoopThread.h
#   eventLoopThreadPool.h
#   inetAddress.h
#   tcpClient.h
#   tcpConnection.h
#   tcpServer.h
#   timerId.h
#   )
file(GLOB NET_HEADERS "*.h")
install(FILES ${NET_HEADERS} DESTINATION include/learning/net)

add_subdirectory(http)