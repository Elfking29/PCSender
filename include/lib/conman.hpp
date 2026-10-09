#pragma once

#include <string>
#include <vector>

#include <termios.h>

class Conman{
    private:
        termios tty{};
        bool con=false;
        std::string buffer="";

        int port();
    public:
        Conman();

        void disconnect();
        void connect();
        bool connected();

        std::string read();
        void send(std::string s);


};