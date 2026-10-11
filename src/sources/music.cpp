#include <music.hpp>

#include <string>
#include <cstdio>
#include <charconv>

void Music::reset(){
    for (int i=0; i<5; i++){pppvm[i]=-1;}
    for (int i=0; i<4; i++){taas[i]=" ";}
    players.clear();
    name.clear();
    manual=0;
}

std::string Music::sendstring(){
    std::string sndstr="///MUSIC///";
    for (int i=0; i<4; i++){
        sndstr+=taas[i]+"///SPLIT///";
    }
    sndstr+=std::to_string(pppvm[0])+"///SPLIT///";
    sndstr+=std::to_string(pppvm[1])+"///SPLIT///";
    sndstr+=players+"///SPLIT///";
    sndstr+=std::to_string(pppvm[2])+"///SPLIT///";
    sndstr+=std::to_string(pppvm[3])+"///SPLIT///";
    sndstr+=std::to_string(pppvm[4])+"///SPLIT///";
    return sndstr;
}

void Music::update(){
    // Player List
    players.clear();
    char buffer[1024];
    FILE* pipe = popen("playerctl -l 2>/dev/null", "r");
    if (pipe != nullptr){
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr){
            players += buffer;
        }
        pclose(pipe);
    }

    size_t pos;
    while ((pos = players.find('\n')) != std::string::npos){
        players.replace(pos, 1, "///PSPLT///");
    }
    
    // Manual & Name
    if (players!=""){
        int num=0;
        std::string playercopy=players;
        pos=0;
        while ((pos=playercopy.find("///PSPLT///"))!=std::string::npos){
            playercopy.erase(0,pos+11);
            num++;
        }
        manual=((manual%(num+1))+(num+1))%(num+1);
        pppvm[4]=manual;
        if (manual<=0){name="%any";}
        else{
            playercopy=players;
            for (int i=1; i<manual; i++){
                playercopy.erase(0,playercopy.find("///PSPLT///")+11);
            }
            name=playercopy.substr(0,playercopy.find("///PSPLT///"));
        }
    }
    else {reset();return;}

    // Get Data

    std::string command =
    "playerctl metadata --format "
    "'{{playerInstance}}///SPLIT///{{xesam:title}}///SPLIT///"
    "{{xesam:album}}///SPLIT///{{xesam:artist}}///SPLIT///"
    "{{status}}///SPLIT///{{position}}///SPLIT///{{mpris:length}}///SPLIT///' "
    "-p \"" + name + "\" 2>/dev/null";

    std::string data;
    pipe = popen(command.c_str(), "r");
    if (pipe != nullptr){

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr){
            data += buffer;
        }
        pclose(pipe);

    
        // Handle Data
        int i=0;
        std::string sub;
        while ((pos=data.find("///SPLIT///"))!=std::string::npos){
            if (i>6){reset();return;}
            sub=data.substr(0,pos);
            if (i==0){
                size_t jos=0;
                pppvm[2]=-1;
                int j=0;
                std::string playercopy=players;
                while ((jos=playercopy.find("///PSPLT///"))!=std::string::npos){
                    if (playercopy.substr(0,jos)==sub){pppvm[2]=j;break;}
                    else{playercopy.erase(0,jos+11);}
                    j++;
                }
            }
            else if (i<5){taas[i-1]=sub;}
            else {
                long long work=0;
                if (!sub.empty()){
                    auto result=std::from_chars(sub.data(),sub.data()+sub.size(),work);
                    if (result.ec!=std::errc()||result.ptr!=sub.data()+sub.size())
                        {reset();return;}
                }
                pppvm[i-5]=static_cast<int>(work/1000000.0+0.5);
            } 
            data.erase(0,pos+11);
            i++;
        }
        if (i!=7){reset();return;}

        // Volume
        pipe = popen("wpctl get-volume @DEFAULT_AUDIO_SINK@ 2>/dev/null", "r");
        std::string output;
        if (pipe != nullptr){
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr){
                output += buffer;
            }
            pclose(pipe);
        }

        pos=output.find_last_not_of(" \t\r\n");
        if (pos==std::string::npos){reset();return;}
        output.erase(pos+1);        
        pos=output.find(' ');
        if (pos!=std::string::npos){
            float value;
            auto result=std::from_chars(output.data()+pos+1, output.data()+output.size(), value);
            if (result.ec!=std::errc()||result.ptr!=output.data()+output.size())
                {reset();return;}
            pppvm[3]=static_cast<int>(value*100);
        }
        else{reset();return;}
    }
    else {reset();return;}
}
