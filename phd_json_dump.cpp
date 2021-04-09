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
    Json::Value bbs_and_flows_extract(Json::arrayValue);
    
    PhdFilterBox filter;

    float itr = 0;
    for(const auto& data: obj)
    {
        std::cout << "\rProgress: " << 100.0 * itr++ / static_cast<float>(obj.size()) << "%" << std::flush;
        
        arma::mat detections = get_detection_matrix(data);

        filter.update(detections);

        bbs_and_flows.append(particles_to_json(filter.get_x_k_()));
        bbs_and_flows_extract.append(particles_to_json(filter.extract_target_states()));
        
        // stats_for_debugging(filter);
    }

    std::ofstream comprehensive_file("all_particles_dump.json"); 
    comprehensive_file << bbs_and_flows;
    comprehensive_file.close();

    std::ofstream extracted_file("tracks_dump.json"); 
    extracted_file << bbs_and_flows_extract;
    extracted_file.close();

    std::cout << std::endl << "Done!" << std::endl;

    return 0;
}