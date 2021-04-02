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
    }
    F_ = join_cols(
        join_rows(eye<mat>(2, 2), eye<mat>(2, 2)),
        join_rows(zeros<mat>(2, 2), eye<mat>(2, 2)));

    H_ = join_rows(eye<mat>(2, 2), zeros<mat>(2, 2));

    Q_ = join_cols(
            join_rows(1.25 * eye<mat>(2,2), 2.5 * eye<mat>(2,2)),
            join_rows(1.25 * eye<mat>(2,2), 5.0 * eye<mat>(2,2)));

    // R_ = 100 * eye<mat>(2,2); 
    R_ = 8 * eye<mat>(2,2); 
}



Particle phd_filter::SpawnMotionModel(Particle parent){
    Particle spawn_target;
    spawn_target.state = mvnrnd(vec{0,0,0,0},diagmat(vec{50,50,10,10})) + parent.state;
    spawn_target.P = kP_beta + parent.P;
    spawn_target.weight = 0;
    return spawn_target;
}
double phd_filter::SpawnWeight(vec spawn,vec parent){
    auto n = normpdf(spawn, parent, diagvec(kweight_beta_P));
    double spawn_weight = norm(0.05*n + 0.1*n);
    return spawn_weight;
}
/** UPDATE
 * This function should return a set of new spawned targets
 */
void phd_filter::propose_spawned_targets(void)
{
    for(int it2 = 0; it2 < J_beta_; it2++)        // for each spawning Gaussian
    {
        for(const auto& x : x_k_)                        // for each current target
        {
            i_++;
            auto pred_target = SpawnMotionModel(x);
            pred_target.weight = SpawnWeight(pred_target.state, x.state) * x.weight;
            x_pred_.push_back(pred_target);
        }
    }

    // need to add current targets to x_pred_? after checking survival 
    
    
}

double phd_filter::BirthWeight(vec current_state){
    double birth_weight = 
        norm(
            0.1 * normpdf(current_state, mu_gamma_.col(0), diagvec(kP_gamma)) 
            + 
            0.1 * normpdf(current_state, mu_gamma_.col(1), diagvec(kP_gamma))
        );
    return birth_weight;
}
/** UPDATE
 * This function should return a set of new born targets
 */
void phd_filter::propose_new_born_targets(void)
{
    i_ = -1;       // reset i_ to -1 every iteration, because i_++ is the first thing in the loop
    // i_ should be the same as x_pred_.size(), so we can remove i_
    x_pred_.clear();
    Particle pred_target;
    for (int it1 = 0; it1 < J_gamma_; it1 ++) {
        i_ = i_ + 1;
        pred_target.weight = BirthWeight(mu_gamma_.col(it1)); // not sure about weight
        pred_target.state = mu_gamma_.col(it1);
        pred_target.P = kP_gamma;
        x_pred_.push_back(pred_target);      // need to declare x_pred_ somewhere 
    }
    
    
}

void phd_filter::propagate_states(void)
{
    for (const auto& x : x_k_)
    {
        Particle pred;
        pred.weight = p_s_ * x.weight;
        pred.state = F_ * x.state;
        pred.P = Q_ + (F_ * x.P * F_.t());

        x_pred_.push_back(pred);
    }
}

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






vector<Particle> phd_filter::propose_particles_with_missing_detections()
{
    vector<Particle> x_miss = x_k_;
    for (auto& x : x_miss)
    {
        x.weight *= (1 - p_d_);
    }
    return x_miss;
}

PHDupdate phd_filter::UpdatePHDComponent(const Particle& p)
{
    PHDupdate u;
    u.eta= H_* p.state;
    u.S= R_ + H_*p.P*H_.t();
    u.K = p.P*H_.t()*u.S.i();
    mat I = eye<mat>(size(p.P));
    mat d = (I - u.K * H_);
    u.P = d*p.P*d.t() + u.K*R_*u.K.t();
    return u;
}

void phd_filter::construct_phd_update_components()
{
    phd_updates_.clear();
    for (const auto& x : x_pred_)
    {
        phd_updates_.push_back(UpdatePHDComponent(x));
    }
}

void phd_filter::sensor_update(const mat& detections) 
{
    vector<Particle> x_new;
    
    construct_phd_update_components();
    
    // x_new = propose_particles_with_missing_detections();  // TODO: Confirm the use of this 
    
    for (int z_idx = 0; z_idx < detections.n_rows; z_idx++)
    {
        vec z = arma::vectorise(detections.row(z_idx));
        for (int x_idx = 0; x_idx < x_pred_.size(); x_idx++)
        {
            Particle& target = x_pred_[x_idx];
            PHDupdate& p_update = phd_updates_[x_idx];
            
            auto distribution = normpdf(z, p_update.eta, diagvec(p_update.S));
            double distribution_mean = as_scalar(mean(distribution));

            Particle detected_target;
            detected_target.weight = p_d_ * target.weight * distribution_mean;
            detected_target.state= target.state + p_update.K * (z - p_update.eta);
            detected_target.P = p_update.P;

            x_new.push_back(detected_target);
        }
    }
    x_k_ = x_new;
}















void phd_filter::NormalizeWeights(){
    double tot = 0.0;
    for(const auto& x : x_k_)
    {
        tot += x.weight;
    }
    for (auto& x : x_k_)
    {
        x.weight /= tot;
    }
}

Particle& phd_filter::get_max_weight_particle(vector<Particle>& particles)
{
    Particle& heaviest = particles[0];
    for (Particle& particle : particles)
    {
        if(particle.weight > heaviest.weight)
        {
            heaviest = particle;
        }
    }
    return heaviest;
}

double phd_filter::mahalanobis_distance(const Particle& a, const Particle& b)
{
    arma::vec state_diff = a.state - b.state; 
    return sqrt(as_scalar(state_diff.t() * a.P.i() * state_diff));
}

Particle phd_filter::merge_particles(const vector<Particle> particles)
{
    Particle merged_p;
    for (const auto& elem : particles) 
    { 
        merged_p.weight += elem.weight; 
    }
    
    for (const auto& elem: particles) 
    { 
        merged_p.state += elem.weight * elem.state; 
    }
    merged_p.state /= merged_p.weight;
    
    for(const auto& elem : particles)
    {
        arma::vec delta_state = merged_p.state - elem.state; 
        arma::mat inovation_covariance = delta_state * delta_state.t();
        merged_p.P += elem.weight * (elem.P + inovation_covariance);
    }
    merged_p.P /= merged_p.weight;
    return merged_p;
}

void phd_filter::PruningAndMerging(){
    vector<Particle> pruned_set;
    vector<Particle> heavy_particles;
    for(const auto& x : x_k_)
    {
        if(x.weight > T_)
        {
            heavy_particles.push_back(x);
        }
    }

    while(!heavy_particles.empty()){
        Particle& heaviest_p = get_max_weight_particle(heavy_particles);

        vector<Particle> unmerged_particles, merge_candidates;
        for(const auto& p : heavy_particles){
            if(mahalanobis_distance(p, heaviest_p) < U_)
            {
                merge_candidates.push_back(p);
            }
            else
            {
                unmerged_particles.push_back(p);
            }
        }

        pruned_set.push_back(merge_particles(merge_candidates));

        heavy_particles = unmerged_particles;
    }
    if(pruned_set.size() > J_max_)
    {
        auto comparator = [](const Particle &a, const Particle &b)->bool
        {
            return a.weight > b.weight;
        };
        sort(pruned_set.begin(), pruned_set.end(), comparator);
        x_k_ = vector<Particle>(pruned_set.begin(), pruned_set.begin() + J_max_);
    } 
    else
    {
        x_k_ = pruned_set;
    }   
}

