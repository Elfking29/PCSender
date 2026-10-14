#include <rss.hpp>

#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void Rss::reset(){
    token.clear();
    sndstr.clear();
}

std::string Rss::sendstring(){
    return "";
}

void Rss::update(){
    now = std::chrono::steady_clock::now();
    bool fail=false;
    for (int i=0; i<6; i++){if (sndstr.empty()){fail=true;}}
    if (last+600<std::chrono::duration<double>(now-start).count()||fail){
        if (req.done()){
            if (token.empty()){tokenize(req.get());}
            else {handle(req.get());}
        }
        if (req.wait()){
            if (token.empty()){
                req.post(std::string(Secrets::url)+"/api/greader.php/accounts/ClientLogin","Email=" + std::string(Secrets::usr)+"&Passwd=" + std::string(Secrets::pas));
            }
            else {
                req.request(std::string(Secrets::url)+"/api/greader.php/reader/api/0/stream/contents/reading-list?xt=user/-/state/com.google/read&output=json",token);
            }
        }
    }
}

void Rss::handle(std::string res) {
    if (res.empty()) {return;}
    if (res=="Unauthorized!"){reset();return;}
    try {
        json data = json::parse(res);

        if (!data.contains("items") || !data["items"].is_array()) {
            return;
        }

        std::vector<std::string> articles;

        for (const auto& item : data["items"]) {
            if (item.is_object() &&
                item.contains("title") &&
                item["title"].is_string()) {
                articles.push_back(item["title"].get<std::string>());
            }
        }

        sndstr="///RSSFD///"+std::to_string(articles.size())+"///SPLIT///";

        for (const auto& article : articles) {
            if (sndstr.size()+article.size()+11>1024){break;}
            sndstr += article+"///SPLIT///";
        }
    }
    catch (...) {}
}

void Rss::tokenize(std::string res){
    if (res.empty()){return;}
    size_t pos = res.find("Auth=");
    if (pos == std::string::npos) {return;}
    pos += 5;
    size_t end = res.find('\n', pos);
    token = res.substr(pos, end - pos);
}