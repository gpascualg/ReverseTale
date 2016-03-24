project(ReverseTale-GameServer)

file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)

set(GameServer_SOURCES
    ${sources_Main}
)

add_executable(ReverseTale-GameServer ${GameServer_SOURCES})

target_include_directories(ReverseTale-GameServer
    PRIVATE cppformat
    PRIVATE ReverseTale-CommonServer
)

target_link_libraries(ReverseTale-GameServer
    ReverseTale-CommonServer
)

set_property(TARGET ReverseTale-GameServer PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-GameServer PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-GameServer PROPERTIES
    OUTPUT_NAME GameServer
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin)
