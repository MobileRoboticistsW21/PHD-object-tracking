#pragma once
#include "phd_filter_base.h"


class PhdFilterBox : public PhdFilterBase
{

public:
 PhdFilterBox() 
    : PhdFilterBase()
    {
        // J_k_ = 2;
        // sigma_v_ = 5;
        auto cov = 0.1*diagmat(vec{1,1,2,2,4,4});
        // Updating these two values caused deviation from sensor input
        p_s_ = 0.8; //0.99;
        p_d_ = 0.75; //0.98;

        T_ = 0.0001; // min weight. Pruning.
        U_ = 20; 
        J_max_ = 1000;  // max particles
        
        J_gamma_ = 0;
        // kP_gamma = diagmat(vec{5,5,5,5,5,5});
        kP_gamma = cov;

        // Spawns
        J_beta_ = 0; // 2;
        kP_beta = eye<mat>(6, 6);
        kweight_beta_P = eye<mat>(6, 6);

        // Motion
        F_ = eye<mat>(6,6);
        F_(0, 4) = 1;
        F_(1, 5) = 1;
        // Q_ = diagmat(vec{1.25,1.25,2.5,2.5,2.5,2.5});
        Q_ = 0.75*cov;

        // Sensor
        H_ = eye<mat>(6,6);
        // R_ = 0.1*diagmat(vec{2,2,5,5,4,4});
        R_ = 2*cov;
        
        extraction_weight_threshold_ = 0.03;
        
        // initialize_particles();
    }

    virtual void update(const mat& detections)
    {
        J_gamma_ = detections.n_rows;
        mu_gamma_ = detections;
        PhdFilterBase::update(detections);
    }


private:

    void initialize_particles()
    {
        // Particle p;
        // x_k_.push_back(p);
    }

    Particle get_default_particle()
    {
        Particle p;
        p.state = vec{6, arma::fill::zeros};
        p.P = mat{6,6, arma::fill::zeros};
        p.weight = 0;
        return p;
    }

    Particle SpawnMotionModel(Particle parent)
    {
        Particle spawn_target;
        spawn_target.state = mvnrnd(vec{0,0,0,0,0,0},diagmat(vec{50,50,10,10,20,20})) + parent.state;
        spawn_target.P = kP_beta + parent.P;
        spawn_target.weight = 0;
        return spawn_target;
    }
    double SpawnWeight(vec spawn,vec parent)
    {
        // auto n = normpdf(spawn, parent, diagvec(kweight_beta_P));
        // return norm(0.05*n + 0.1*n);
        return normpdf(spawn, parent, diagvec(kweight_beta_P))[0];
    }

    Particle spawn_particle(const Particle& x) override
    {
        auto pred_target = SpawnMotionModel(x);
        pred_target.weight = SpawnWeight(pred_target.state, x.state) * x.weight;
        return pred_target;
    }


    double BirthWeight(vec current_state)
    {
        // return norm(normpdf(current_state, mu_gamma_.row(0).t(), diagvec(kP_gamma)));
        return 0.1;
    }

    Particle propose_new_born_particle(int i) override
    {
        Particle p;
        p.weight = BirthWeight(mu_gamma_.row(i).t()); // not sure about weight
        p.state = mu_gamma_.row(i).t();
        p.P = kP_gamma;
        return p;
    }

    bool potentially_associated(
        const Particle& target, 
        const arma::vec& detection) override
    {
        arma::vec state_diff = target.state - detection; 
        double dist = sqrt(as_scalar(state_diff.t() * target.P.i() * state_diff));
        return dist < 100;
    }


};