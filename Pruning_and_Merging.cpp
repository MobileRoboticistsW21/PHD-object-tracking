#include"phd_filter.h"

void phd_filter::PruningAndMerging(){
    vector<Particle> pruned_set;
    vector<Particle> I;
    vector<Particle> I_diff;
    for(auto target:x_k_){
        if(target.weight>T_){
            I.push_back(target);
        }
    }
    int l=0;

    while(!I.empty()){
        l++;
        double max_weight=0;
        size_t j=0;
        for(size_t iter=0;iter<I.size();iter++){
            if(I[iter].weight>max_weight){
                max_weight=I[iter].weight;
                j=iter;
            }
        }
        vector<Particle> L;
        cout<<I[j].weight<<endl;
        for(auto iter:I){
           double Mah_d= as_scalar((iter.state-I[j].state).t()*
           iter.P.i()*(iter.state-I[j].state));
           if(Mah_d<U_){
               //cout<<iter.state<<" "<<I[j].state<<endl;
               L.push_back(iter);
           }
           else{
               I_diff.push_back(iter);
           }
        }
        Particle merged_target;
        merged_target.weight=0;
        merged_target.state=vec{0,0,0,0};
        merged_target.P=zeros<mat>(4,4);
        for(auto iter:L){
            merged_target.weight=merged_target.weight+iter.weight;
            merged_target.state=merged_target.state+iter.weight*iter.state;
        }
        merged_target.state=merged_target.state/merged_target.weight;
        for(auto iter:L){
             merged_target.P=merged_target.P+iter.weight*
             (iter.P+(merged_target.state-iter.state)*
             (merged_target.state-iter.state).t());
        }
        merged_target.P=merged_target.P/merged_target.weight;
        I.clear();
        I=I_diff;
        I_diff.clear();
        pruned_set.push_back(merged_target);
        //cout<<I.size()<<endl;
    }
    //cout<<"Done here"<<endl;
    if(pruned_set.size()>100){
        sort(pruned_set.begin(), pruned_set.end(), 
        [](const Particle &a, const Particle &b)->bool{
        return a.weight < b.weight;
        });
        vector<Particle> temp(pruned_set.end()-100,pruned_set.end());
        x_k_=temp;
    } 
    else{
        x_k_=pruned_set;
    }   
}

