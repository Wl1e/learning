set(base_SRCS
    currentThread.cpp
    fileUtil.cpp
    logFile.cpp
    logging.cpp
    logStream.cpp
    processInfo.cpp
    thread.cpp
    threadPool.cpp
)

add_library(learning_base SHARED ${base_SRCS})
target_link_libraries(learning_base pthread rt)

install(TARGETS learning_base DESTINATION lib)

file(GLOB BASE_HEADERS "*.h")
install(FILES ${BASE_HEADERS} DESTINATION include/learning/base)
