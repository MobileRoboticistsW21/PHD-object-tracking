#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<armadillo>

using namespace std;
using namespace arma;

struct Particle{
   vec state{4};
   mat P{4,4};
   double weight; 
};

struct PHDupdate{
    vec eta{4};
    mat S{2,2};
    mat K{4,4};
};

// Notice (priorty Medium): This should house the settings needed for the filter eventually. 
struct PHD_filter_parameters
{};

class phd_filter{
    public:
        phd_filter(string);  // Notice (priority low): currently this hardcodes settings, that should be changed

        void propagate_states(void);  // Notice (priority high):

        vector<Particle> get_x_k_(){ return x_k_; }  //Notice (priority Low): not ideal, needs changing.


        //// MAIN FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////
        // Notice: Most of those functions need to be called as a group.
        //          they should be made private and an encompassing function should be put in place. 

        void propose_spawned_targets(void); // Notice (priority Medium-high) Currently not implemented
        void propose_new_born_targets(void); // Notice (priority Medium-high)Currently not implemented

        void construct_phd_update_components(); // Notice (priority high): needs revew and revamp
        
        void sensor_update(mat z_); // Notice (priority high): needs review and revamp
        void FAILING_sensor_update_for_object_missing_detections(); // Notice (priority high): needs review and revamp
        void NormalizeWeights(); // Notice (priority high): needs review and revamp

        void PruningAndMerging(); // Notice (priority high): needs review and revamp

        vector<Particle> extract_target_states(); // Notice (priority high): needs review and revamp
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        vector<Particle> CarBirth();


        ////////////////////////////////////////////////////////////////////////////////
        // Notice (priority medium): Those functions need massive revamp
        //              most of them can be intregrated with the functions that call 
        ////////////////////////////////////////////////////////////////////////////////
        double BirthWeight(vec); // used in propose_new_born_targets
        Particle SpawnMotionModel(Particle); // used in propose_spawned_targets
        double SpawnWeight(vec,vec); // used in propose_spawned_targets
        tuple <PHDupdate,mat> UpdatePHDComponent(Particle); // used in construct_phd_update_components
        Particle ObjectMissedDetection(Particle);  // used in FAILING_sensor_update_for_object_missing_detections
        ////////////////////////////////////////////////////////////////////////////////

        // Member variables
        int J_k_, J_beta_, J_gamma_, sigma_v_;
        double p_s_, p_d_, T_;
        int U_, J_max_, i_;
        mat mu_gamma_;
        
        vector<Particle> x_k_;  // states
        vector<PHDupdate> phd_updates_;

        mat F_{4,4};
        mat H_{4,4};
        vector<mat> bbox_;

        // Constant variables
        mat kInit_covP={{100,0,0,0},{0,100,0,0},{0,0,10,0},{0,0,0,10}};
        mat kP_gamma=diagmat(vec{100,100,25,25});
        mat kP_beta=diagmat(vec{50,50,10,10});
        mat kweight_beta_P=diagmat(vec{100,100,400,400});
        mat Q=join_cols(join_rows(1.25*eye<mat>(2,2), 2.5*eye<mat>(2,2)),join_rows(1.25*eye<mat>(2,2), 5*eye<mat>(2,2)));
        mat R=100*eye<mat>(2,2);

};