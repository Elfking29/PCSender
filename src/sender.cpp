#include <string>
#include <array>
#include <thread>
#include <chrono>

#include <conman.hpp>

#include <stats.hpp>
#include <music.hpp>
#include <weather.hpp>
#include <rss.hpp>


int main(){
    Conman usb;

    // Sources
    Stats stats;
    Music music;
    Weather weather;
    Rss rss;

    std::array<Source*, 4> sources = {
        &stats,
        &music,
        &weather,
        &rss
    };


    // Variables
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    double elapsed=-1;
    double watchdog=-1;
    double poke=-1;
    double update=-1;
    int item=0;

    while (true){

        // Time Stuff
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration<double>(now-start).count();


        if (usb.connected()){
            // Watchdog
            if (watchdog+5<elapsed){
                printf("WATCHDOG\n");
                usb.disconnect();
                for (int i=0; i<sources.size(); i++){sources[i]->reset();}
            }

            // Poke
            if (poke+2<elapsed){
                poke=elapsed;
                usb.write("ALIVE");
            }

            // Handle Pico Input
            std::string line = usb.read();
            if (line!=""){
                printf(("FROM PICO: "+line).c_str());
                if (line=="ALIVE\r\n"){watchdog=elapsed;}
                else {}
            }

            // Handle Pico Output
            if (update+0.6<elapsed){
                update=elapsed;
                sources[item]->update();
                usb.write(sources[item]->sendstring());
                item+=1;
                item%=sources.size();
            }

        }

        else {
            printf("DISCONNECT\n");
            usb.connect();
            printf("CONNECT\n");
            now = std::chrono::steady_clock::now();
            elapsed = std::chrono::duration<double>(now-start).count();
            watchdog=elapsed;
            poke=-1;
            update=-1;
        }
    }

    return 0;
}