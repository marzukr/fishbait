#ifndef AGENT
#define AGENT

#include <string>
 
class Agent
{
    private:
        int c1_;
        int c2_;
        std::string name_;

        // measured in big blinds 
        double bet_;
        double chips_;

        bool in_game_;

        friend std::ostream& operator<<(std::ostream&, const Agent&);
     
    public:
        Agent(double start_chips, std::string agent_name);
     
        int get_c1() { return c1_; };
        int get_c2() { return c2_; };
        double get_bet() { return bet_; };
        double get_chips() { return chips_; };

        double action(double max_bet, double min_raise);
        void assign_blind(double blind);
        void deal_cards(int card1, int card2);
};
 
#endif
