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
   vec state;
   mat P;
   double weight = 0;
};

struct PHDupdate
{
    vec eta;  // Projected detection
    mat S  ;  // Projected sensor covariance
    mat K  ;  // Kalman gain
    mat P  ;  // Updated state covariance
};


class PhdFilterBase{
    public:
        PhdFilterBase();  // Notice (priority low): currently this hardcodes settings, that should be changed
        
        /**
         * TODO: be specific about what's needed as input. 
         */
        void update(const mat& detections);

        /**
         * NOTICE: Not ideal. Mainly for debugging.
         */
        vector<Particle> get_x_k_()
        { 
            return x_k_; 
        }  
      
        /**
         * @def get targets from tracked particles. 
         * @return particles with weight higher than a set threashold.
         */
        vector<Particle> extract_target_states();

    protected:
        
        /**
         * NOTICE: This is not necessary if better implemented.
         * Currently this is only required for the sensor update function.
         */
        virtual Particle get_default_particle() = 0;

        /**
         * @def: performs linear motion update.
         */
        void propagate_states(void);

        /**
         * @def: performs sensor update
         * @param detections: 2xN matrix of N detections (one per row)
         */
        void sensor_update(const mat& detections);

        /**
         * @def Determines whether the detection is close enough to be considered 
         * associated with a track. the main purpose of this is code speedup.
         */
        virtual bool potentially_associated(const Particle& target, const arma::vec& detection);

        /**
         * @def Normalizes all particle weights
         */
        void NormalizeWeights(); 
        
        /**
         * @def Merges particles of low manhalanobis distance proximity. 
         *      If many particles remain (configurable) the ones with lower weight are discarded. 
         */
        void PruningAndMerging();

        vector<Particle> propose_particles_with_missing_detections();

        void propose_spawned_targets(void);
        virtual Particle spawn_particle(const Particle& x) = 0;
        
        void propose_new_born_targets(void);
        virtual Particle propose_new_born_particle(int i) = 0;

        void construct_phd_update_components();
        PHDupdate UpdatePHDComponent(const Particle&); 
        
        Particle merge_particles(const vector<Particle> particles);
        
        // general particle utils
        Particle& get_max_weight_particle(vector<Particle>& particles);
        double mahalanobis_distance(const Particle& a, const Particle& b);

        //////////////// Member variables //////////////////
        vector<Particle> x_k_;  // states
        vector<Particle> x_pred_;  // states
        vector<PHDupdate> phd_updates_; // phd update components

        ////// Birth params
        mat mu_gamma_;  // States of new born particles
        int J_gamma_;  // New born particles' covariance
        mat kP_gamma; 

        ////// Spawn params
        int J_beta_;  // Number of partilces to spawn from each target.
        mat kP_beta;  // Covariance added to a spawned particle
        mat kweight_beta_P;

        ////// Motion update:
        double p_s_; // probability that a target still exists at time k given that its previous state.
                     // notice: currently state and time independent.
        mat F_; // Linearize motion model
        mat Q_; // Motion noise covariance.
            

        ////// Sensor update  (and those related to it)
        double p_d_; // def: probability of detection given a state x at time k.
                     // notice: currently state and time independent.
        mat H_;  // Sensor model. Currently assumed linear
        mat R_; // Detection covariance
        
        
        ////// Pruning & Merging
        double T_;  // Weight threshold. Particles with lower weights are pruned
        int J_max_; // Max particles: If there are more than that many particles, discard ones with lower weight
        int U_; // Merge distance threshold: Particles that are closer than this distance are merged.
        
        ////// Extraction
        double extraction_weight_threshold_;
};