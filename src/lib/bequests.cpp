#include <bequests.hpp>

#include <curl/curl.h>
#include <cstddef>

bool Bequests::done(){return finished;}
bool Bequests::wait(){return waiting;}

std::string Bequests::get(){
    if (!finished){return "";}
    if (t.joinable()){t.join();}
    finished=false;
    return result;
}

void Bequests::request(const std::string& url){
    if (waiting){
        waiting=false;
        if (t.joinable()){t.join();}
        t=std::thread(&Bequests::internal,this,url);
    }
}

size_t Bequests::write_callback(char* data, size_t size, size_t count, void* output) {
    size_t bytes = size * count;
    static_cast<std::string*>(output)->append(data, bytes);
    return bytes;
}

void Bequests::internal(const std::string& url){
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Bequests::write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            response.clear();
        }

        curl_easy_cleanup(curl);
    }

    result=response;
    finished=true;
    waiting=true;
}