#include "sources.hpp"
#include "secrets.hpp"

#include <chrono>

#include <bequests.hpp>

class Rss : public Source {
private:
    std::chrono::steady_clock::time_point start=std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now=std::chrono::steady_clock::now();
    double last=-1;

    Bequests req;

    std::string token="";
    std::string sndstr="";

    void handle(std::string res);
    void tokenize(std::string res);

public:
    std::string sendstring() override;
    void update() override;
    void reset() override;
};