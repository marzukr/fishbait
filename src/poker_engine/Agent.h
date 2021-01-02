#ifndef AGENT
#define AGENT

#include <string>
 
class Agent
{
    private:
        int c1;
        int c2;
        std::string name;

        // measured in big blinds 
        double bet;
        double chips;

        friend std::ostream& operator<<(std::ostream&, const Agent&);
     
    public:
        Agent(double start_chips, std::string agent_name);
     
        int get_c1() { return c1; };
        int get_c2() { return c2; };
        double get_bet() { return bet; };
        double get_chips() { return chips; };

        double action(double max_bet, double min_raise);
        void assign_blind(double blind);
        void deal_cards(int card1, int card2);
};
 
#endif
