cmake_minimum_required (VERSION 3.1)
project (luabind11)

option(BUILD_SHARED "Build as shared" ON)
option(BUILD_TEST "Build tests" OFF)

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

include_directories("${PROJECT_SOURCE_DIR}/")

file(GLOB SRC "src/*.cpp" "luabind/*.hpp")
file(GLOB TEST_SRC "test/*.cpp" "test/*.hpp")

if(BUILD_SHARED)
	add_definitions(-DLUABIND_DYNAMIC_LINK)
	add_library(luabind11 SHARED ${SRC})
else()
	add_library(luabind11 STATIC ${SRC})
endif()
target_link_libraries(luabind11 ${CONAN_LIBS})

if(BUILD_TEST)
	add_executable(Test ${TEST_SRC})
	target_link_libraries(Test luabind11)
endif()

install(TARGETS luabind11 RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY luabind/ DESTINATION include/luabind FILES_MATCHING PATTERN "*.hpp")
