#include <armadillo>
#include "phd_filter_base.h"


/**
 * Notice (priority medium) This needs to not hardcode values, 
 * instead setup struct to be passed that contains all necesssary parameters
 */
 // TODO: meaningful constructor
PhdFilterBase::PhdFilterBase()
{
}

// the higher level function that calls other stuff
// TODO: need to add more inputs?
// TODO: functions called in here can become private?
// TODO: move plotting outside?
void PhdFilterBase::update(const mat& detections)
{
    x_pred_.clear();
    
    propose_new_born_targets();

    propose_spawned_targets();

    propagate_states();

    sensor_update(detections);
    NormalizeWeights(); // TODO: move normalization into functions that update particles.

    PruningAndMerging();
    NormalizeWeights();  // TODO: Check if this is required. Likely is. 

    x_pred_.clear();
}


void PhdFilterBase::propose_spawned_targets(void)
{
    for(int i = 0; i < J_beta_; i++)        // for each spawning Gaussian
    {
        for(const auto& x : x_k_)                        // for each current target
        {
            x_pred_.push_back(spawn_particle(x));
        }
    }
}


void PhdFilterBase::propose_new_born_targets(void)
{
    Particle pred_target;
    for (int i = 0; i < J_gamma_; i++) 
    {
        x_pred_.push_back(propose_new_born_particle(i));      // need to declare x_pred_ somewhere 
    }
    
}

void PhdFilterBase::propagate_states(void)
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

vector<Particle> PhdFilterBase::extract_target_states(){
    vector<Particle> extracted_state;
    for(const auto& x : x_k_)
    {
        if(x.weight > extraction_weight_threshold_) 
        {
            extracted_state.push_back(x);
        }
    }
    return extracted_state;
}


vector<Particle> PhdFilterBase::propose_particles_with_missing_detections()
{
    vector<Particle> x_miss = x_k_;
    for (auto& x : x_miss)
    {
        x.weight *= (1 - p_d_);
    }
    return x_miss;
}

PHDupdate PhdFilterBase::UpdatePHDComponent(const Particle& p)
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

void PhdFilterBase::construct_phd_update_components()
{
    phd_updates_.clear();
    for (const auto& x : x_pred_)
    {
        phd_updates_.push_back(UpdatePHDComponent(x));
    }
}

void PhdFilterBase::sensor_update(const mat& detections) 
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

void PhdFilterBase::NormalizeWeights(){
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

Particle& PhdFilterBase::get_max_weight_particle(vector<Particle>& particles)
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

double PhdFilterBase::mahalanobis_distance(const Particle& a, const Particle& b)
{
    arma::vec state_diff = a.state - b.state; 
    return sqrt(as_scalar(state_diff.t() * a.P.i() * state_diff));
}

Particle PhdFilterBase::merge_particles(const vector<Particle> particles)
{
    Particle merged_p = get_default_particle();
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

void PhdFilterBase::PruningAndMerging(){
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