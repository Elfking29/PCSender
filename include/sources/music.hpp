#include "sources.hpp"

class Music : public Source {
private:
    

public:
    std::string sendstring() override;
    void update() override;
    void reset() override;
};