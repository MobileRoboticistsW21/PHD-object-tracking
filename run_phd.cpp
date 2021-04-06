#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include "phd_filter_box.h" // TODO: this needs to be changed

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

    for(const auto& data: obj)
    {
        
        arma::mat detections = get_detection_matrix(data);

        filter.update(detections);

        break;  // TODO: remove
    }

    return 0;
}