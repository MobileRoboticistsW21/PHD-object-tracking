#include"phd_filter.h"

Particle phd_filter::ExistingObjectMotionModel(Particle existing_target){
    Particle pred_target;
    pred_target.weight=p_s_*existing_target.weight;
    pred_target.state=F_*existing_target.state;
    pred_target.P=Q+F_*existing_target.P*F_.t();
    return pred_target;
}