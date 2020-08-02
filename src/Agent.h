#ifndef AGENT
#define AGENT
 
class Agent
{
    private:
        int c1;
        int c2;

        friend std::ostream& operator<<(std::ostream&, const Agent&);
     
    public:
        Agent(int card1, int card2) : c1(card1), c2(card2) {};
     
        int get_c1() { return c1; };
        int get_c2() { return c2; };

        int action();
};
 
#endif
