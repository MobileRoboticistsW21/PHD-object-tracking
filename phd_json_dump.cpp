#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include "filter/phd_filter_box.h"
#include "utils/data_utils.h"

void stats_for_debugging(PhdFilterBox& filter)
{
    // temp sats for testing.
    double wmin = 1000000000;
    double wsum = 0;
    double wmax = 0;
    for(const auto& x:filter.get_x_k_())
    {
        wmin = std::min({wmin, x.weight});
        wmax = std::max({wmax, x.weight});
        wsum += x.weight;
    }
    std::cout << "weights: " << wmin << ", " << wmax << "  tot: " << wsum << std::endl;
}

int main()
{
    std::string file_dir;

    std::cout << "enter the relative data file directory: ";
    std::cin >> file_dir;
    // file_dir = "../data/first_20_frame_optical_res.json";

    std::ifstream data_file(file_dir);
    if (!data_file.is_open()) std::cout << "Failed to open file." << std::endl;

    Json::Reader reader;
    Json::Value obj;
    reader.parse(data_file, obj);
    
    Json::Value bbs_and_flows(Json::arrayValue);
    
    PhdFilterBox filter;
    
    for(const auto& data: obj)
    {
        arma::mat detections = get_detection_matrix(data);

        filter.update(detections);

        bbs_and_flows.append(particles_to_json(filter.get_x_k_()));
        
        // stats_for_debugging(filter);
    }

    std::ofstream out_file("run_phd_output.json");
    out_file << bbs_and_flows;
    out_file.close();
    std::cout << "Done!" << std::endl;

    return 0;
}