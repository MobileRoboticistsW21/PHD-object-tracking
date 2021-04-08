#pragma once

#include "phd_filter_base.h"


class PhdFilter2d : public PhdFilterBase
{

public:
    PhdFilter2d() 
    : PhdFilterBase()
    {
        // J_k_ = 2;
        // sigma_v_ = 5;
        
        J_beta_ = 2;
        J_gamma_ = 2;
        p_s_ = 0.99;
        p_d_ = 0.98;
        T_ = 0.00001;
        U_ = 4;
        J_max_ = 100;
        mu_gamma_ = join_rows(vec{250, 250, 0, 0}, vec{-250, -250, 0, 0});
        kP_gamma = diagmat(vec{100,100,25,25});
        kP_beta = diagmat(vec{50,50,10,10});
        kweight_beta_P = diagmat(vec{100,100,400,400});
        F_ = join_cols(
            join_rows(eye<mat>(2, 2), eye<mat>(2, 2)),
            join_rows(zeros<mat>(2, 2), eye<mat>(2, 2)));

        H_ = join_rows(eye<mat>(2, 2), zeros<mat>(2, 2));

        Q_ = join_cols(
                join_rows(1.25 * eye<mat>(2,2), 2.5 * eye<mat>(2,2)),
                join_rows(1.25 * eye<mat>(2,2), 5.0 * eye<mat>(2,2)));

        // R_ = 100 * eye<mat>(2,2); 
        R_ = 8 * eye<mat>(2,2); 

        initialize_particles();

        extraction_weight_threshold_ = 0.3;

    }

private:

    void initialize_particles()
    {
        Particle p;
        p.state = {250, 250, 0, 0};
        p.P = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        p.weight = 0.5;
        x_k_.push_back(p);
        p.state = {-250, -250, 0, 0};
        x_k_.push_back(p);
    }

    Particle get_default_particle()
    {
        Particle p;
        p.state = vec{4, arma::fill::zeros};
        p.P = mat{4,4, arma::fill::zeros};
        p.weight = 0;
        return p;
    }
  
    // PHDupdate get_default_PHDupdate()
    // {
    //     PHDupdate u;
    //     u.eta = vec{4, arma::fill::zeros};  // Projected detection
    //     u.S = mat{2,2, arma::fill::zeros};  // Projected sensor covariance
    //     u.K = mat{4,4, arma::fill::zeros};  // Kalman gain
    //     u.P = mat{4,4, arma::fill::zeros};  // Updated state covariance
    //     return u;
    // }


    Particle SpawnMotionModel(Particle parent){
        Particle spawn_target;
        spawn_target.state = mvnrnd(vec{0,0,0,0},diagmat(vec{50,50,10,10})) + parent.state;
        spawn_target.P = kP_beta + parent.P;
        spawn_target.weight = 0;
        return spawn_target;
    }
    double SpawnWeight(vec spawn,vec parent){
        auto n = normpdf(spawn, parent, diagvec(kweight_beta_P));
        return norm(0.05*n + 0.1*n);
    }

    Particle spawn_particle(const Particle& x) override
    {
        auto pred_target = SpawnMotionModel(x);
        pred_target.weight = SpawnWeight(pred_target.state, x.state) * x.weight;
        return pred_target;
    }


    double BirthWeight(vec current_state){
        double birth_weight = 
            norm(
                0.1 * normpdf(current_state, mu_gamma_.col(0), diagvec(kP_gamma)) 
                + 
                0.1 * normpdf(current_state, mu_gamma_.col(1), diagvec(kP_gamma))
            );
        return birth_weight;
    }

    Particle propose_new_born_particle(int i) override
    {
        Particle p;
        p.weight = BirthWeight(mu_gamma_.col(i)); // not sure about weight
        p.state = mu_gamma_.col(i);
        p.P = kP_gamma;
        return p;
    }


};