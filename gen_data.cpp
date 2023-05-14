#include<parameter.h>
#include <sstream>
#include <random>
#include<iostream>
inline float rand_p(){
    return  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
using namespace std;
int main(){
    int range = UPPER_BOUND -LOWER_BOUND;
    for (int i =0;i<N;i++){
        for(int j=0;j<M;++j){
            int offset = int(range*rand_p());
            cout<<LOWER_BOUND + offset<<" ";
        }
        
    }
    for(int j=0;j<M;++j){
        int offset = int(range*rand_p());
            cout<<(LOWER_BOUND + offset*build_ratio) <<" ";
    }
}
