#include "phd_filter.h"

#include "simulator/Ground_Truth.hpp"
#include "simulator/PositionSensor.hpp"

#include "utils/plotting_utils.hpp"



int main()
{
    std::cout << "Starting a simple PHD filter simulation..." << std::endl;
    phd_filter filter("simulation"); // Notice (priority medium): Should not be passed that argument, pass parameters instead

    // int t_steps_ = 3;
    int t_steps_ = 100;
    for (int t = 1; t < t_steps_; t++)
    {
        // Simulation portion ////
        auto detections = PositionSensor(t);
        auto g_ = GroundTruth(t);  //// NOTICE: not used. 

        // Filter potion //

        // filter.propose_new_born_targets();
        // filter.propose_spawned_targets();

        filter.propagate_states();

        filter.construct_phd_update_components();
        // filter.FAILING_sensor_update_for_object_missing_detections();

        filter.sensor_update(detections);
        filter.NormalizeWeights(); //TODO: move normalization into functions that update particles.

        filter.PruningAndMerging();
        filter.NormalizeWeights();  // TODO: Check if this is required. Likely is. 

        vector<Particle> particles = filter.extract_target_states();
        // vector<Particle> particles = filter.get_x_k_();
        plt::clf();
        plot_particles(particles);
        plot_detections(detections);
        plt::pause(0.0001);
    }

    std::cout << "Simulation done!" << std::endl;
    string s; std::cin >> s;  // Temporary, just to pause at last view

    return 0;
}