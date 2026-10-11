#include <stats.hpp>

#include <string>
#include <fstream>
#include <sys/statvfs.h>

std::string Stats::sendstring(){
    std::string sndstr="///STATS///";
    for (int i=0; i<5; i++){sndstr+=std::to_string(computer[i])+"///SPLIT///";}
    return sndstr;
}

void Stats::reset(){
    for (int i=0; i<5; i++){computer[i]=-1;}
    cpu_initialized=false;
}

void Stats::update(){
    std::ifstream file;

    // CPU First
    file.open("/proc/stat");
    std::string label;
    long long new_user, new_nice, new_system, new_idle;
    long long new_iowait, new_irq, new_softirq, new_steal;
    int cpu = -1;
    if (file >> label >> new_user >> new_nice >> new_system >> new_idle
        >> new_iowait >> new_irq >> new_softirq >> new_steal) {
        if (cpu_initialized) {
            long long total1 = user + nice + system + idle
                + iowait + irq + softirq + steal;
            long long total2 = new_user + new_nice + new_system + new_idle
                + new_iowait + new_irq + new_softirq + new_steal;
            long long idle1 = idle + iowait;
            long long idle2 = new_idle + new_iowait;
            long long total_delta = total2 - total1;
            long long idle_delta = idle2 - idle1;
            if (total_delta>0) {
                cpu = static_cast<int>(100.0*(total_delta-idle_delta)/total_delta+0.5);
            }
        }
        else{cpu=0;}

        // Save for next update
        user = new_user;
        nice = new_nice;
        system = new_system;
        idle = new_idle;
        iowait = new_iowait;
        irq = new_irq;
        softirq = new_softirq;
        steal = new_steal;
        cpu_initialized = true;
        }
    file.close();
    file.clear();

    // RAM
    file.open("/proc/meminfo");
    std::string name, unit;
    long long value;
    long long total = 0, available = 0;
    while (file >> name >> value >> unit) {
        if (name == "MemTotal:") total = value;
        else if (name == "MemAvailable:") available = value;
    }
    int ram=(total > 0)?static_cast<int>((100.0*(total - available)/total)+0.5):-1;
    file.close();
    file.clear();

    // Disk
    struct statvfs fs;
    int dsk = -1;

    if (statvfs("/", &fs) == 0 && fs.f_blocks > 0) {
        dsk = static_cast<int>(
            100.0 * (fs.f_blocks - fs.f_bfree) / fs.f_blocks + 0.5
        );
    }

    // Percentage
    file.open("/sys/class/power_supply/BAT0/capacity");
    int bat = -1;
    file >> bat;
    file.close();
    file.clear();

    // Charging
    file.open("/sys/class/power_supply/AC/online");
    int plg = -1;
    file >> plg;
    file.close();
    file.clear();

    // Set Data
    if (cpu!=-1&&ram!=-1&&dsk!=-1&&bat!=-1&&plg!=-1){
        computer[0]=cpu;
        computer[1]=ram;
        computer[2]=dsk;
        computer[3]=bat;
        computer[4]=plg;
    }
    else {for (int i=0; i<5; i++){computer[i]=-1;}}
}
