cmake_minimum_required(VERSION 2.8.0)

# ---[ Latte project
project(ReverseTale C CXX)

# ---[ Includes
set(ReverseTale_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/library/include)
include_directories(${ReverseTale_INCLUDE_DIR} ${PROJECT_BINARY_DIR})

# ---[ Dependencies
add_subdirectory(dep/xHacking)

# ---[ Subdirectories
add_subdirectory(library)
add_subdirectory(bot)
add_subdirectory(headless)
