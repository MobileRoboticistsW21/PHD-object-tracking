#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>
#include "phd_filter.h"

int main()
{
    std::string file_dir;

    std::cout << "enter the relative data file directory: ";
    std::cin >> file_dir;

    std::ifstream data_file(file_dir);
    if (!data_file.is_open()) std::cout << "Failed to open file." << std::endl;

    Json::Reader reader;
    Json::Value obj;
    reader.parse(data_file, obj);
    
    phd_filter filter;

    for(const auto& data: obj)
    {
        // TODO: change this data to a format that the filter can consume.
        std::cout << data;
        break;
    }

    return 0;
}