#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include "phd_filter_2d.h" // TODO: this needs to be changed

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
    
    PhdFilter2d filter;

    for(const auto& data: obj)
    {
        arma::mat bb_mat = jason_to_arma_mat(data["bb"]);
        std::cout << bb_mat << std::endl;
        
        arma::mat flows_mat = jason_to_arma_mat(data["flow"]);
        std::cout << flows_mat << std::endl;

        break;
    }

    return 0;
}