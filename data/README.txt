#Data README

This folder is intended for data that is to be consumed by the filter and visualization code.
Provided in the repo is a sample of code processed by our RCNN code and augmented with optical flow.

The python notebook provided is setup to read from this folder and write to it. 
The code will write files to data/output when phd_json_dump is run and when portions or the notebook are run.
Generated videos will be placed in a temporary folder inside data/output/.