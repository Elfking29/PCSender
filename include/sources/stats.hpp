#include "sources.hpp"

class Stats : public Source {
private:
    int computer[5] = {-1,-1,-1,-1,-1};

    long long user = 0, nice = 0, system = 0, idle = 0;
    long long iowait = 0, irq = 0, softirq = 0, steal = 0;
    bool cpu_initialized = false;

public:
    std::string sendstring() override;
    void update() override;
    void reset() override;
};