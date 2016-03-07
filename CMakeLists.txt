cmake_minimum_required (VERSION 3.1)
project (luabind11)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

include_directories("${PROJECT_SOURCE_DIR}/")

add_definitions(-DLUABIND_DYNAMIC_LINK)

file(GLOB SRC "src/*.cpp" "src/*.h")
file(GLOB TEST_SRC "test/*.cpp" "test/*.h")

set(LIBS lua5.1)

add_library(luabind11 SHARED ${SRC})
set_property(TARGET luabind11 PROPERTY CXX_STANDARD 14)
set_property(TARGET luabind11 PROPERTY CXX_STANDARD_REQUIRED ON)

# <AdditionalLibraryDirectories>$(BOOST_LIB_FOR_ADAPTER);%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>

add_executable(Test ${TEST_SRC})
target_link_libraries(Test ${LIBS} luabind11)
set_property(TARGET Test PROPERTY CXX_STANDARD 14)
set_property(TARGET Test PROPERTY CXX_STANDARD_REQUIRED ON)
