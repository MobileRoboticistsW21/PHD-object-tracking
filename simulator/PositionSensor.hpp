// #include"phd_filter.h"

#include<armadillo>

arma::mat PositionSensor(int t_){
    arma::mat z_ = arma::zeros<arma::mat>(2, 2);
    auto eye2 = arma::eye<arma::mat>(2,2);
    z_.row(0)=arma::mvnrnd(
        arma::vec{
            -250+12.7475*t_*0.9806,
            -250+12.7475*t_*-0.1961}, 10 * eye2).t(); 
    z_.row(1)=arma::mvnrnd(
        arma::vec{
            250+12.7475*t_*0.1961,
            250+12.7475*t_*-0.9806}, 10 * eye2).t();
    /*
    if(t_>66){ // Spsawned Particle Sensing
        z_=z_.rows(0,1); 
        z_.insert_rows(2,(mvnrnd(vec{417.4+12.7475*(t_-67)*-0.92,
        -587+12.7475*(t_-67)*-0.39},10*eye<mat>(2,2)).t()));
    }*/
    return z_;
}
 	 