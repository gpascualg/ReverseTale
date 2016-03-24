# ---[ Create project
project(ReverseTale-CommonServer)

# ---[ Find all source and header files
file(GLOB_RECURSE sources_Main      src/*.cpp     src/*.h)
set(CommonServer_SOURCES
    ${sources_Main}
)

# ---[ Load dependencies
find_package(LibBSON)
find_package(LibMongoC)
find_package(libbsoncxx)
find_package(libmongocxx)
find_package(cppformat)

# ---[ Setup include directories
set(THIRD_PARTY_INCLUDE
    ${CMAKE_SOURCE_DIR}/server/common/src
    ${CMAKE_SOURCE_DIR}/dep/inih/cpp
    ${CMAKE_SOURCE_DIR}/dep/threadpool11/threadpool11/include

    ${LibBSON_INCLUDE_DIRS}
    ${LibMongoC_INCLUDE_DIRS}
    ${LIBBSONCXX_INCLUDE_DIRS}
    ${LIBMONGOCXX_INCLUDE_DIRS}
)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${THIRD_PARTY_INCLUDE}
)

# ---[ Setup some definitions
set(DEFINITIONS "MONGOCXX_STATIC=1 BSONCXX_STATIC=1")
#if (NOT ${DEBUG})
#   set(DEFINITIONS "${DEFINITIONS} NDEBUG=1")
#endif()

# ---[ Add library as static and add dependencies
add_library(ReverseTale-CommonServer STATIC ${CommonServer_SOURCES})
add_dependencies(ReverseTale-CommonServer EP_mongo-cxx-driver)


# ---[ Find absolute paths to libraries
find_library(LIBBSON_LIBRARY ${LIBBSON_LIBRARIES} PATHS ${LibBSON_LIBRARY_DIRS})
find_library(LIBMONGOC_LIBRARY ${LIBMONGOC_LIBRARIES} PATHS ${LibMongoC_LIBRARY_DIRS})
find_library(LIBBSONCXX_LIBRARY ${LIBBSONCXX_LIBRARIES} PATHS ${LIBBSONCXX_LIBRARY_DIRS})
find_library(LIBMONGOCXX_LIBRARY ${LIBMONGOCXX_LIBRARIES} PATHS ${LIBMONGOCXX_LIBRARY_DIRS})

# ---[ Link all third party libraries
target_link_libraries(ReverseTale-CommonServer
    #PUBLIC ${LIBBSON_LIBRARY} # Not needed, libbsoncxx already links it
    #PUBLIC ${LIBMONGOC_LIBRARY} # Not needed, libmongocxx already links it
    PUBLIC ${LIBBSONCXX_LIBRARY}
    PUBLIC ${LIBMONGOCXX_LIBRARY}
    PUBLIC inih
    PUBLIC threadpool11
    PUBLIC ReverseTale-Library
)

# ---[ Set C++11 standard
set_property(TARGET ReverseTale-CommonServer PROPERTY CXX_STANDARD 11)
target_compile_features(ReverseTale-CommonServer PRIVATE cxx_decltype cxx_nullptr cxx_variadic_templates)

# ---[ Setup output directory and include directories
separate_arguments(THIRD_PARTY_INCLUDE)
set_target_properties(ReverseTale-CommonServer PROPERTIES
    OUTPUT_NAME CommonServer
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../bin
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../bin

    INTERFACE_COMPILE_DEFINITIONS "${DEFINITIONS}"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRD_PARTY_INCLUDE}"
)
