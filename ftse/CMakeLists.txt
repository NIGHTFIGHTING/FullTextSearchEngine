cmake_minimum_required(VERSION 3.6)
project(ftse)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#include_directories("src/ftse/include")

set(SOURCE_FILES
    src/global.h
    src/buffer.h
    src/buffer.cpp
    src/ftse_env.h
    src/ftse_env.cpp
    src/encoding.h
    src/encoding.cpp
    src/token.h
    src/token.cpp
    src/database.h
    src/database.cpp
    src/postings.h
    src/postings.cpp
    src/utils.h
    src/utils.cpp
    src/main.cpp)
    

add_executable(ftse ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(ftse sqlite3)
TARGET_LINK_LIBRARIES(ftse expat)
TARGET_LINK_LIBRARIES(ftse m)
