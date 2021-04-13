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


# Evaluation
The evaluation kit used is provided in the MOT Chanllenge website  
Download the kit from here: https://github.com/dendorferpatrick/MOTChallengeEvalKit  
The evaluation kit needs a python version of 3.6.9 or newer but under 3.9 and MATLAB (we ran with R2020b).  

To run the evaluation, first need to setup the matlab python engine. Go to the matlab directory.  
`cd .\extern\engines\python` (in windows)  
`python setup.py install`

Then install the requirement package in the evaluation kit. Go to the direcotry of **MOTChallengeEvalKit-master**, run `pip install -r .\MOT\requirements.txt`

Then everything should be ready to go. We used dataset MOT16 training set MOT16-09 and MOT16-11 as the ground truth. Go to the data directory of **MOTChallengeEvalKit-master**, 
create a folder called MOT16 then put the MOT16-09 and MOT16-11 folder downloaded from https://motchallenge.net/data/MOT16/ in it. Then change the directory to .\MOTChallengeEvalKit-master\seqmaps, go to MOT16-train.txt, edit the content until there are only MOT16-09 and MOT16-11 left under name.

Use the function in json_to_txt.py to convert the json output file from PHD filter to txt file in a format that can be read by the evaualtion kit. Named the two results txt file to be MOT16-09.txt and MOT16-11.txt and put them in the directory .\MOTChallengeEvalKit-master\res\MOT16res  

In the .\MOTChallengeEvalKit-master directory, run `python .\MOT\evalMOT.py` and should get the evaluation metrics of MOTA, MOTP, etc.
