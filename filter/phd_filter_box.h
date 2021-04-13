#pragma once
#include "phd_filter_base.h"


class PhdFilterBox : public PhdFilterBase
{

public:
 PhdFilterBox() 
    : PhdFilterBase()
    {
        arma::mat cov = diagmat(vec{1,1,8,8,5,5});
        // Updating these two values caused deviation from sensor input
        p_s_ = 0.9;
        p_d_ = 0.9;
        T_ = 0.0001; // min weight. Pruning.
        U_ = 40; 
        J_max_ = 1000;  // max particles
        
        J_gamma_ = 0;
        kP_gamma = cov;

        // Spawns
        J_beta_ = 0; // 2;
        kP_beta = 0.5*cov;
        kweight_beta_P = 0.5*cov;

        // Motion
        F_ = eye<mat>(6,6);
        F_(0, 4) = 1;
        F_(1, 5) = 1;
        Q_ = 0.05*cov;

        // Sensor
        H_ = eye<mat>(6,6);
        R_ = 1*cov;
        
        extraction_weight_threshold_ = 0.3;
    }

    virtual void update(const mat& detections)
    {
        J_gamma_ = detections.n_rows;
        mu_gamma_ = detections;
        PhdFilterBase::update(detections);
    }


private:

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
        spawn_target.state = mvnrnd(vec{0,0,0,0,0,0}, parent.P) + parent.state;
        spawn_target.P = kP_beta + parent.P;
        spawn_target.weight = 0;
        return spawn_target;
    }
    double SpawnWeight(vec spawn,vec parent)
    {        
        // return normpdf(spawn, parent, diagvec(kweight_beta_P))[0];
        return 0.25;
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
        return 0.25;
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
        return dist < 80;
    }


};