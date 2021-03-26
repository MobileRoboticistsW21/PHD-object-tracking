#include <armadillo>
#include "phd_filter.h"


/**
 * Notice (priority medium) This needs to not hardcode values, 
 * instead setup struct to be passed that contains all necesssary parameters
 */
phd_filter::phd_filter(string type)
{
    if (type.compare("simulation") == 0)
    {
        J_k_ = 2;
        J_beta_ = 2;
        J_gamma_ = 2;
        sigma_v_ = 5;
        p_s_ = 0.99;
        p_d_ = 0.98;
        T_ = 0.00001;
        U_ = 4;
        J_max_ = 100;
        i_ = 0;
        mu_gamma_ = join_rows(vec{250, 250, 0, 0}, vec{-250, -250, 0, 0});
        // t_steps_ = 100;
        Particle p;
        p.state = {250, 250, 0, 0};
        p.P = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        p.weight = 0.5;
        x_k_.push_back(p);
        p.state = {-250, -250, 0, 0};
        x_k_.push_back(p);
        F_ = join_cols(join_rows(eye<mat>(2, 2), eye<mat>(2, 2)),
                       join_rows(zeros<mat>(2, 2), eye<mat>(2, 2)));
        
        H_ = join_rows(eye<mat>(2, 2), zeros<mat>(2, 2));
    }
}





void phd_filter::propagate_states(void)
{
    
    for (auto& x : x_k_)
    {
        Particle pred;
        pred.weight = p_s_*x.weight;
        pred.state = F_*x.state;
        pred.P = Q+F_*x.P*F_.t();

        x = pred;
    }
}

// TODO: Should this function return a state instead of a particle.
vector<Particle> phd_filter::extract_target_states(){
    vector<Particle> extracted_state;
    for(const auto& x : x_k_)
    {
        if(x.weight > 0.3) 
        {
            extracted_state.push_back(x);
        }
    }
    return extracted_state;
}








tuple <PHDupdate,mat> phd_filter::UpdatePHDComponent(Particle tracked_target){
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

void phd_filter::construct_phd_update_components()
{
    phd_updates_.clear();
    for (auto& x : x_k_)
    {
        tuple<PHDupdate, mat> target_update;
        target_update = UpdatePHDComponent(x);
        phd_updates_.push_back(get<0>(target_update));
        x.P = get<1>(target_update);
    }
}








Particle phd_filter::ObjectMissedDetection(Particle missed_target){
    Particle undetected_object;
    undetected_object.weight=(1-p_d_)*missed_target.weight;
    undetected_object.state=missed_target.state;
    undetected_object.P=missed_target.P;
    return undetected_object;  
}

// TODO: .... just change this
void phd_filter::FAILING_sensor_update_for_object_missing_detections()
{
    // // Missed Detections
    // x_k_.clear();
    // for (auto it7 : x_pred_)
    // {
    //     // TODO: make sure this is correct! where are we checking that the object is not detected?!
    //     x_k_.push_back(ObjectMissedDetection(it7));
    // }
    // x_k_.clear();
}




 



void phd_filter::sensor_update(mat detections) 
{

    vector<Particle> x_new;
    for (int z_idx = 0; z_idx < detections.n_rows; z_idx++)
    {
        vec z = arma::vectorise(detections.row(z_idx));
        for (int x_idx = 0; x_idx < x_k_.size(); x_idx++)
        {
            Particle target = x_k_[x_idx];
            PHDupdate detected_update = phd_updates_[x_idx];
            
            Particle detected_target;
            double c=as_scalar(mean(normpdf(z, detected_update.eta, diagvec(detected_update.S))));
            detected_target.weight=p_d_*target.weight*as_scalar(mean(normpdf(z,detected_update.eta,diagvec(detected_update.S))));
            detected_target.state=target.state+detected_update.K*(z-detected_update.eta);
            detected_target.P=target.P;

            x_new.push_back(detected_target);
        }
    }
    x_k_ = x_new;
}










Particle phd_filter::SpawnMotionModel(Particle parent){
    Particle spawn_target;
    spawn_target.state=mvnrnd(vec{0,0,0,0},diagmat(vec{50,50,10,10}))
                        + parent.state;
    spawn_target.P= kP_beta + parent.P;
    spawn_target.weight=0;
    return spawn_target;
}
double phd_filter::SpawnWeight(vec spawn,vec parent){
    double spawn_weight = norm(0.05*normpdf(spawn,parent,
    diagvec(kweight_beta_P))+0.1*normpdf(spawn,parent,
    diagvec(kweight_beta_P)));
    return spawn_weight;
}
/** UPDATE
 * This function should return a set of new spawned targets
 */
void phd_filter::propose_spawned_targets(void)
{
    // Spawning
    /*
    for(int it3=0;it3<J_beta_;it3++)
    {
        for(auto it4:x_k_)
        {
            i_++;
            pred_target=SpawnMotionModel(it4);
            pred_target.weight=SpawnWeight(pred_target.state,
            it4.state)*it4.weight;
            x_pred_.push_back(pred_target);
        }
    }
    */
}







double phd_filter::BirthWeight(vec current_state){
    double birth_weight=norm(0.1*normpdf(current_state,mu_gamma_.col(0),
    diagvec(kP_gamma))+0.1*normpdf(current_state,mu_gamma_.col(1),
    diagvec(kP_gamma)));
    return birth_weight;
}
/** UPDATE
 * This function should return a set of new born targets
 */
void phd_filter::propose_new_born_targets(void)
{
    // int i_ = 0;
    // Particle pred_target;

    // Simulation
    // Birth
    /*
    for(int it1=0;it1<J_gamma_;it1++){
        for(auto it2:x_k_){
            i_++;
            pred_target.weight=BirthWeight(it2.state);
            pred_target.state=mu_gamma_.col(it1); // make it constant
            pred_target.P=kP_gamma;
            x_pred_.push_back(pred_target);
        }
    }
    */
}