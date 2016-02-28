cmake_minimum_required(VERSION 2.8.0)

# ---[ Latte project
project(ReverseTale C CXX)

# ---[ Includes
set(ReverseTale_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/library/include)
include_directories(${ReverseTale_INCLUDE_DIR} ${PROJECT_BINARY_DIR})

# ---[ Patches
execute_process(
	COMMAND git apply ${PROJECT_SOURCE_DIR}/cmake/Patches/01-inih-cmake.patch
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dep/inih
)

# ---[ Dependencies
add_subdirectory(dep/xHacking)
add_subdirectory(dep/inih)

# ---[ Subdirectories
add_subdirectory(library)
add_subdirectory(bot)
add_subdirectory(headless)
