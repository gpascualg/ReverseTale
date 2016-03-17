
include(ExternalProject)

set(DEP_BASE ${CMAKE_BINARY_DIR}/dep)
SET_PROPERTY(DIRECTORY PROPERTY EP_BASE ${DEP_BASE})

set(BSON_DIR ${DEP_BASE}/Install/libbson)
set(MONGOC_DIR ${DEP_BASE}/Install/mongo-c-driver)
set(MONGOCXX_DIR ${DEP_BASE}/Install/mongo-cxx-driver)

ExternalProject_Add(
    libbson
    GIT_REPOSITORY https://github.com/mongodb/libbson.git
    GIT_TAG r1.3
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    LOG_UPDATE ON
    UPDATE_COMMAND ""
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DENABLE_TESTS=OFF -DCMAKE_INSTALL_PREFIX:PATH=${BSON_DIR} -DINCLUDE_INSTALL_DIR=.
)

ExternalProject_Add_Step(
    libbson
    fix-libbson-alignment
    WORKING_DIRECTORY ${DEP_BASE}/Source/libbson
    COMMAND git stash && git apply --whitespace=fix ${CMAKE_SOURCE_DIR}/cmake/Patches/07-libbson.patch
    DEPENDEES download
)

ExternalProject_Add(
    mongo-c-driver
    GIT_REPOSITORY https://github.com/mongodb/mongo-c-driver.git
    GIT_TAG r1.3
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    LOG_UPDATE ON
    UPDATE_COMMAND ""
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBSON_ROOT_DIR=${BSON_DIR} -DBSON_INCLUDE_DIR=${BSON_DIR}/include -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX:PATH=${MONGOC_DIR} -DINCLUDE_INSTALL_DIR=.
    DEPENDS libbson
)

ExternalProject_Add(
    mongo-cxx-driver
    GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver.git
    #GIT_TAG r1.3
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
    LOG_UPDATE ON
    UPDATE_COMMAND ""
    CMAKE_ARGS -DBOOST_ROOT=${BOOST_ROOT}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DLIBBSON_DIR=${BSON_DIR}
    -DLIBMONGOC_DIR=${MONGOC_DIR} -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX:PATH=${MONGOCXX_DIR} -DINCLUDE_INSTALL_DIR=.
    DEPENDS mongo-c-driver
)

ExternalProject_Add_Step(
    mongo-cxx-driver
    fix-find-bson
    WORKING_DIRECTORY ${DEP_BASE}/Source/mongo-cxx-driver/cmake
    COMMAND git stash && git apply --whitespace=fix ${CMAKE_SOURCE_DIR}/cmake/Patches/06-mongocxx.patch
    DEPENDEES download
)

set(LIBBSON_LIBRARY_DIRS "${BSON_DIR}/lib" CACHE INTERNAL "")
set(LIBMONGOC_LIBRARY_DIRS "${MONGOC_DIR}/lib" CACHE INTERNAL "")

set(LIBMONGOCXX_INCLUDE_DIRS "${MONGOCXX_DIR}/include/mongocxx/v_noabi" CACHE INTERNAL "")
set(LIBMONGOCXX_LIBRARY_DIRS "${MONGOCXX_DIR}/lib" CACHE INTERNAL "")

set(LIBBSONCXX_INCLUDE_DIRS "${MONGOCXX_DIR}/include/bsoncxx/v_noabi" CACHE INTERNAL "")
set(LIBBSONCXX_LIBRARY_DIRS "${MONGOCXX_DIR}/lib" CACHE INTERNAL "")

find_library(LIBBSON_LIBRARY bson-static-1.0 ${LIBBSON_LIBRARY_DIRS})
mark_as_advanced(${LIBBSON_LIBRARY})

find_library(LIBMONGOC_LIBRARY mongoc-static-1.0 ${LIBMONGOC_LIBRARY_DIRS})
mark_as_advanced(${LIBMONGOC_LIBRARY})

find_library(LIBMONGOCXX_LIBRARY NAMES libmongocxx mongocxx.a PATHS ${LIBMONGOCXX_LIBRARY_DIRS})
mark_as_advanced(${LIBMONGOCXX_LIBRARY})

find_library(LIBBSONCXX_LIBRARY NAMES libbsoncxx bsoncxx.a PATHS ${LIBBSONCXX_LIBRARY_DIRS})
mark_as_advanced(${LIBBSONCXX_LIBRARY})

set (LIBMONGOCXX_LIBRARIES 
    ${LIBBSON_LIBRARY}
    ${LIBMONGOC_LIBRARY}
    ${LIBBSONCXX_LIBRARY}
    ${LIBMONGOCXX_LIBRARY}
)
set(LIBMONGOCXX_LIBRARIES ${LIBMONGOCXX_LIBRARIES} CACHE INTERNAL "")
