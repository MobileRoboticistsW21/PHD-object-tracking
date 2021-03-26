// #include"phd_filter.h"

// Particle phd_filter::SpawnMotionModel(Particle parent){
//     Particle spawn_target;
//     spawn_target.state=mvnrnd(vec{0,0,0,0},diagmat(vec{50,50,10,10}))
//                         + parent.state;
//     spawn_target.P= kP_beta + parent.P;
//     spawn_target.weight=0;
//     return spawn_target;
// }