extern "C" {
    #include "hand_index.h"
    #include "deck.h"
}
#include <iostream>

int iso_card_hr(int hr_card) {
    int res = 51 - hr_card;
    int rem = hr_card % 4;
    return res + (rem - 3 + rem);
}

int iso_card_omp(int omp_card) {
    int rem = omp_card % 4;
    if (rem == 2) return omp_card + 1;
    else if (rem == 3) return omp_card - 1;
    return omp_card;
}

int main() {
    // hand_indexer_t* isocalc = new hand_indexer_t();
    // const uint8_t cpr[] = {2,5};
    // hand_indexer_init(2, cpr, isocalc);
    // const uint8_t cards1[] = {0,4,8,12,16,20,24};
    // const uint8_t cards2[] = {4,0,8,12,16,20,24};
    // const uint8_t cards3[] = {0,8,4,12,16,20,24};
    // const uint8_t cards4[] = {0,4,12,8,16,20,24};
    // std::cout << hand_index_last(isocalc, cards1) << std::endl;
    // std::cout << hand_index_last(isocalc, cards2) << std::endl;
    // std::cout << hand_index_last(isocalc, cards3) << std::endl;
    // std::cout << hand_index_last(isocalc, cards4) << std::endl;
    for(int i = 0; i < 52; i++) {
        // int ic = iso_card(i);
        int ic = iso_card_omp(i);
        std::cout << RANK_TO_CHAR[deck_get_rank(ic)];
        std::cout << SUIT_TO_CHAR[deck_get_suit(ic)] << std::endl;
    }
}

