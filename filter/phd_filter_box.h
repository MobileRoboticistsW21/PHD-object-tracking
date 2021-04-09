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
        
        J_beta_ = 2;
        p_s_ = 0.99;
        p_d_ = 0.98;
        T_ = 0.00001;
        U_ = 4;
        J_max_ = 100;
        
        
        J_gamma_ = 0;  // J_gamma_ = 2;
        // mu_gamma_ = join_rows(vec{250, 250, 50, 50, 0, 0}, vec{-250, -250, 50, 50, 0, 0}).t();
        kP_gamma = diagmat(vec{100,100,100,100,25,25});
        
        kP_beta = diagmat(vec{50,50,50,50,10,10});
        kweight_beta_P = diagmat(vec{75,75,25,25,100,100});

        F_ = eye<mat>(6,6);
        F_(0,4) = 1;
        F_(1,5) = 1;
        F_.print();
        Q_ = 4 * eye<mat>(6,6);
        
        H_ = eye<mat>(6,6);
        R_ = 8 * eye<mat>(6,6); 
        
        extraction_weight_threshold_ = 0.02;
        
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
        Particle p;
        p.state = {250, 250, 50, 50, 0, 0};
        p.P = arma::mat(6,6,arma::fill::zeros);
        p.weight = 0.5;
        x_k_.push_back(p);
        p.state = {-250, -250, 50, 50, 0, 0};
        x_k_.push_back(p);
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
        // double birth_weight = 
        //     norm(
        //         0.1 * normpdf(current_state, mu_gamma_.row(0).t(), diagvec(kP_gamma)) 
        //         // + 
        //         // 0.1 * normpdf(current_state, mu_gamma_.row(1), diagvec(kP_gamma))
        //     );
        // return birth_weight;
        return 1/J_gamma_;
    }

    Particle propose_new_born_particle(int i) override
    {
        Particle p;
        p.weight = BirthWeight(mu_gamma_.row(i).t()); // not sure about weight
        p.state = mu_gamma_.row(i).t();
        p.P = kP_gamma;
        return p;
    }

};