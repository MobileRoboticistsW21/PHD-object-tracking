#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>

// Function to read json file
Json::Value read_json(std::string json_file) {
    std::ifstream ifs(json_file);
    Json::Reader reader;
    Json::Value obj;
    reader.parse(ifs, obj);
    return obj;
}

// Test
int main() {
    Json::Value obj = read_json("first_20_frame_optical_res.json");
    std::cout<<obj[0]["bb"][0]<<std::endl;
    return 0;
}

