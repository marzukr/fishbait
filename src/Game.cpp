#include <iostream>
#include <random>
#include "Game.h"

// Agent::Agent(int card1, int card2, double blind) {
//     c1 = card1;
//     c2 = card2;
//     bet = blind;
//     chips = 100 - blind;
// }
// 
// // -1: fold
// //  any other amount is a bet. if the value is the same as max_bet, then it is
// //  a check/call, otherwise a raise.
// //
// //  this agent is dumb and will check/fold, check/call, and raise with equal
// //  probability. when raising, it will always min raise.
// double Agent::action(double max_bet, double min_raise) {
//     std::random_device dev;
//     std::mt19937 rng(dev());
//     std::uniform_int_distribution<std::mt19937::result_type> dist6(0,2);
//     int decision = dist6(rng);
//     
//     // check/fold
//     if (decision == 0) {
//         if (max_bet > bet) return -1; // fold
//         return bet; // check
//     }
//     // check/call
//     else if (decision == 1) {
//         if (max_bet > bet) { // call
//             chips = chips - (max_bet - bet);
//             bet = max_bet;
//             return bet;
//         }
//         return bet; // check
//     }
//     // raise
//     else {
//         chips = chips - (max_bet - bet) - min_raise;
//         bet = max_bet + min_raise;
//         return bet;
//     }
// }
// 
// std::ostream& operator<<(std::ostream &strm, const Agent &a) {
//     return strm << "Agent(" << a.c1 << ", " << a.c2 << ", " << a.bet << ")";
// }
