project(cpp_phd)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# find_package (Python3 COMPONENTS Interpreter Development)

# find_package(lapack )
# find_package( PythonInterp 3 REQUIRED )
find_package(PythonLibs 3 REQUIRED )
find_package(LAPACK REQUIRED)
find_package(BLAS REQUIRED)
find_package(Armadillo REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/matplotlib-cpp-master)
include_directories(${ARMADILLO_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

# Local source files here
SET(TARGET_SRC
    Birth_Weight.cpp
    Spawn_Motion_Model.cpp
    Spawn_Weights.cpp
    Existing_Object_Motion_Model.cpp
    PositionSensor.cpp
    Ground_Truth.cpp
    Update_PHD_components.cpp
    Missed_detections.cpp
    Detected_Object.cpp
    Normalize_weights.cpp
    Pruning_and_Merging.cpp
    MultiTargetStateExtraction.cpp
)


add_executable(output run_phd.cpp ${TARGET_SRC})

target_link_libraries(output ${PYTHON_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} ${ARMADILLO_LIBRARIES})
# -std=c++11 -I/usr/include/python3.8 -lpython3.8 -o output -O2 -llapack -lblas -larmadillo
