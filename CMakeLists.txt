project(cpp_phd)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(PythonLibs 3 REQUIRED )
find_package(LAPACK REQUIRED)
find_package(BLAS REQUIRED)
find_package(Armadillo REQUIRED)

include_directories(${ARMADILLO_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

SET(TARGET_SRC phd_filter.cpp )

add_executable(phd_filter_simulation run_phd.cpp ${TARGET_SRC})

target_link_libraries(phd_filter_simulation  
    ${PYTHON_LIBRARIES} 
    ${LAPACK_LIBRARIES} 
    ${BLAS_LIBRARIES} 
    ${ARMADILLO_LIBRARIES}
)
