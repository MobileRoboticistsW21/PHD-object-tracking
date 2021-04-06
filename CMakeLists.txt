project(cpp_phd)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# for visualization
find_package(PythonLibs 3 REQUIRED )
find_package(Python3 COMPONENTS NumPy REQUIRED)

# for Armadillo (linear algebra calculation)
find_package(LAPACK REQUIRED)
find_package(BLAS REQUIRED)
find_package(Armadillo REQUIRED)

# for data loading
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)


SET(PHD_FILTER_SRC phd_filter.cpp )


# Simulator executable.
add_executable(phd_filter_simulation phd_sim.cpp ${PHD_FILTER_SRC})

include_directories(${ARMADILLO_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

target_link_libraries(phd_filter_simulation   
    ${LAPACK_LIBRARIES} 
    ${BLAS_LIBRARIES} 
    ${ARMADILLO_LIBRARIES}
    )
target_link_libraries(phd_filter_simulation 
    Python3::NumPy 
    ${PYTHON_LIBRARIES}
    )
# target_link_libraries(phd_filter_simulation 
#     ${JSONCPP_LIBRARIES}
#     )


# Data consumer executable.
add_executable(phd_with_json_data run_phd.cpp ${PHD_FILTER_SRC})

include_directories(${ARMADILLO_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

target_link_libraries(phd_with_json_data   
    ${LAPACK_LIBRARIES} 
    ${BLAS_LIBRARIES} 
    ${ARMADILLO_LIBRARIES}
    )
target_link_libraries(phd_with_json_data 
    Python3::NumPy 
    ${PYTHON_LIBRARIES}
    )
target_link_libraries(phd_with_json_data 
    ${JSONCPP_LIBRARIES}
    )