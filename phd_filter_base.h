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
   vec state; // 4x1 vector [x, y, Vx, Vy]
   mat P    ; // 4x4 covariance
   double weight = 0; // particle weight
};

struct PHDupdate
{
    vec eta; // Projected detection
    mat S  ;  // Projected sensor covariance
    mat K  ;  // Kalman gain
    mat P  ;  // Updated state covariance
};

// Notice (priorty Medium): This should house the settings needed for the filter eventually. 
struct PHD_filter_parameters
{};

class PhdFilterBase{
    public:
        PhdFilterBase();  // Notice (priority low): currently this hardcodes settings, that should be changed
        
        /**
         * TODO: be specific about what's needed as input. 
         */
        void update(const mat& detections);

        /**
         * NOTICE: (priority Low): not ideal, we should remove it.
         */
        vector<Particle> get_x_k_()
        { 
            return x_k_; 
        }  
      
        /**
         * NOTICE: (priority low): simple and seems to be functioning fine
         * @def get targets from tracked particles. 
         * @return particles with weight higher than a set threashold.
         */
        vector<Particle> extract_target_states();
        
        // TODO: make this local and have it derive a default from x_k_ 
        virtual Particle get_default_particle() = 0;

    protected:

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

        /**
         * NOTICE: (priority low): simple and seems to be functioning fine
         * @def Normalizes all particle weights
         */
        void NormalizeWeights(); 
        
        /**
         * NOTICE: (priority low): simple and seems to be functioning fine
                   BUT: the function can be sped up using a priority queue and less copying.
         * @def Merges particles of low manhalanobis distance proximity. 
         *      If many particles remain (configurable) the ones with lower weight are discarded. 
         */
        void PruningAndMerging();

        /**
         * NOTIE: THIS CURRENTLY is not being used. 
         */
        vector<Particle> propose_particles_with_missing_detections();

        // spawn related methods
        void propose_spawned_targets(void); // Notice (priority Medium-high) Currently not implemented
        virtual Particle spawn_particle(const Particle& x) = 0;
        // Particle SpawnMotionModel(Particle);
        // double SpawnWeight(vec,vec);
        
        void propose_new_born_targets(void); // Notice (priority Medium-high)Currently not implemented
        virtual Particle propose_new_born_particle(int i) = 0;
        // double BirthWeight(vec);

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
        mat mu_gamma_;
        int J_gamma_;
        mat kP_gamma;

        ////// Spawn params
        int J_beta_;  // Number of partilces to spawn from each target.
        mat kP_beta;
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

        // /////// unsorted
        // int J_k_;
        // int sigma_v_;
        // // Constant variables
        // const mat kInit_covP={{100,0,0,0},{0,100,0,0},{0,0,10,0},{0,0,0,10}};
        
};