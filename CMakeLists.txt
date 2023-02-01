set(CMAKE_C_FLAGS -pthread)

add_executable(bridge bridge.c)
add_executable(multicastClient multicastClient.c)
add_executable(unicastClient unicastClient.c)
