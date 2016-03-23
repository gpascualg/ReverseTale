project(ReverseTale-GameServer)

file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)

set(GameServer_SOURCES
    ${sources_Main}
)

include_directories(
	${CMAKE_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/../common/src
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

add_executable(ReverseTale-GameServer ${GameServer_SOURCES})
add_dependencies(ReverseTale-GameServer mongo-cxx-driver)

target_link_libraries(ReverseTale-GameServer
    ${SOCKET_LIB}
    ${LIBMONGOCXX_LIBRARIES}
    inih
    threadpool11
    ReverseTale-Library
    ReverseTale-CommonServer)

set_property(TARGET ReverseTale-GameServer PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-GameServer PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-GameServer PROPERTIES
    OUTPUT_NAME GameServer
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin)
