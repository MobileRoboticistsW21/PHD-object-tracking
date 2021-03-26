# TODO LIST: 

lots needs to be done. Here is some direction that could prove helpful!

- ignore discarded_files/* and evaluate_object_3d_offline.cpp
- Best starting point is run_phd. The file is still a mess but has the calls to the PHD filter.
- Most of the functionality is in phd_filter.cpp, Pruning_and_merging, and Normalize_weights. In that order.

I left some todos in the code and attempted to give them priority scores (low, medium, high). 

Let us chat before people take on a specific piece of the code for the sake of dividing and conquering.

# PHD-object-traking
Experimentation on the use of a Probability Hypothesis Density Filter to track objects in image space.
