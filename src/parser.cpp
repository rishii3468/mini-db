#include<sstream>
#include<map>
#include "structs.h"
#include "parser.h"

#include<algorithm>
#include<cctype>

using namespace std;



Query parse(const string& input){
    stringstream ss(input);
    Query q;
    ss >> q.type;
    string token;
    while(ss >> token){
        auto pos = token.find("=");
        if(pos != string::npos){
            string key = token.substr(0,pos);
            string value = token.substr(pos+1);
            q.data[key] = value;
        }else{
            q.index_column = token;
        }
        if(token == "set"){
            while(ss >> token){
                auto pos = token.find("=");
                if(pos != string::npos){
                    string key = token.substr(0,pos);
                    string value = token.substr(pos+1);
                    q.newData[key] = value;
                }
            }
            break;
        }
    }
    return q;
}

