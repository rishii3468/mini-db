#include<sstream>
#include<map>
#include "structs.h"
#include "parser.h"

#include<algorithm>
#include<cctype>



Query parse(const std::string& input){
    std::stringstream ss(input);
    Query q;
    ss >> q.type;
    std::string token;
    while(ss >> token){
        auto pos = token.find("=");
        if(pos != std::string::npos){
            std::string key = token.substr(0,pos);
            std::string value = token.substr(pos+1);
            q.data[key] = value;
        }else{
            q.index_column = token;
        }
        if(token == "set"){
            while(ss >> token){
                auto pos = token.find("=");
                if(pos != std::string::npos){
                    std::string key = token.substr(0,pos);
                    std::string value = token.substr(pos+1);
                    q.newData[key] = value;
                }
            }
            break;
        }
    }
    return q;
}

