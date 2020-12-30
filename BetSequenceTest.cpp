#include "BetSequence.h"

int main(){
    float raise_sizes [1] = {1};
    char num_raise_sizes = 1;
    float starting_chips = 2;
    std::cout << GetNumPossible(raise_sizes, num_raise_sizes, starting_chips) << std::endl;
    return 0;
}