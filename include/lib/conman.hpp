#pragma once

#include <string>
#include <vector>

#include <termios.h>

class Conman{
    private:
        int dev=-1;
        bool con=false;
        std::string buffer="";

        std::string port();
    public:
        Conman();

        void disconnect();
        void connect();
        bool connected() const;

        std::string read();
        void write(std::string s);


};