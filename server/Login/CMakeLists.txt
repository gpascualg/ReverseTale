project(ReverseTale-LoginServer)

file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)

set(LoginServer_SOURCES
    ${sources_Main}
)

add_definitions(-DMONGOCXX_STATIC -DBSONCXX_STATIC)
add_executable(ReverseTale-LoginServer ${LoginServer_SOURCES})

target_include_directories(ReverseTale-LoginServer
    PRIVATE cppformat
    PRIVATE ReverseTale-CommonServer
)

target_link_libraries(ReverseTale-LoginServer
    ReverseTale-CommonServer
)

set_property(TARGET ReverseTale-LoginServer PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-LoginServer PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-LoginServer PROPERTIES
    OUTPUT_NAME LoginServer
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin)
