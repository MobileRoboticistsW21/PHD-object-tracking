#include <iostream>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>  // could be wrong 
#include <fstream>
#include "phd_filter_box.h"
#include "utils/plotting_utils.hpp"

arma::mat jason_to_arma_mat(const Json::Value& vals)
{
    int data_count = vals.size();
    int data_shape = vals[0].size();
    arma::mat m(data_count, data_shape, arma::fill::zeros);
    for (int i = 0; i < data_count; i++)
    {
        for (int j = 0; j < data_shape; j++)
        {
            m(i, j) = vals[i][j].asDouble();
        }
    }
    return m;
}

arma::mat get_detection_matrix(const Json::Value& vals)
{
        arma::mat bb_mat = jason_to_arma_mat(vals["bb"]);
        arma::mat flows_mat = jason_to_arma_mat(vals["flows"]);
        arma::mat detections = std::move(arma::join_rows(bb_mat, flows_mat)); 
        return detections;
}

Json::Value particles_to_json(const vector<Particle>& particles){
    Json::Value data;
    Json::Value bbs(Json::arrayValue);
    Json::Value flows(Json::arrayValue);
    for (const auto& p : particles){
        Json::Value bb(Json::arrayValue);
        bb.append(Json::Value(p.state[0]));
        bb.append(Json::Value(p.state[1]));
        bb.append(Json::Value(p.state[2]));
        bb.append(Json::Value(p.state[3]));
        bbs.append(bb);


        Json::Value flow(Json::arrayValue);
        flow.append(Json::Value(p.state[4]));
        flow.append(Json::Value(p.state[5]));
        flows.append(flow);
    }
    data["bb"] = bbs;
    data["flows"] = flows;

    return data;

}

int main()
{
    std::string file_dir;

    // std::cout << "enter the relative data file directory: ";
    // std::cin >> file_dir;
    file_dir = "../data/first_20_frame_optical_res.json";

    std::ifstream data_file(file_dir);
    if (!data_file.is_open()) std::cout << "Failed to open file." << std::endl;

    Json::Reader reader;
    Json::Value obj;
    reader.parse(data_file, obj);
    
    PhdFilterBox filter;

    // Json writer
    Json::Value bbs_and_flows(Json::arrayValue);



 
    for(const auto& data: obj)
    {
        
        arma::mat detections = get_detection_matrix(data);

        filter.update(detections);


        auto particles = filter.extract_target_states();
        // auto particles = filter.get_x_k_();
        plt::clf();
        plt::xlim(0, 540);
        plt::ylim(0, 960);
        plot_detections(detections);
        plot_particles(particles, 10);
        plt::pause(0.000001);


        auto particles_xk = filter.get_x_k_();
        bbs_and_flows.append(particles_to_json(particles_xk));

        //// temp sats for testing.
        // double wmin = 1000000000;
        // double wsum = 0;
        // double wmax = 0;
        // for(const auto& x:filter.get_x_k_())
        // {
        //     wmin = std::min({wmin, x.weight});
        //     wmax = std::max({wmax, x.weight});
        //     wsum += x.weight;
        // }
        // std::cout << "weights: " << wmin << ", " << wmax << "  tot: " << wsum << std::endl;

    }

    std::ofstream out_file("run_phd_output.json");
    out_file << bbs_and_flows;
    outfile.close();


    std::string stemp;
    std::cin >> stemp;
    
    return 0;
}