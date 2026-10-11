#include <string>
#include <thread>
#include <chrono>

#include <conman.hpp>


int main(){
    Conman usb;

    // Sources

    // Variables
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    double elapsed=-1;
    double watchdog=-1;
    double poke=-1;
    double update=-1;
    int item=0;

    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        now = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration<double>(now-start).count();


        if (usb.connected()){
            // Watchdog
            if (watchdog+5<elapsed){
                usb.disconnect();
            }

            // Poke
            if (poke+2<elapsed){
                poke=elapsed;
                usb.write("ALIVE");
            }

            // Handle Pico Input
            std::string line = usb.read();
            printf(("FROM PICO: "+line).c_str());
            if (line=="ALIVE"){watchdog=elapsed;}
            else {}

            // Update Sources
            //for (int i=0; i<sources.size(); i++){sources[i]->update();}

            // Handle Pico Output
            if (update+0.6<elapsed){
                update=elapsed;
                //usb.write(sources[item]->sendstring());
                //item+=1;
                //item%=sources.size();
            }

        }

        else {
            usb.connect();
            watchdog=elapsed;
            poke=-1;
            update=-1;
        }
    }

    return 0;
}