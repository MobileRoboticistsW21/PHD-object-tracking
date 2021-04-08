project(cpp_phd)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# for visualization
find_package(PythonLibs 3 REQUIRED )
# find_package(Python3 COMPONENTS NumPy REQUIRED)

# for Armadillo (linear algebra calculation)
find_package(LAPACK REQUIRED)
find_package(BLAS REQUIRED)
find_package(Armadillo REQUIRED)

# for data loading
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)


SET(PHD_FILTER_SRC filter/phd_filter_base.cpp)

# Simulator executable.
add_executable(phd_filter_simulation phd_basic_sim.cpp ${PHD_FILTER_SRC})

include_directories(${ARMADILLO_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

target_link_libraries(phd_filter_simulation   
    ${LAPACK_LIBRARIES} 
    ${BLAS_LIBRARIES} 
    ${ARMADILLO_LIBRARIES}
    )
target_link_libraries(phd_filter_simulation 
    ${PYTHON_LIBRARIES}
    )

# Data consumer executable.
add_executable(phd_json_sim phd_json_sim.cpp ${PHD_FILTER_SRC})

include_directories(${ARMADILLO_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

target_link_libraries(phd_json_sim   
    ${LAPACK_LIBRARIES} 
    ${BLAS_LIBRARIES} 
    ${ARMADILLO_LIBRARIES}
    )
target_link_libraries(phd_json_sim 
    ${PYTHON_LIBRARIES}
    )
target_link_libraries(phd_json_sim 
    ${JSONCPP_LIBRARIES}
    )


# Data consumer executable.
add_executable(phd_json_dump phd_json_dump.cpp ${PHD_FILTER_SRC})

include_directories(${ARMADILLO_INCLUDE_DIRS})

target_link_libraries(phd_json_dump   
    ${LAPACK_LIBRARIES} 
    ${BLAS_LIBRARIES} 
    ${ARMADILLO_LIBRARIES}
    )

target_link_libraries(phd_json_dump 
    ${JSONCPP_LIBRARIES}
    )