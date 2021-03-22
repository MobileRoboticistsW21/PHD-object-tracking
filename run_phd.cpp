#include "phd_filter.h"
#include "matplotlibcpp.h"
// #include "matplotlib-cpp-master/matplotlibcpp.h"
namespace plt = matplotlibcpp;
phd_filter::phd_filter(string type)
{
    if (type.compare("simulation") == 0)
    {
        type_ = 0;
        J_k_ = 2;
        J_beta_ = 2;
        J_gamma_ = 2;
        sigma_v_ = 5;
        p_s_ = 0.99;
        p_d_ = 0.98;
        T_ = 0.00001;
        U_ = 4;
        J_max_ = 100;
        i_ = 0;
        mu_gamma_ = join_rows(vec{250, 250, 0, 0}, vec{-250, -250, 0, 0});
        t_steps_ = 100;
        Particle p;
        p.state = {250, 250, 0, 0};
        p.P = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
        p.weight = 0.5;
        x_k_.push_back(p);
        p.state = {-250, -250, 0, 0};
        x_k_.push_back(p);
        F_ = join_cols(join_rows(eye<mat>(2, 2), eye<mat>(2, 2)),
                       join_rows(zeros<mat>(2, 2), eye<mat>(2, 2)));
        ;
        z_ = zeros<mat>(2, 2);
        g_ = zeros<mat>(2, 2);
        H_ = join_rows(eye<mat>(2, 2), zeros<mat>(2, 2));
    }
}
void phd_filter::run_filter()
{
    //plt::figure_size(1000,1000);
    //plt::xlim(-500,1200);
    //plt::ylim(-1200,500);
    for (t_ = 1; t_ < t_steps_; t_++)
    {
        i_ = 0;
        x_pred_.clear();
        phd_updates_.clear();
        Particle pred_target;
        // Simulation
        // Birth
        /*
        for(int it1=0;it1<J_gamma_;it1++){
            for(auto it2:x_k_){
                i_++;
                pred_target.weight=BirthWeight(it2.state);
                pred_target.state=mu_gamma_.col(it1); // make it constant
                pred_target.P=kP_gamma;
                x_pred_.push_back(pred_target);
            }
        }
        */
        // Spawning
        /*
        for(int it3=0;it3<J_beta_;it3++){
            for(auto it4:x_k_){
                i_++;
                pred_target=SpawnMotionModel(it4);
                pred_target.weight=SpawnWeight(pred_target.state,
                it4.state)*it4.weight;
                x_pred_.push_back(pred_target);
            }
        }
        */
        // Existing Object Motion Model
        for (auto it5 : x_k_)
        {
            i_++;
            pred_target = ExistingObjectMotionModel(it5);
            x_pred_.push_back(pred_target);
        }
        int J_k_pred = i_;

        // Update PHD Components
        for (auto it6 : x_pred_)
        {
            tuple<PHDupdate, mat> target_update;
            target_update = UpdatePHDComponents(it6);
            phd_updates_.push_back(get<0>(target_update));
            it6.P = get<1>(target_update);
        }

        // Missed Detections
        x_k_.clear();
        for (auto it7 : x_pred_)
        {
            x_k_.push_back(ObjectMissedDetection(it7));
        }

        x_k_.clear(); ////////
        //Sensor Readings
        PositionSensor();
        GroundTruth();

        for (int it8 = 0; it8 < z_.n_rows; it8++)
        {
            for (int it9 = 0; it9 < x_pred_.size(); it9++)
            {
                x_k_.push_back(DetectedObjectUpdate(x_pred_[it9], vectorise(z_.row(it8)), phd_updates_[it9]));
            }
        }

        NormalizeWeights();
        PruningAndMerging();
        MultiTargetStateExtraction();

        J_k_ = x_k_.size();
        //Plotting
        for (int iter = 0; iter < x_k_.size(); iter++)
        {
            if (iter < trajectory_.size())
            {
                trajectory_[iter].push_back(x_k_[iter].state);
                trajectoryx_[iter].push_back(x_k_[iter].state[0]);
                trajectoryy_[iter].push_back(x_k_[iter].state[1]);
            }
            else
            {
                trajectory_.push_back(vector<vec>({x_k_[iter].state}));
                trajectoryx_.push_back(vector<double>({x_k_[iter].state[0]}));
                trajectoryy_.push_back(vector<double>({x_k_[iter].state[1]}));
            }
        }
        for (int i = 0; i < trajectory_.size(); i++)
        {
            plt::clf();
            plt::plot(trajectoryx_[i], trajectoryy_[i], "b*");
        }
        cout << x_k_.size() << endl;
        plt::pause(0.0001);
        //    plt::close();
    }
}
int main()
{
    phd_filter a("simulation");
    a.run_filter();
    return 0;
}