#include"phd_filter.h"

tuple <PHDupdate,mat> phd_filter::UpdatePHDComponents(Particle tracked_target){
    PHDupdate tracked_update;
    tracked_update.eta=H_*tracked_target.state;
    tracked_update.S=R+H_*tracked_target.P*H_.t();
    tracked_update.K = tracked_target.P * H_.t() * tracked_update.S.i();
    mat I=eye<mat>(size(tracked_target.P));
    mat tracked_P = (I - tracked_update.K * H_) * tracked_target.P * (I - tracked_update.K *H_).t()
    + tracked_update.K * R * tracked_update.K.t();
    auto t=make_tuple(tracked_update,tracked_P);
    return t;
}