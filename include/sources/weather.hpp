#include "sources.hpp"

class Weather : public Source {
public:
    std::string sendstring() override;
    void update() override;
    void reset() override;
};