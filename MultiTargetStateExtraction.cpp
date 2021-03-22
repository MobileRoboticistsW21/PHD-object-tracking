#include"phd_filter.h"

void phd_filter::MultiTargetStateExtraction(){
    vector<Particle> extracted_state;
    for_each(x_k_.begin(), x_k_.end(), [&](Particle& a){
        if(a.weight>0.3) extracted_state.push_back(a);
    });
    x_k_.clear();
    x_k_=extracted_state;
}