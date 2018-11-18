cmake_minimum_required (VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory(thirdparty/benchmark)

project(fast_try_emplace)
add_executable(fast_try_emplace try_emplace.cpp)
set(BENCHMARK_SOURCE_FILES fast_try_emplace.cpp flatmap.hpp)
target_link_libraries(fast_try_emplace benchmark)
target_compile_options(fast_try_emplace PUBLIC ${BENCHMARK_FLAGS})
