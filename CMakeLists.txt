cmake_minimum_required(VERSION 3.17)

project(ipk-projekt-1)

set(EXECUTABLE server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add some flags to compiler if build type is DEBUG
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(COMPILER_FLAGS "")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	list(APPEND COMPILER_FLAGS
		"-g"
	)
endif()

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
	message(STATUS "Using GNU C compiler")
	list(APPEND COMPILER_FLAGS
		"-g"
		"-pthread"
		"-Werror"
		"-Wall"
		"-Wextra"
		"-Wpedantic"
	)
	# set(CMAKE_C_FLAGS_INIT "-g -Werror -Wall -Wextra -Wpedantic")
	# set(CMAKE_CXX_FLAGS_INIT "-g -Werror -Wall -Wextra -Wpedantic")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
message(STATUS "Using GNU C++ compiler")
endif()

list(JOIN COMPILER_FLAGS " " COMPILER_FLAGS)
message(STATUS "C compiler build flags: " ${COMPILER_FLAGS})

# set(CMAKE_C_FLAGS ${COMPILER_FLAGS})
set(CMAKE_CXX_FLAGS ${COMPILER_FLAGS})


# Google test framework
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
# )
#
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)
#
# enable_testing()


# link_directories(include)

# file(GLOB_RECURSE TEST_SRC test/*.cpp)
# file(GLOB_RECURSE MAIN_SRC src/*.cpp)
set(MAIN_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/http.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/pool.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/socket.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp
)
# file(GLOB_RECURSE MAIN_HEADERS include/*.hpp)

set(MAIN_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/http.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/pool.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/buffer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/logger.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/socket.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/server.hpp
)

message(STATUS ${MAIN_HEADERS})

# list(REMOVE_ITEM MAIN_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# file(GLOB_RECURSE MAIN_HEADERS include/*.c)

# add_library(${EXECUTABLE}_lib SHARED ${MAIN_SRC} ${MAIN_HEADERS})
# target_include_directories(${EXECUTABLE}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_include_directories(ifj21_lib PUBLIC tests/mock)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(${EXECUTABLE} ${MAIN_SRC} ${MAIN_HEADERS})
# target_include_directories(${EXECUTABLE} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(${EXECUTABLE} ${EXECUTABLE}_lib)

# add_executable(
# 	${EXECUTABLE}_test
# 	${TEST_SRC}
# 	${MAIN_HEADERS}
# )

# target_link_directories(ifj21_test PUBLIC include)
# target_include_directories(ifj21_test PUBLIC include)
# include_directories(${CMAKE_CURENT_COURCE_DIR}/include)
# target_link_libraries(ifj21_test ifj21_lib)
# target_link_libraries(ifj21_test gtest_main)
# target_link_libraries(${EXECUTABLE}_test ${EXECUTABLE}_lib gtest_main gmock_main)
# GTest::gtest GTest::gmock GTest::gmock_main

# include(GoogleTest)
# gtest_discover_tests(${EXECUTABLE}_test)
