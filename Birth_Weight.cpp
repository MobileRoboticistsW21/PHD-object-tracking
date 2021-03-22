#include"phd_filter.h"

double phd_filter::BirthWeight(vec current_state){
    double birth_weight=norm(0.1*normpdf(current_state,mu_gamma_.col(0),
    diagvec(kP_gamma))+0.1*normpdf(current_state,mu_gamma_.col(1),
    diagvec(kP_gamma)));
    return birth_weight;
}