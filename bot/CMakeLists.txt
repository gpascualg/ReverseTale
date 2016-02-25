project(ReverseTale-Bot)

file(GLOB_RECURSE sources_Bot       src/*.cpp     src/*.h)

set(Bot_SOURCES
    ${sources_Bot}
)

include_directories(${CMAKE_SOURCE_DIR}/dep/xHacking/include)

add_library(ReverseTale-Bot SHARED ${Bot_SOURCES})
target_link_libraries(ReverseTale-Bot ws2_32 xHacking ReverseTale-Library)

set_property(TARGET ReverseTale-Bot PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-Bot PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-Bot PROPERTIES
    OUTPUT_NAME Bot
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
