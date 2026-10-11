#include <weather.hpp>

#include <string>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Weather::reset(){
    for (int i=0; i<6; i++){weather[i]=-1;}
    locnstr=" ";
}

std::string Weather::sendstring(){
    std::string sndstr="///WTHER///";
    sndstr+=locnstr+"///SPLIT///";
    for (int i=0; i<6; i++){sndstr+=std::to_string(weather[i])+"///SPLIT///";}
    return sndstr;
}

void Weather::update(){
    now = std::chrono::steady_clock::now();
    bool fail=false;
    for (int i=0; i<6; i++){if (weather[i]==-1){fail=true;}}
    if (last+600<std::chrono::duration<double>(now-start).count()||fail){
        if (req.done()){wformat(req.get());}
        if (req.wait()){
            req.request("https://api.open-meteo.com/v1/forecast?latitude="
                + std::to_string(locdata[0])
                + "&longitude=" + std::to_string(locdata[1])
                + "&hourly=precipitation_probability,temperature_2m,weather_code"
                + "&current=temperature_2m,weather_code,relative_humidity_2m"
                + "&timeformat=unixtime&wind_speed_unit=mph"
                + "&temperature_unit=fahrenheit&precipitation_unit=inch"
            );
        }
    }
}

void Weather::wformat(const std::string& response){
    if (response.empty()){return;}
    json data;
    try {
        data=json::parse(response);
    } catch (...) {
        return;
    }

    // Current Data
    auto current = data.find("current");
    if (current == data.end() || !current->is_object()){return;}

    // Temperature
    auto temp = current->find("temperature_2m");
    if (temp == current->end() || !temp->is_number()){return;}
    double value = temp->get<double>();
    int tmp=static_cast<int>(value + (value >= 0 ? 0.5 : -0.5));

    // Humidity
    auto humd = current->find("relative_humidity_2m");
    if (humd == current->end() || !humd->is_number()){return;}
    int hum = static_cast<int>(humd->get<double>() + 0.5);

    // Weather Code
    auto wcod = current->find("weather_code");
    if (wcod == current->end() || !wcod->is_number()){return;}
    int wco = static_cast<int>(wcod->get<double>() + 0.5);

    // Hourly Data
    auto hourly = data.find("hourly");
    if (hourly == data.end() || !hourly->is_object()){return;}

    auto times = hourly->find("time");
    if (times == hourly->end() || !times->is_array()){return;}

    long long fint = (std::time(nullptr) / 3600 + 1) * 3600;
    size_t i = 0;

    for (; i < times->size(); i++) {
        if (!(*times)[i].is_number()){return;}
        if ((*times)[i].get<long long>() >= fint){break;}
    }

    if (i == times->size()){return;}

    // Temperature
    auto temps = hourly->find("temperature_2m");
    if (temps == hourly->end() || !temps->is_array() || i >= temps->size()){return;}
    if (!(*temps)[i].is_number()){return;}
    value = (*temps)[i].get<double>();
    int htm = static_cast<int>(value + (value >= 0 ? 0.5 : -0.5));

    // Percipitation Probability
    auto pcips = hourly->find("precipitation_probability");
    if (pcips == hourly->end() || !pcips->is_array() || i >= pcips->size()){return;}
    if (!(*pcips)[i].is_number()){return;}
    int hpc = static_cast<int>((*pcips)[i].get<double>() + 0.5);
    
    // Weather Code
    auto wcods = hourly->find("weather_code");
    if (wcods == hourly->end() || !wcods->is_array() || i >= wcods->size()){return;}
    if (!(*wcods)[i].is_number()){return;}
    int hwc = static_cast<int>((*wcods)[i].get<double>() + 0.5);
    
    locnstr=locname;
    weather[0]=tmp;
    weather[1]=hum;
    weather[2]=wco;
    weather[3]=htm;
    weather[4]=hpc;
    weather[5]=hwc;
    now=std::chrono::steady_clock::now();
    last=std::chrono::duration<double>(now-start).count();
}