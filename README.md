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

# TODO LIST: 
lots needs to be done. Here is some direction that could prove helpful!

* ignore discarded_files/* and evaluate_object_3d_offline.cpp
* Best starting point is run_phd. The file is still a mess but has the calls to the PHD filter.
* Most of the functionality is in phd_filter.cpp, Pruning_and_merging, and Normalize_weights. In that order.

I left some todos in the code and attempted to give them priority scores (low, medium, high). 

Let us chat before people take on a specific piece of the code for the sake of dividing and conquering.


### High level concerns
1. Investigate the disappearance of a visible track 
    
    * The issue seems to relate to sampling and weighting particles
        * when visualizing all particles it is evident that there are particles traking all the way. 
    
    * Improve visualizer to display weights and such 

2. Implement birth portion of the algorithm

3. Implement spawn portion of the algorithm

# PHD-object-traking
Experimentation on the use of a Probability Hypothesis Density Filter to track objects in image space.


