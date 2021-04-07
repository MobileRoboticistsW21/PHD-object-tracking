project(cpp_phd)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# for visualization
find_package(PythonLibs 3 REQUIRED )
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

add_library(matplotlib_cpp INTERFACE)
target_include_directories(matplotlib_cpp
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/examples>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(matplotlib_cpp INTERFACE
  cxx_std_11
)
# TODO: Use `Development.Embed` component when requiring cmake >= 3.18
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
target_link_libraries(matplotlib_cpp INTERFACE
  Python3::Python
  #Python3::Module
)
#find_package(Python3 COMPONENTS NumPy)
#if(Python3_NumPy_FOUND)
#  target_link_libraries(matplotlib_cpp INTERFACE
#    Python3::NumPy
#  )
#else()
target_compile_definitions(matplotlib_cpp INTERFACE WITHOUT_NUMPY)
#endif()


# for Armadillo (linear algebra calculation)
find_package(LAPACK REQUIRED)
find_package(BLAS REQUIRED)
find_package(Armadillo REQUIRED)

# for data loading
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)


SET(PHD_FILTER_SRC phd_filter_base.cpp )


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
#    Python3::NumPy 
    ${PYTHON_LIBRARIES}
    matplotlib_cpp
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
#    Python3::NumPy 
    ${PYTHON_LIBRARIES}
    matplotlib_cpp
    )
target_link_libraries(phd_with_json_data 
    ${JSONCPP_LIBRARIES}
    )
