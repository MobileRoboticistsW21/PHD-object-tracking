#include"phd_filter.h"

void phd_filter::NormalizeWeights(){
    double sum_weights=0.0;
    for(auto iter:x_k_){
        sum_weights=sum_weights+iter.weight;
    }
    for_each(x_k_.begin(), x_k_.end(), [&](Particle& a){
        a.weight=a.weight/sum_weights;
    });
}

