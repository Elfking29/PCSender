#pragma once

#include <thread>
#include <string>
#include <atomic>

class Bequests {
private:
    std::string result="";
    std::thread t;
    std::atomic<bool> finished{false};
    std::atomic<bool> waiting{true};


    static size_t write_callback(char* data, size_t size, size_t count, void* output);
    void internal(const std::string& url, const std::string& body, const std::string& token);
public:
    void request(const std::string& url, const std::string& token="");
    void post(const std::string& url, const std::string& body);
    bool done();
    bool wait();
    std::string get();
};