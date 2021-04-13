# Gaussian Mixture Probability Hypothesis Density Filter

This code base is admittedly a work-in-progress implementation of the probability hypothesis density filter. 

Based on the <a href="http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.148.7889&rep=rep1&type=pdf">GM-PHD proposed by Vo _et. al._</a>, the code includes an implementation for a toy problem as well as one for tracking bounding boxes. 

Although lacking unit-testing, the code sets up some abstraction, debuggning, and visualization tools to facilitate expantion and further development.

## Installing dependencies 

If using a linux machine running `./setup.sh` should install all needed software

After installation, run the following commands to build the code using CMakeLists: 
* `mkdir build`
* `cd build`
* `cmake ..`
* `make < target >`

Three build targets are currently supported: 
* __phd_filter_simulation__: demo example with visualization 
* __phd_json_sim__: reads data from json file, executes the filter and visulizes.
* __phd_json_dump__: reads json data and dumps results to a json file for easy use is other environments.


## Colab Notebook.
For this project we focused on using the MOT dataset and so the notebook is setup to visualize filter output over images as well as re-format data to be used in the evaluation code associated with MOT.


## Some possible imporovements

1. Lack of unit testing: The filter is not vetted extensively enough to ensure no bugs but it does not fail to run through.

2. Better software practices:
    * Particles could be made into a class and have associated distance functions and comparators.
    * Some processing loops (e.g. major sensor update) could be, at least partly, substituted by linear algebra equations to allow for faster processing.
    * Some global variables could be removed from the global scope and housed in functions where they are used (e.g. birth and spawn variables). This would increase code clarity and make it less susceptible to bugs.
    * Template code based on state size. Reducing dinamicity of memory could increase speed, clarity of code use, and rid the code of the default_particle function required for the sensor update step.


3. 

