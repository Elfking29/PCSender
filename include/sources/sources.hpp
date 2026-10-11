#pragma once

#include <string>

class Source {
public:
    virtual ~Source() = default;
    virtual std::string sendstring() = 0;
    virtual void update() = 0;
    virtual void reset() = 0;
};