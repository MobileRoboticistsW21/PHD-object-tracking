#pragma once 

#include "matplotlibcpp.h"
#include <armadillo>

namespace plt = matplotlibcpp;

using pair_xy_vectors_t = std::pair<std::vector<int>, std::vector<int>>;

string createRGBA(int r, int g, int b)
{   
    std::stringstream ss;
    ss << std::hex << ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8);
    string rgb_str = ss.str();
    while (rgb_str.length() < 6)   rgb_str = "0" + rgb_str;
    rgb_str = "#" + rgb_str;
    return rgb_str;
}

void plot_particles(const vector<Particle>& particles, int history_depth = 5, std::string color="b")
{
    std::map<std::string, std::string> cfg;
    cfg["color"] = color;
    cfg["marker"] = "*";
    vector<int> xs, ys;
    static std::vector<pair_xy_vectors_t> trace;
    for (const auto& p : particles)
    {
        xs.push_back(p.state[0]);
        ys.push_back(p.state[1]);
    }
    trace.push_back(pair_xy_vectors_t(xs, ys));
    // plt::scatter(xs, ys, 2, cfg);

    int count = 0;
    for (auto t = trace.end()-1; t > trace.begin(); t--)
    {
        plt::scatter(t->first, t->second, 2, cfg);
        if (++count >= history_depth) break;
    }
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
