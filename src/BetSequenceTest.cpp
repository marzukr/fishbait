#include "BetSequence.h"

int main(){
    //add 0s or whatever for the columns you don't want to use
    // float raise_sizes [4][14] = {{0.25, 0.35, 0.45,0.55,0.65,0.75,0.85,0.95,1.05,1.15,1.25,1.35,1.45,1.55},
    //                              {0.25,0.5,0.75,1,1.5,0,0,0,0,0,0,0,0,0},
    //                              {0.5,1,0,0,0,0,0,0,0,0,0,0,0,0},
    //                              {0.5,1,0,0,0,0,0,0,0,0,0,0,0,0}};
    // //number of raise sizes for each round
    // float num_raise_sizes [4] = {14,5,2,2};
    float raise_sizes [1][1] = {{2}};
    float num_raise_sizes [1] = {1};
    float starting_chips = 100;
    std::cout << GetNumPossible(raise_sizes, num_raise_sizes, starting_chips) << std::endl;
    return 0;
}