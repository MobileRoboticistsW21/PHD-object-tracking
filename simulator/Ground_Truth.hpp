#include<armadillo>

arma::mat GroundTruth(int t_)
{
    arma::mat g_ = arma::zeros<arma::mat>(2, 2);

    g_.row(0) = arma::vec{
        -250 + 12.7475 * t_ * 0.9806,
        -250 + 12.7475 * t_ * -0.1961
        }.t(); 
    g_.row(1) = arma::vec{
        250 + 12.7475 * t_ * 0.1961,
        250 + 12.7475 * t_ * -0.9806
        }.t();

    if(t_>66)
    { // Spawned Particle Sensing
        g_ = g_.rows(0,1); 
        g_.insert_rows(1,(
                arma::vec{
                    417.4 + 12.7475 * (t_ - 67) * -0.92,
                    -587 + 12.7475 * (t_ - 67) * -0.39
                }
            ).t()
        );
    }
    return g_;
}
 	 