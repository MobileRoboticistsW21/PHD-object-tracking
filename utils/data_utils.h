#include<armadillo>
#include <jsoncpp/json/json.h>
#include "../filter/phd_filter_base.h"

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

Json::Value particles_to_json(const std::vector<Particle>& particles)
{
    Json::Value data;
    Json::Value bbs(Json::arrayValue);
    Json::Value flows(Json::arrayValue);
    Json::Value weights(Json::arrayValue);
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


        weights.append(Json::Value(p.weight));
    }
    data["bb"] = bbs;
    data["flows"] = flows;
    data["weights"] = weights;

    return data;

}