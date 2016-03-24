project(ReverseTale-CommonServer)

file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)

set(CommonServer_SOURCES
    ${sources_Main}
)

include_directories(
	${CMAKE_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/dep/inih/cpp
    ${CMAKE_SOURCE_DIR}/dep/threadpool11/threadpool11/include

	${LIBMONGOCXX_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
)

find_package(Boost)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
else()
  include_directories("${CMAKE_SOURCE_DIR}/dep/threadpool11/ext/include")
endif()

if (NOT ${DEBUG})
	add_definitions(-DNDEBUG)
endif()

add_definitions(-DMONGOCXX_STATIC -DBSONCXX_STATIC)

if (MSVC)
	set(SOCKET_LIB ws2_32)
else()
	set(SOCKET_LIB "")
endif()

link_directories(
    ${LIBBSON_LIBRARY_DIRS}
    ${LIBMONGOC_LIBRARY_DIRS}
    ${LIBMONGOCXX_LIBRARY_DIRS}
    ${LIBBSONCXX_LIBRARY_DIRS}
)

add_library(ReverseTale-CommonServer STATIC ${CommonServer_SOURCES})
add_dependencies(ReverseTale-CommonServer mongo-cxx-driver)

target_link_libraries(ReverseTale-CommonServer
    ${SOCKET_LIB}
    ${LIBMONGOCXX_LIBRARIES}
    inih
    ReverseTale-Library)

set_property(TARGET ReverseTale-CommonServer PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-CommonServer PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-CommonServer PROPERTIES
    OUTPUT_NAME CommonServer
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin)
