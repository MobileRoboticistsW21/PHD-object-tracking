#include "filter/phd_filter_2d.h"
#include "simulator/Ground_Truth.hpp"
#include "simulator/PositionSensor.hpp"
#include "utils/plotting_utils.hpp"

int main()
{
    std::cout << "Starting a simple PHD filter simulation..." << std::endl;
    PhdFilter2d filter; // Notice (priority medium): Should not be passed that argument, pass parameters instead

    // int t_steps_ = 3;
    int t_steps_ = 100;
    for (int t = 1; t < t_steps_; t++)
    {
        // Simulation portion ////
        auto detections = PositionSensor(t);
        auto g_ = GroundTruth(t);  //// NOTICE: not used. 

        filter.update(detections);
        
        /// Visualization
        vector<Particle> particles = filter.extract_target_states();
        // vector<Particle> particles = filter.get_x_k_();
        plt::clf();
        plt::xlim(-250, 1000);
        plt::ylim(-1000, 250);
        plot_particles(particles, 50);
        plot_detections(detections);
        plt::pause(0.0001);
    }

    std::cout << "Simulation done!" << std::endl;
    string s; std::cin >> s;  // Temporary, just to pause at last view

    return 0;
}