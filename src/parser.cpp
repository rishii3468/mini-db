#include<sstream>
#include<map>
#include "structs.h"
#include "parser.h"
#include "storage.h"

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

bool validate(const Query& q){
    auto headers = getHeaders();
    
    // Validate query type
    if(q.type.empty()){
        std::cout<<"Error: Query type cannot be empty."<<std::endl;
        return false;
    }
    
    std::string query_type = q.type;
    std::transform(query_type.begin(), query_type.end(), query_type.begin(), ::tolower);
    
    if(query_type != "insert" && query_type != "select" && 
       query_type != "update" && query_type != "delete"){
        std::cout<<"Error: Invalid query type '"<<q.type<<"'. Expected INSERT, SELECT, UPDATE, or DELETE."<<std::endl;
        return false;
    }
    
    // INSERT validation
    if(query_type == "insert"){
        // Check all headers have values
        for(auto& header : headers){
            if(!q.data.count(header)){
                std::cout<<"Error: Values for all attributes required. Missing: "<<header<<std::endl;
                return false;
            }
        }
        
        // Check no extra fields are provided
        for(auto& field : q.data){
            bool found = false;
            for(auto& header : headers){
                if(field.first == header){
                    found = true;
                    break;
                }
            }
            if(!found){
                std::cout<<"Error: Unknown attribute '"<<field.first<<"'."<<std::endl;
                return false;
            }
        }
        
        // Check values are not empty
        for(auto& field : q.data){
            if(field.second.empty()){
                std::cout<<"Error: Value for attribute '"<<field.first<<"' cannot be empty."<<std::endl;
                return false;
            }
        }
    }
    
    // UPDATE validation
    else if(query_type == "update"){
        // Check that newData is provided
        if(q.newData.empty()){
            std::cout<<"Error: UPDATE query must contain new values to set."<<std::endl;
            return false;
        }
        
        // Validate all fields in newData exist in headers
        for(auto& field : q.newData){
            bool found = false;
            for(auto& header : headers){
                if(field.first == header){
                    found = true;
                    break;
                }
            }
            if(!found){
                std::cout<<"Error: Unknown attribute '"<<field.first<<"'."<<std::endl;
                return false;
            }
        }
        
        // Check new values are not empty
        for(auto& field : q.newData){
            if(field.second.empty()){
                std::cout<<"Error: New value for attribute '"<<field.first<<"' cannot be empty."<<std::endl;
                return false;
            }
        }
        
        // Check WHERE clause criteria exists
        if(q.data.empty()){
            std::cout<<"Warning: UPDATE without WHERE clause will update all records."<<std::endl;
        } else {
            // Validate WHERE clause fields
            for(auto& field : q.data){
                bool found = false;
                for(auto& header : headers){
                    if(field.first == header){
                        found = true;
                        break;
                    }
                }
                if(!found){
                    std::cout<<"Error: Unknown attribute in WHERE clause: '"<<field.first<<"'."<<std::endl;
                    return false;
                }
            }
        }
    }
    
    // SELECT validation
    else if(query_type == "select"){
        // Validate WHERE clause fields if provided
        for(auto& field : q.data){
            bool found = false;
            for(auto& header : headers){
                if(field.first == header){
                    found = true;
                    break;
                }
            }
            if(!found){
                std::cout<<"Error: Unknown attribute in WHERE clause: '"<<field.first<<"'."<<std::endl;
                return false;
            }
        }
    }
    
    // DELETE validation
    else if(query_type == "delete"){
        // Validate WHERE clause fields if provided
        for(auto& field : q.data){
            bool found = false;
            for(auto& header : headers){
                if(field.first == header){
                    found = true;
                    break;
                }
            }
            if(!found){
                std::cout<<"Error: Unknown attribute in WHERE clause: '"<<field.first<<"'."<<std::endl;
                return false;
            }
        }
        
        // Check WHERE clause criteria exists
        if(q.data.empty()){
            std::cout<<"Warning: DELETE without WHERE clause will delete all records."<<std::endl;
        }
    }
    
    return true;
}