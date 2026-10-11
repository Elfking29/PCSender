#include "sources.hpp"

#include <string>

class Music : public Source {
private:
    int pppvm[5]={-1,-1,-1,-1,-1};
    std::string taas[4]={" "," "," "," "};
    std::string players=" ";
    std::string name=" ";
    int manual=0;

public:
    std::string sendstring() override;
    void update() override;
    void reset() override;
};