#include<cmath>
#include<random>
#include<iostream>
#include<cstdlib>
#include <bitset>
#include<algorithm>
#include <time.h>
#include <string> 
#include <fstream>
#include <sstream>
#include <vector>
#include<parameter.h>
#define RANGE (UPPER_BOUND - LOWER_BOUND)
#define POP_SIZE  100
#define TOUR_SIZE  2
#define RAND_SEED 12345
#define Pc 0.3 
#define Pm 0.3
using namespace std;

//RV on (0,1)
inline float rand_p(){
    return  static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
//cost[i][j] cost costumer i to facility j
int mov_cost[N][M];
int build_cost[M];
//indexed_list[i][j]: the j th small cost facility index  for  costumer i
vector<int> indexed_list[N];


template< size_t size>
typename std::bitset<size> random_bitset( double p = 0.5) {

    typename std::bitset<size> bits;
    std::random_device rd;
    std::mt19937 gen( rd());
    std::bernoulli_distribution d( p);

    for( int n = 0; n < size; ++n) {
        bits[ n] = d( gen);
    }

    return bits;
}

class person{
    public:
    unsigned long long fitness;
    int gene_len;
    //need to be int(log(index)/log(2))
    bitset<M> gene;
    //int phenotype[N];

    unsigned long long obj_funtion(){
        int e = 0;//# of builded facility
        vector<int> builded_index;
        unsigned long long ans = 0;
        for (int j =0;j<M;++j){
            if(gene[j] == true){
                e++;
                ans += build_cost[j];
                builded_index.push_back(j);
            }
        }
        if(e>sqrt(M)){//facility is dense
            for(int i = 0;i<N;i++){
                int facility_index = 0;
                for(int  j =0;j<M;j++){
                    if( gene[  indexed_list[i][j] ] == true){
                        facility_index = indexed_list[i][j];
                        break;
                    }
                }
                ans += mov_cost[i][facility_index];
            }
        }
        else{//facility is sparse
            for(int i = 0;i<N;i++){
                //search for best facility
                int min_cost = 9999999;
                for(int j=0;j<e;++j){
                    int fac_idx = builded_index[j];
                    int cur_cost = mov_cost[i][fac_idx];
                    if( cur_cost < min_cost)
                        min_cost = cur_cost;
                }
                ans += min_cost;
            }
        }
        return ans;
    }
    person(){
        //randomly ini

        gene = random_bitset<M>( 0.5);
        fitness = obj_funtion();
        gene_len = M ;
    }
    //creat new person with a lover
    person CrossOver(person lover){
        person kid;
        int pt = (rand()%(gene_len-1));
        for(int i =0;i<gene_len;i++){
            if(i<=pt)
                kid.gene[i] = this->gene[i];
            else
                kid.gene[i] = lover.gene[i];
        }
        
        kid.fitness = kid.obj_funtion();
        return kid;
  
    }
    void mutate(){
        float pm= 1;
        pm = pm/gene_len;

        for(int j =0;j<gene_len;++j){
            float r =rand_p();
            if(r<pm){
                gene[j] = ~gene[j];
            }
        }
        fitness = obj_funtion();
        ;
    }

};
inline static bool cmp(person a,person b){
    return a.fitness < b.fitness;
}
class BinarySGA
{

public:
    int pop_size ;
    int termination;
    int cyc;
    person pop1[POP_SIZE];
    person pop2[POP_SIZE];
    person pop3[POP_SIZE];
    person *parent;
    person *child;
    person *new_gen;

    BinarySGA(int termination_size){
        parent = pop1;
        child = pop2;
        new_gen = pop3;
        termination = termination_size;
        pop_size = POP_SIZE;
        cyc =0;
    }
/*
    float variance(){
        float mean ;
        float var=0;
        for(int i =0;i<N;++i){
            mean= 0;

            for(int j =0;j<POP_SIZE;++j){
                mean += parent[j].phenotype[i];
            }
            mean /= POP_SIZE;
            float var_t=0;
            for(int j =0;j<POP_SIZE;++j){
                float dis = (parent[j].phenotype[i] -mean);
                var_t += dis*dis;
            }
            var += var_t/POP_SIZE ;
        }
        var = sqrt(var); 
        return var;
    }
*/

    int FindBest(){
        int ans=0;
        for(int i=0;i<pop_size;++i)
            if( parent[i].fitness < parent[ans].fitness)
                ans = i ;
        return ans;
    }
    
    //from child and parent select new generation
    void SurvivorSelection_E(){
        
        sort(parent,parent+pop_size,cmp);
        sort(child ,child +pop_size,cmp);
        int i_p=0;
        int i_c=0;
        for (int i =0;i<pop_size;++i){
            if(i_c<pop_size){
                if(parent[i_p].fitness > child[i_c].fitness){
                    new_gen[i] = child[i_c++];
                }
                else{
                    new_gen[i] = parent[i_p++];
                }
            }
            else{
                new_gen[i] = parent[i_p++];
            }

        }
        //update new gen as parent
        swap(parent,new_gen);
    }
    //randomly choose k and find the best amount them
    //return index
    int TournamentSelection(int k){
        int best_idx = 99;
        for(int i =0;i<k;++i){
            int choosen  =best_idx= rand()%pop_size;
            if(parent[choosen].fitness< parent[best_idx].fitness)
                best_idx = choosen;
        }
        return best_idx;
    }
    void SelectAndMate(float pc,int k){
        //parents are all of the population
        int i=0;
        while(i<pop_size-1){
            int a= TournamentSelection(k);
            int b= TournamentSelection(k);
            //float pd= parent[a].dis(parent+b);
            //float max_d = RANGE*RANGE*10;
            //pd = (max_d - pd) / max_d;
            if(rand_p()<pc ){
                child[i++] = parent[a].CrossOver(parent[b]);
                child[i++] = parent[b].CrossOver(parent[a]);
            }
            else{;
                child[i++] = parent[a];
                child[i++] = parent[b];
            }

        }

    }
    //apply mutate with P() pm on every child
    void mutate(float pm){
        for(int i=0;i<pop_size;++i){
            if(rand_p()<pm) 
                child[i].mutate();
        }
    }

    void report(){
        //if(cyc%199) return;
        int bestIdx = FindBest();
        cout<<"at cycle ["<<cyc<<"]\n";
        cout<<"best fitness is ["<<parent[bestIdx].fitness<<"]\n";
        cout<<"at X =[";
        for(int i=0;i<M;i++)
            cout<<parent[bestIdx].gene[i];
        cout<<"\n";
    }
    
};
int main(){
    int run_cyc =500;
    int num_run = 5;
    srand(time(0));
    std::ofstream outfile("GA_B.csv");
    std::ofstream outfile_v("GA_B_V.csv");
    for(int i =0;i<N;++i){
        for(int j=0;j<M;++j){
            cin >> mov_cost[i][j]; 
        }
    }
    for(int j=0;j<M;++j){
        cin>> build_cost[j];
    }
    for(int i =0;i<N;++i){
        vector<int> tmp ;
        for(int j =0;j<M;++j){
            int a = mov_cost[i][j];
            tmp.push_back(a);
        }
        vector<size_t> tmp_ = sort_indexes(tmp);
        indexed_list[i].assign( tmp_.begin(), tmp_.end());
    }

    for(int j =0;j<num_run;++j){

        srand(time(0));
        BinarySGA GA(run_cyc);
        for(int i =0;i<run_cyc;++i){
            GA.SelectAndMate(Pc,2);
            GA.mutate(Pm);
            GA.SurvivorSelection_E();
            GA.cyc ++;
            //GA.report();
            outfile <<GA.parent[GA.FindBest()].fitness<<",";
            //outfile_v <<GA.variance()<<",";
        }
        outfile<<"\n";
        outfile_v<<"\n";
        //cout<<"["<<j<<"] th run done\n";
    };
    outfile.close();
    outfile_v.close();

    return 0;

}



