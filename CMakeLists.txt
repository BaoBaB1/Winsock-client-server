cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

project("Winsock client-server")

set (WINDOWS_LIBS 
	Ws2_32)

add_executable(Client Client.cpp)
target_link_libraries(Client ${WINDOWS_LIBS})
add_executable(Server Server.cpp)
target_link_libraries(Server ${WINDOWS_LIBS})

