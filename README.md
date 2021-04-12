# Installing dependencies 

This code is setup with a linux environment in mind. To install dependencies run:

./setup.sh

Afterwards, simply use CMakeLists to build the code: 
* mkdir build
* cd build
* cmake ..
* make < target >

Curretn targets are: 
* __phd_filter_simulation__: demo example with visualization 
* __phd_json_sim__: reads data from json file, executes the filter and visulizes.
* __phd_json_dump__: reads json data and dumps results to a json file for easy use is other environments.


# Running in colab.
 
TODO: add a colab notebook that sets things up.

Only the phd_json_dump target can be made in colab environemnt. 


### High level concerns
1. The system will fail with no tracks currently. Some edge cases need to be considered tested.

2. Implement birth portion of the algorithm

3. Implement spawn portion of the algorithm

# PHD-object-traking
Experimentation on the use of a Probability Hypothesis Density Filter to track objects in image space.


