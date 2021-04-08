cmake_minimum_required(VERSION 3.14)

set(VCPKG_FEATURE_FLAGS "registries")

project(beison-ptr CXX)

find_package(beison REQUIRED)
find_package(fmt REQUIRED)

add_executable(beison-ptr src/main.cxx include/json-pointer/json-pointer.hxx)

target_compile_definitions(beison-ptr PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_features(beison-ptr PUBLIC cxx_std_17)
target_include_directories(beison-ptr PUBLIC include)

target_link_libraries(beison-ptr PRIVATE beison::beison fmt::fmt)
