#https://www.glfw.org/docs/latest/build_guide.html#build_link_cmake_package

cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

project(IOSServer LANGUAGES CXX)

find_package(OpenAL REQUIRED)

add_executable(IOSServer
                main.cpp
                main.hpp
                util/networking/TCP.hpp
                util/networking/TCP.cpp
                util/networking/Listeners.cpp
                util/networking/Listeners.hpp
                util/networking/TCPListener.cpp
                util/networking/TCPListener.hpp
                util/networking/MusicSend.cpp
                util/networking/MusicSend.hpp

                util/networking/networkConfig.hpp

                util/documentor.cpp
                util/documentor.hpp
                util/getMusic.cpp
                util/getMusic.hpp

)


target_include_directories(IOSServer PRIVATE
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/util"
                          )

target_include_directories(IOSServer PUBLIC ${OpenAL_INCLUDE_DIR})
target_link_libraries(IOSServer ${OPENAL_LIBRARY})
