cmake_minimum_required(VERSION 2.8.0)

# ---[ Latte project
project(ReverseTale C CXX)

# -- [ Options
option(BUILD_HEADLESS	"Build headless bot"	ON)
option(BUILD_BOT		"Build bot"				ON)
option(BUILD_SERVER		"Build NosTale Server"	ON)
option(DEBUG			"Debug library and tools"	ON)

if(${BUILD_BOT})
	set(BUILD_XHACKING ON)
else()
	set(BUILD_XHACKING OFF)
endif()

# ---[ Includes
set(ReverseTale_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/library/include)
include_directories(${ReverseTale_INCLUDE_DIR} ${PROJECT_BINARY_DIR})

# ---[ Configure
include(CheckIncludeFiles)
check_include_files(experimental/filesystem HAVE_EXPERIMENTAL_FS)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
	${CMAKE_CURRENT_BINARY_DIR}/ReverseTale.h)

# ---[ Patches
execute_process(
	COMMAND git apply --whitespace=fix ${PROJECT_SOURCE_DIR}/cmake/Patches/01-inih-cmake.patch
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dep/inih
	OUTPUT_QUIET
	ERROR_QUIET
)
execute_process(
	COMMAND git apply --whitespace=fix ${PROJECT_SOURCE_DIR}/cmake/Patches/02-inih-has_key.patch
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dep/inih
	OUTPUT_QUIET
	ERROR_QUIET
)
execute_process(
	COMMAND git apply --whitespace=fix ${PROJECT_SOURCE_DIR}/cmake/Patches/03-libbson.patch
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dep/libbson
	OUTPUT_QUIET
	ERROR_QUIET
)
execute_process(
	COMMAND git apply --whitespace=fix ${PROJECT_SOURCE_DIR}/cmake/Patches/04-mongoc.patch
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dep/mongo-c-driver
	OUTPUT_QUIET
	ERROR_QUIET
)
execute_process(
	COMMAND git apply --whitespace=fix ${PROJECT_SOURCE_DIR}/cmake/Patches/05-mongocxx.patch
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dep/mongo-cxx-driver
	OUTPUT_QUIET
	ERROR_QUIET
)

set(ENABLE_SASL OFF)
set(ENABLE_TESTS OFF)
set(ENABLE_EXAMPLES OFF)

# ---[ Dependencies
if (${BUILD_XHACKING})
	add_subdirectory(dep/xHacking)
endif()
add_subdirectory(dep/inih)
add_subdirectory(dep/libbson)
add_subdirectory(dep/mongo-c-driver)
add_subdirectory(dep/mongo-cxx-driver)
add_subdirectory(dep/threadpool11)

# ---[ Subdirectories
add_subdirectory(library)
if (${BUILD_BOT})
	add_subdirectory(bot)
endif()
if (${BUILD_HEADLESS})
	add_subdirectory(headless)
endif()
if (${BUILD_SERVER})
	add_subdirectory(server)
endif()
