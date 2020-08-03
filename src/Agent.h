#ifndef AGENT
#define AGENT
 
class Agent
{
    private:
        int c1;
        int c2;

        // measured in big blinds 
        double bet;
        double chips;

        friend std::ostream& operator<<(std::ostream&, const Agent&);
     
    public:
        Agent(int card1, int card2, double blind);
     
        int get_c1() { return c1; };
        int get_c2() { return c2; };
        double get_bet() { return bet; };
        double get_chips() { return chips; };

        double action(double max_bet, double min_raise);
};
 
#endif
