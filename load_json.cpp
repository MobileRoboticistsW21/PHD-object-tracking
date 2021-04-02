#include <iostream>
#include <jsoncpp/json/json.h>
#include <fstream>

int main() {
    std::ifstream ifs("first_20_frame_optical_res.json");
    Json::Reader reader;
    Json::Value obj;
    reader.parse(ifs, obj);
    std::cout << obj[0]["bb"]<<std::endl;
    return 1;
}

