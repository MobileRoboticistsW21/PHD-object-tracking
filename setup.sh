#! /bin/bash

# install armadillo and requirements
sudo apt-get install -y  libhdf5-dev 
sudo apt-get install -y libopenblas-dev liblapack-dev libarpack2-dev libsuperlu-dev 

sudo apt-get install wget
wget https://netactuate.dl.sourceforge.net/project/arma/armadillo-10.3.0.tar.xz
tar -xf ./armadillo-10.3.0.tar.xz
cd armadillo-10.3.0
mkdir build
cd build
cmake ..
make
sudo make install
cd ../ && sudo rm -rf armadillo*

# Setup matplotlib-cpp
pip3 install matplotlib
wget https://raw.githubusercontent.com/lava/matplotlib-cpp/master/matplotlibcpp.h


## Failed to get this to work
## pickle tools 
# wget http://www.picklingtools.com/PicklingTools170Release.tar.gz

# For data loading
sudo apt-get install libjsoncpp-dev