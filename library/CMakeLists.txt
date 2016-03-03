project(ReverseTale-Library)

file(GLOB_RECURSE sources_Login     src/Login/*.cpp     include/Login/*.h)
file(GLOB_RECURSE sources_Tools     src/Tools/*.cpp     include/Tools/*.h)
file(GLOB_RECURSE sources_Game		src/Game/*.cpp		include/Game/*.h)

set(Library_SOURCES
    ${sources_Login}
    ${sources_Tools}
	${sources_Game}
)

include_directories(${CMAKE_SOURCE_DIR}/dep/xHacking/include)

add_library(ReverseTale-Library STATIC ${Library_SOURCES})

# Do not target anything at this point, final products will
#target_link_libraries(ReverseTale-Library ws2_32 xHacking)

set_property(TARGET ReverseTale-Library PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-Library PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

set_target_properties(ReverseTale-Library PROPERTIES
    OUTPUT_NAME ReverseTale
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib)
