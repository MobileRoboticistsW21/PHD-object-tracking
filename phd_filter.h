#pragma once

#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<armadillo>

using namespace std;
using namespace arma;

struct Particle
{
   vec state{4};  // 4x1 vector [x, y, Vx, Vy]
   mat P{4,4}; // 4x4 covariance
   double weight; // particle weight
};

struct PHDupdate
{
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

        vector<Particle> get_x_k_(){ return x_k_; }  //Notice (priority Low): not ideal, needs changing.


        //// MAIN FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////
        // Notice: Most of those functions need to be called as a group.
        //          they should be made private and an encompassing function should be put in place. 

        /**
         * NOTICE: {priority low} functioning okay 
         * @def: performs linear motion update.
         */
        void propagate_states(void);

        /**
         * NOTICE: {priority medium} Seems working... not well inspected  
         * TODO: Change detections to be columns rather than rows.
         *
         * @def: performs sensor update
         * @param detections: 2xN matrix of N detections (one per row)
         */
        void sensor_update(const mat& detections);

        void NormalizeWeights(); // Notice (priority high): needs review and revamp
        void PruningAndMerging(); // Notice (priority high): needs review and revamp

        vector<Particle> extract_target_states(); // Notice (priority high): needs review and revamp


        void propose_spawned_targets(void); // Notice (priority Medium-high) Currently not implemented
        
        void propose_new_born_targets(void); // Notice (priority Medium-high)Currently not implemented

        void construct_phd_update_components(); // Notice (priority high): needs revew and revamp

        void FAILING_sensor_update_for_object_missing_detections(); // Notice (priority high): needs review and revamp
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        ////////////////////////////////////////////////////////////////////////////////
        // Notice (priority medium): Those functions need massive revamp
        //              most of them can be intregrated with the functions that call 
        ////////////////////////////////////////////////////////////////////////////////
        double BirthWeight(vec); // used in propose_new_born_targets
        Particle SpawnMotionModel(Particle); // used in propose_spawned_targets
        double SpawnWeight(vec,vec); // used in propose_spawned_targets
        tuple <PHDupdate,mat> UpdatePHDComponent(Particle); // used in construct_phd_update_components
        Particle ObjectMissedDetection(Particle);  // used in FAILING_sensor_update_for_object_missing_detections
        vector<Particle> CarBirth(); // NOTICE: not implemented, is it needed?
        ////////////////////////////////////////////////////////////////////////////////

        




        //////////////// Member variables //////////////////
        vector<Particle> x_k_;  // states
        vector<PHDupdate> phd_updates_; // phd update components

        ////// Birth params
        mat mu_gamma_;
        int J_gamma_;
        const mat kP_gamma = diagmat(vec{100,100,25,25});

        ////// Spawn params
        int J_beta_;
        const mat kP_beta = diagmat(vec{50,50,10,10});
        const mat kweight_beta_P = diagmat(vec{100,100,400,400});

        ////// Motion update:
        double p_s_; // probability that a target still exists at time k given that its previous state.
                     // notice: currently state and time independent.
        mat F_{4,4, arma::fill::zeros}; // Linearize motion model
        mat Q_{4,4, arma::fill::zeros}; // Motion noise covariance.
            

        ////// Sensor update  (and those related to it)
        double p_d_; // def: probability of detection given a state x at time k.
                     // notice: currently state and time independent.
        mat H_{4,4, arma::fill::zeros};  // TODO: definition
                                         // used in constructing PHD update components
        mat R_{2, 2, arma::fill::zeros}; // TODO: definition
                                         // Used in constructing PHD update components
        
        
        ////// Pruning & Merging
        double T_;  // Weight threshold. Particles with lower weights are pruned
        int J_max_; // Max particles: If there are more than that many particles, discard ones with lower weight
        
        int U_; // Merge distance threshold: Particles that are closer than this distance are merged.
        

        /////// unsorted
        int J_k_;
        int sigma_v_;
        int i_;
        // Constant variables
        const mat kInit_covP={{100,0,0,0},{0,100,0,0},{0,0,10,0},{0,0,0,10}};
        
};