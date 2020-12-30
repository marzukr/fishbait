#include "BetSequence.h"

int main(){
    //add 0s or whatever for the columns you don't want to use
    float raise_sizes [4][14] = {{0.1, 0.2, 0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,1.1,1.2,1.3,1.4},{0.2,0.4,0.6,0.8,1,1.2,1.4,0,0,0,0,0},{0.5,1,0,0,0,0,0,0,0,0,0,0,0,0},{0.5,1,0,0,0,0,0,0,0,0,0,0,0,0}};
    //number of raise sizes for each round
    float num_raise_sizes [4] = {14,7,2,2};
    float starting_chips = 100;
    std::cout << GetNumPossible(raise_sizes, num_raise_sizes, starting_chips) << std::endl;
    return 0;
}