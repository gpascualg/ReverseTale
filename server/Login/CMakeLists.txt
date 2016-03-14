project(ReverseTale-LoginServer)

file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)

set(LoginServer_SOURCES
    ${sources_Main}
)

include_directories(
	${CMAKE_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/../common/src
	${CMAKE_SOURCE_DIR}/dep/inih/cpp
    ${CMAKE_SOURCE_DIR}/dep/threadpool11/threadpool11/include
	${CMAKE_SOURCE_DIR}/dep/mongo-cxx-driver/src
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

if (MSVC)
	set(SOCKET_LIB ws2_32)
else()
	set(SOCKET_LIB "")
endif()

add_executable(ReverseTale-LoginServer ${LoginServer_SOURCES})
target_link_libraries(ReverseTale-LoginServer ${SOCKET_LIB} inih ReverseTale-Library ReverseTale-CommonServer)

set_property(TARGET ReverseTale-LoginServer PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-LoginServer PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-LoginServer PROPERTIES
    OUTPUT_NAME LoginServer
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin)
