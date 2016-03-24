project(ReverseTale-LoginServer)

file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)

set(LoginServer_SOURCES
    ${sources_Main}
)

find_package(Boost)
find_package(LibBSON)
find_package(LibMongoC)
find_package(libbsoncxx)
find_package(libmongocxx)
find_package(cppformat)

if(NOT Boost_FOUND)
    set(Boost_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/threadpool11/ext/include)
endif()

message(WARNING ${LIBMONGOCXX_INCLUDE_DIRS})

include_directories(
	${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/server/common/src
	${CMAKE_SOURCE_DIR}/dep/inih/cpp
    ${CMAKE_SOURCE_DIR}/dep/threadpool11/threadpool11/include

    ${Boost_INCLUDE_DIR}
    ${LibBSON_INCLUDE_DIRS}
    ${LibMongoC_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
    ${LIBMONGOCXX_INCLUDE_DIRS}
    ${cppformat_INCLUDE_DIRS}
)

link_directories(
    ${LibBSON_LIBRARY_DIRS}
    ${LibMongoC_LIBRARY_DIRS}
    ${LIBBSONCXX_LIBRARY_DIRS}
    ${LIBMONGOCXX_LIBRARY_DIRS}
)

if (NOT ${DEBUG})
	add_definitions(-DNDEBUG)
endif()

add_definitions(-DMONGOCXX_STATIC -DBSONCXX_STATIC)

if (MSVC)
	set(SOCKET_LIB ws2_32)
else()
	set(SOCKET_LIB "")
endif()

add_executable(ReverseTale-LoginServer ${LoginServer_SOURCES})
add_dependencies(ReverseTale-LoginServer EP_mongo-cxx-driver)
add_dependencies(ReverseTale-LoginServer EP_cppformat)

target_link_libraries(ReverseTale-LoginServer
    ${SOCKET_LIB}
    ${Boost_LIBRARY}
    ${LibBSON_LIBRARY}
    ${LibMongoC_LIBRARY}
    ${LIBBSONCXX_LIBRARIES}
    ${LIBMONGOCXX_LIBRARIES}
    ${cppformat_LIBRARY}
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
