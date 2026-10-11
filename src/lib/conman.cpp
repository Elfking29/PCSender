#include "conman.hpp"

#include <string>

#include <termios.h>
#include <libudev.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <ctime>
#include <thread>
#include <chrono>

Conman::Conman(){}

void Conman::disconnect(){
    con = false;
    if (dev != -1) {
        ::close(dev);
        dev = -1;
    }
    buffer.clear();
}

bool Conman::connected() const{return con;}

std::string Conman::read(){

    // Drain USB Buffer
    char buf[1024];
    ssize_t n = ::read(dev, buf, sizeof(buf));

    // Append to internal buffer
    if (buffer.size()>1024){buffer.clear();}
    if (n>0){buffer.append(buf,n);}

    // Return 1 line
    if (buffer.size()>0){
        size_t pos = buffer.find('\n');
        if (pos != std::string::npos) {
            std::string sub = buffer.substr(0, pos);
            buffer.erase(0, pos + 1);
            return sub + "\n";
        }
    }

    return "";
}

void Conman::write(std::string s){
    if (dev==-1){return;}

    s += '\n';

    ssize_t n = ::write(dev, s.c_str(), s.size());

    if (n==-1) {perror("write");}
    else if (static_cast<size_t>(n)<s.size()){
        fprintf(stderr, "Partial write: %zd of %zu bytes\n", n, s.size());
    }
}

std::string Conman::port(){
    udev* ctx = udev_new();
    if (!ctx) return "";

    udev_enumerate* en = udev_enumerate_new(ctx);
    if (!en) {
        udev_unref(ctx);
        return "";
    }

    udev_enumerate_add_match_subsystem(en, "tty");
    udev_enumerate_scan_devices(en);

    udev_list_entry* devices = udev_enumerate_get_list_entry(en);
    udev_list_entry* entry;
    std::string port;

    udev_list_entry_foreach(entry, devices) {
        udev_device* dev = udev_device_new_from_syspath(
            ctx, udev_list_entry_get_name(entry));

        for (udev_device* usb = dev; usb;
             usb = udev_device_get_parent(usb)) {
            const char* vid = udev_device_get_sysattr_value(usb, "idVendor");
            const char* pid = udev_device_get_sysattr_value(usb, "idProduct");

            if (vid && pid &&
                std::string(vid) == "2e8a" &&
                std::string(pid) == "000a") {
                const char* node = udev_device_get_devnode(dev);
                if (node) port = node;
                break;
            }
        }

        udev_device_unref(dev);
        if (!port.empty()) break;
    }

    udev_enumerate_unref(en);
    udev_unref(ctx);
    return port;
}

void Conman::connect(){
    while (con==false){
        std::this_thread::sleep_for(std::chrono::milliseconds(750));
        while (true){
            //Sleep at some point

            // Get Port
            std::string p=port();
            if (p.empty()){break;}

            dev = ::open(p.c_str(), O_RDWR|O_NOCTTY|O_NONBLOCK);
            if (dev==-1){perror("open");disconnect();break;}
      
            // Configure Port
            termios settings{};

            if (tcgetattr(dev, &settings) == -1) {
                perror("tcgetattr");
                disconnect();
                break;
            }

            cfmakeraw(&settings);

            cfsetispeed(&settings, B115200);
            cfsetospeed(&settings, B115200);

            settings.c_cflag |= CLOCAL | CREAD;
            settings.c_cflag &= ~CSTOPB;
            settings.c_cflag &= ~PARENB;
            settings.c_cflag &= ~CSIZE;
            settings.c_cflag |= CS8;

            if (tcsetattr(dev, TCSANOW, &settings) == -1) {
                perror("tcsetattr");
                disconnect();
                break;
            }
            
            // Begin Communications
            std::time_t now = std::time(nullptr);
            std::tm t = *std::localtime(&now);
            std::string sndstr="///TIMED///";
            sndstr+=std::to_string(t.tm_year+1900)+"///SPLIT///";
            sndstr+=std::to_string(t.tm_mon+1)+"///SPLIT///";
            sndstr+=std::to_string(t.tm_mday)+"///SPLIT///";
            sndstr+=std::to_string(t.tm_hour)+"///SPLIT///";
            sndstr+=std::to_string(t.tm_min)+"///SPLIT///";
            sndstr+=std::to_string(t.tm_sec)+"///SPLIT///";
            sndstr+=std::to_string((t.tm_wday+6)%7)+"///SPLIT///";
            tcflush(dev, TCIOFLUSH);
            write(sndstr);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::string line = read();
            if (line.find("PICO\r\n")!=std::string::npos){
                write("///CONFD///");
                con=true;
                break;
            }
            else {disconnect();break;}
        }
    }
}