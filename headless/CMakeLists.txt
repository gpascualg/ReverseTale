project(ReverseTale-Headless)

file(GLOB_RECURSE sources_Headless      src/*.cpp     src/*.h)

set(Headless_SOURCES
    ${sources_Headless}
)

include_directories(${CMAKE_SOURCE_DIR}/dep/xHacking/include)
include_directories(${CMAKE_SOURCE_DIR}/dep/inih/cpp)

add_executable(ReverseTale-Headless ${Headless_SOURCES})
target_link_libraries(ReverseTale-Headless ws2_32 xHacking inih ReverseTale-Library)

set_property(TARGET ReverseTale-Headless PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-Headless PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-Headless PROPERTIES
    OUTPUT_NAME Headless
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
