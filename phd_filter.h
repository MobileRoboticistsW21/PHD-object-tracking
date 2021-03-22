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

class phd_filter{
    public:
        phd_filter(string);
        void run_filter();
    private:
        // Private functions
        double BirthWeight(vec);
        vector<Particle> CarBirth();
        Particle SpawnMotionModel(Particle);
        double SpawnWeight(vec,vec);
        Particle ExistingObjectMotionModel(Particle);
        tuple <PHDupdate,mat> UpdatePHDComponents(Particle);
        Particle ObjectMissedDetection(Particle);
        Particle DetectedObjectUpdate(Particle,vec,PHDupdate);
        void PositionSensor();
        void GroundTruth();
        void RCNNSensor();
        void NormalizeWeights();
        void PruningAndMerging();
        void MultiTargetStateExtraction();
        
        // Member variables
        int J_k_;
        int J_beta_;
        int J_gamma_;
        int sigma_v_;
        double p_s_;
        double p_d_;
        double T_;
        int U_;
        int J_max_;
        int i_;
        mat mu_gamma_;
        int t_steps_;
        vector<Particle> x_k_;
        vector<PHDupdate> phd_updates_;
        vector<Particle> x_pred_;
        mat z_;
        mat g_;
        int t_;
        mat F_{4,4};
        mat H_{4,4};
        vector<mat> bbox_;
        vector<vector<vec>> trajectory_;
        vector<vector<double>> trajectoryx_;
        vector<vector<double>> trajectoryy_;
        int type_;

        // Constant variables
        mat kInit_covP={{100,0,0,0},{0,100,0,0},{0,0,10,0},{0,0,0,10}};
        mat kP_gamma=diagmat(vec{100,100,25,25});
        mat kP_beta=diagmat(vec{50,50,10,10});
        mat kweight_beta_P=diagmat(vec{100,100,400,400});
        mat Q=join_cols(join_rows(1.25*eye<mat>(2,2), 2.5*eye<mat>(2,2)),join_rows(1.25*eye<mat>(2,2), 5*eye<mat>(2,2)));
        mat R=100*eye<mat>(2,2);

};