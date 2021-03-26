#include "phd_filter.h"
#include "matplotlibcpp.h"

#include "simulator/Ground_Truth.hpp"
#include "simulator/PositionSensor.hpp"


namespace plt = matplotlibcpp;

void plot_particles(const vector<Particle>& particles)
{
    // plt::clf();
    vector<int> xs, ys;
    for (const auto& p : particles)
    {
        xs.push_back(p.state[0]);
        ys.push_back(p.state[1]);
    }
    plt::plot(xs, ys, "b*");
    cout << particles.size() << endl;
    plt::pause(0.0001);
}



int main()
{
    phd_filter filter("simulation"); // Notice (priority medium): Should not be passed that argument, pass parameters instead

    int t_steps_ = 100;
    for (int t = 1; t < t_steps_; t++)
    {
        // Simulation portion ////
        auto z_ = PositionSensor(t);
        auto g_ = GroundTruth(t);  //// NOTICE: not used. 
        /////////////////////////

        // Filter potion
        filter.setup_for_next_iteration(); // Notice (priority medium): get rid of this function eventually

        filter.propose_spawned_targets();

        filter.propagate_states();

        filter.construct_phd_update_components();
        filter.FAILING_sensor_update_for_object_missing_detections();

        filter.sensor_update(z_);

        filter.NormalizeWeights();
        filter.PruningAndMerging();

        auto state = filter.extract_target_states();
        plot_particles(state);
    }

    return 0;
}