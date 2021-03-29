#include"phd_filter.h"



// struct MergeCandidate
// {
//     Particle particle;
//     bool merged;
// };


Particle* get_max_weight_particle(vector<Particle>& particles)
{
    Particle* heaviest;
    for (Particle& particle : particles)
    {
        if(particle.weight > heaviest->weight)
        {
            heaviest = &particle;
        }
    }
    return heaviest;
}

double mahalanobis_distance(Particle a, Particle b)
{
    auto state_diff = a.state - b.state; 
    return as_scalar(state_diff.t() * a.P.i() * state_diff);
}

void phd_filter::PruningAndMerging(){
    vector<Particle> pruned_set;
    vector<Particle> heavy_particles;
    
    // Remove tracks with low weights
    for(const auto& x : x_k_)
    {
        if(x.weight > T_)
        {
            heavy_particles.push_back(x);
        }
    }


    ////////////////////////////////////////////////////////////////////////
    /////////////////////// TODO: re-write this /////////////////////////
    // Base this on a priority queue
    while(!heavy_particles.empty()){
        auto& heaviest_p = *get_max_weight_particle(heavy_particles);

        vector<Particle> I_diff;
        vector<Particle> L;
        for(const auto& p : heavy_particles){
            if(mahalanobis_distance(p, heaviest_p) < U_)
            {
                L.push_back(p);
            }
            else
            {
                I_diff.push_back(p);
            }
        }

        Particle merged_target;
        merged_target.weight = 0;
        merged_target.state = vec{0,0,0,0};
        merged_target.P=zeros<mat>(4,4);
        for(auto iter:L)
        {
            merged_target.weight=merged_target.weight+iter.weight;
            merged_target.state=merged_target.state+iter.weight*iter.state;
        }
        merged_target.state=merged_target.state/merged_target.weight;
        for(auto iter:L)
        {
             merged_target.P=merged_target.P+iter.weight* (iter.P+(merged_target.state-iter.state) * (merged_target.state-iter.state).t());
        }
        merged_target.P=merged_target.P/merged_target.weight;

        heavy_particles=I_diff;
        pruned_set.push_back(merged_target);
    }
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

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

