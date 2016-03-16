project(ReverseTale-LoginServer)

file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)

set(LoginServer_SOURCES
    ${sources_Main}
)

include_directories(
	${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/server/common/src
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

#add_definitions(-DBSONCXX_POLY_USE_BOOST)
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

add_executable(ReverseTale-LoginServer ${LoginServer_SOURCES})
add_dependencies(ReverseTale-LoginServer mongo-cxx-driver)

target_link_libraries(ReverseTale-LoginServer
    ${SOCKET_LIB}
    bson-static-1.0
    mongoc-static-1.0
    libmongocxx
    libbsoncxx
    inih
    threadpool11
    ReverseTale-Library
    ReverseTale-CommonServer
)

set_property(TARGET ReverseTale-LoginServer PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-LoginServer PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-LoginServer PROPERTIES
    OUTPUT_NAME LoginServer
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin)
