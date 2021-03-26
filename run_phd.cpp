#include "phd_filter.h"

#include "simulator/Ground_Truth.hpp"
#include "simulator/PositionSensor.hpp"

#include "utils/plotting_utils.hpp"



int main()
{
    std::cout << "Starting a simple PHD filter simulation..." << std::endl;
    phd_filter filter("simulation"); // Notice (priority medium): Should not be passed that argument, pass parameters instead

    int t_steps_ = 100;
    for (int t = 1; t < t_steps_; t++)
    {
        // Simulation portion ////
        auto detections = PositionSensor(t);
        auto g_ = GroundTruth(t);  //// NOTICE: not used. 
        /////////////////////////

        // Filter potion
        filter.propose_spawned_targets();

        filter.propagate_states();

        filter.construct_phd_update_components();
        filter.FAILING_sensor_update_for_object_missing_detections();

        filter.sensor_update(detections);

        filter.NormalizeWeights();
        filter.PruningAndMerging();

        vector<Particle> extracted_targets = filter.extract_target_states();

        plt::clf();
        plot_particles(extracted_targets);
        plot_detections(detections);
        plt::pause(0.0001);

    }

    std::cout << "Simulation done!" << std::endl;
    return 0;
}