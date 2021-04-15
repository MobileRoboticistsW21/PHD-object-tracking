# Gaussian Mixture Probability Hypothesis Density Filter

This code base is admittedly a work-in-progress implementation of the probability hypothesis density filter. 

Based on the <a href="http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.148.7889&rep=rep1&type=pdf">GM-PHD proposed by Vo _et. al._</a>, the code includes an implementation for a toy problem as well as one for tracking bounding boxes. 

Although lacking unit-testing, the code sets up some abstraction, debuggning, and visualization tools to facilitate expantion and further development.

This repository is a sister repository to this <a href="https://github.com/MobileRoboticistsW21/Mask_RCNN_with_Optical_Flow"> Mask RCNN repo</a>. Using the Mask RCNN repo we augmented MOT data with optical flow and set up a PhdFilterBox class to track the 6 dimensional moving boxes.

We provided <a href="https://drive.google.com/drive/folders/1BZhLONDeXmLR0zSFQNEjoi1ak9FUYFmJ?usp=sharing">sample preprocessed MOT data</a> in the linked Google Drive folder.

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

## Evaluation
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


## Some possible improvements

1. Lack of unit testing: The filter is not vetted extensively enough to ensure no bugs but it does not fail to run through.

2. Better software practices:
    * Particles could be made into a class and have associated distance functions and comparators.
    * Some processing loops (e.g. major sensor update) could be, at least partly, substituted by linear algebra equations to allow for faster processing.
    * Some global variables could be removed from the global scope and housed in functions where they are used (e.g. birth and spawn variables). This would increase code clarity and make it less susceptible to bugs.
    * Template code based on state size. Reducing dinamicity of memory could increase speed, clarity of code use, and rid the code of the default_particle function required for the sensor update step.

3. Advanced birth and spawn models: those models are currently very rudimentary.

4. More stable use of data: the detector provided has vastly varying width and height which means that the center speed will oscilate as the box dimensions change. There might be better ways to track bounding boxes and than the selected representation.