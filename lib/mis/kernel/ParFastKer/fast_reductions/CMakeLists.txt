cmake_minimum_required(VERSION 2.8.7)
project(fast-reductions)

# Define for not using preprocessing for the vertex fold reduction
OPTION(PROFILING "Add detailed timing of the reductions. This will affect the overall runtime and blow up the output" OFF)

MESSAGE(STATUS "PROFILING? " ${PROFILING})
IF(PROFILING)
    ADD_DEFINITIONS(-DPROFILING)
ENDIF(PROFILING)

ADD_DEFINITIONS(-DUNSAFE_LONG)

ADD_DEFINITIONS(-Wreturn-type)

# Inlcude for some machines
include_directories(/usr/include/mpi)
 
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories for executable
include_directories(extern/KaHIP)
include_directories(extern/KaHIP/lib)
include_directories(extern/KaHIP/lib/tools)
include_directories(extern/KaHIP/lib/io)

include_directories(extern/KaHIP/interface)

#For debugging
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
# add_definitions("-pg")
# add_definitions("-g")

#enable c++11
add_definitions(-std=c++11)

set(OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS} -fopenmp")
add_definitions(${OpenMP_CXX_FLAGS})

# ADD_DEFINITIONS(-g2 -fno-inline -O2 -fno-omit-frame-pointer)

# Add the test executable
# set(TEST_SOURCES ${SOURCES})
# list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp")
# list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark_fine_grained.cpp")
# list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark.cpp")
# list(REMOVE_ITEM TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark_reduction_wise.cpp")
# add_executable(test_correctness ${TEST_SOURCES})

# Add the benchmark executable
# set(BENCHMARK_SOURCES ${SOURCES})
# list(REMOVE_ITEM BENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/test_correctness.cpp")
# list(REMOVE_ITEM BENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark_fine_grained.cpp")
# list(REMOVE_ITEM BENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark.cpp")
# list(REMOVE_ITEM BENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark_reduction_wise.cpp")
# add_executable(benchmark ${BENCHMARK_SOURCES})

# Add the sequential benchmark executable
# set(SEQUENTIALBENCHMARK_SOURCES ${SOURCES})
# list(REMOVE_ITEM SEQUENTIALBENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/test_correctness.cpp")
# list(REMOVE_ITEM SEQUENTIALBENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark_fine_grained.cpp")
# list(REMOVE_ITEM SEQUENTIALBENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp")
# list(REMOVE_ITEM SEQUENTIALBENCHMARK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark_reduction_wise.cpp")
# add_executable(sequential_benchmark ${SEQUENTIALBENCHMARK_SOURCES})

# Add the sequential benchmark with reduction wise kernelization executable
# set(SEQUENTIALBENCHMARKREDUCTIONWISE_SOURCES ${SOURCES})
# list(REMOVE_ITEM SEQUENTIALBENCHMARKREDUCTIONWISE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/test_correctness.cpp")
# list(REMOVE_ITEM SEQUENTIALBENCHMARKREDUCTIONWISE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark_fine_grained.cpp")
# list(REMOVE_ITEM SEQUENTIALBENCHMARKREDUCTIONWISE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp")
# list(REMOVE_ITEM SEQUENTIALBENCHMARKREDUCTIONWISE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark.cpp")
# add_executable(sequential_benchmark_reduction_wise ${SEQUENTIALBENCHMARKREDUCTIONWISE_SOURCES})

# Add the sequential benchmark with reduction wise kernelization executable
# set(BENCHMARK_FINE_GRAINED_SOURCES ${SOURCES})
# list(REMOVE_ITEM BENCHMARK_FINE_GRAINED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/test_correctness.cpp")
# list(REMOVE_ITEM BENCHMARK_FINE_GRAINED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark_reduction_wise.cpp")
# list(REMOVE_ITEM BENCHMARK_FINE_GRAINED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp")
# list(REMOVE_ITEM BENCHMARK_FINE_GRAINED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/sequential_benchmark.cpp")
# add_executable(benchmark_fine_grained ${BENCHMARK_FINE_GRAINED_SOURCES})

add_library(ParFastKer ${SOURCES})

# Link libraries
# target_link_libraries(test_correctness argtable2)
# target_link_libraries(test_correctness gomp)
target_link_libraries(ParFastKer argtable2)
target_link_libraries(ParFastKer gomp)
# target_link_libraries(benchmark_fine_grained argtable2)
# target_link_libraries(benchmark_fine_grained gomp)
# target_link_libraries(sequential_benchmark argtable2)
# target_link_libraries(sequential_benchmark gomp)
# target_link_libraries(sequential_benchmark_reduction_wise argtable2)
# target_link_libraries(sequential_benchmark_reduction_wise gomp)
