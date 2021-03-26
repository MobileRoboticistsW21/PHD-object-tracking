#pragma once 

#include "matplotlibcpp.h"
#include <armadillo>

namespace plt = matplotlibcpp;

using pair_xy_vectors_t = std::pair<std::vector<int>, std::vector<int>>;

void plot_particles(const vector<Particle>& particles)
{
    vector<int> xs, ys;
    static std::vector<pair_xy_vectors_t> trace;
    for (const auto& p : particles)
    {
        xs.push_back(p.state[0]);
        ys.push_back(p.state[1]);
    }
    trace.push_back(pair_xy_vectors_t(xs, ys));
    for (const auto& t : trace)
    {
        plt::plot(t.first, t.second, "b*");
    }
    // cout << particles.size() << endl;
}

void plot_detections(arma::mat detections)
{
    pair_xy_vectors_t xs_ys;

    for (int z_idx = 0; z_idx < detections.n_rows; z_idx++)
    {
        vec z = arma::vectorise(detections.row(z_idx));
        xs_ys.first.push_back(z[0]);
        xs_ys.second.push_back(z[1]);
    }

    plt::plot(xs_ys.first, xs_ys.second, "r.");
}
