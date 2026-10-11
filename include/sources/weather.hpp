#include "sources.hpp"

#include <chrono>

#include <bequests.hpp>

class Weather : public Source {
private:
    std::chrono::steady_clock::time_point start=std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now=std::chrono::steady_clock::now();
    double last=-1;

    Bequests req;
    int weather[6]={-1,-1,-1,-1,-1,-1};
    std::string locnstr=" ";

    //std::string locname="Aston|PA";
    //float locdata[2]={39.87428,-75.44825};

    std::string locname="Potsdam|NY";
    float locdata[2]={44.66978,-74.98131};

    void wformat(const std::string& response);
public:
    std::string sendstring() override;
    void update() override;
    void reset() override;
};