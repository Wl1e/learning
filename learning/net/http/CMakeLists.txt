set(http_SRCS
    httpRequest.cpp
    httpResponse.cpp
    httpServer.cpp
)

add_library(learning_http SHARED ${http_SRCS})
target_link_libraries(learning_http learning_net)

install(TARGETS learning_http DESTINATION lib)
file(GLOB HTTP_HEADERS "*.h")
install(FILES ${HTTP_HEADERS} DESTINATION include/learning/net/http)