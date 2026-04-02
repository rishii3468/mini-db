#include "structs.h"
#include "engine.h"
#include "parser.h"
#include "storage.h"

#include<iostream>
#include<cctype>
#include<algorithm>
#include<fstream>
#include<filesystem>

namespace fs = std::filesystem;
std::string index_file = "./data/search_index.bin";

int main(){
    std::string input;
    search_index.clear();

    if(fs::exists(index_file)){
        loadIndexes(search_index,index_file);
    }else{
        createIndex("id");
        std::cout<<"created index id"<<std::endl;
    }
    
    while(true){
        std::cout << "db> ";
        std::getline(std::cin,input);
        
        if(input == "exit"){
            saveIndexes(search_index,index_file);
            break;
        }

        std::transform(input.begin(),input.end(),input.begin(),::tolower);
        Query q = parse(input);

        std::transform(q.type.begin(),q.type.end(),q.type.begin(), ::tolower);
        if(q.type == "insert"){
            insertRecord(q);
            for(auto& [attribute,_] : search_index){
                createIndex(attribute);
            }
        }
        else if(q.type == "select"){
            selectRecord(q);
        }
        else if(q.type == "delete"){
            deleteRecord(q);
            for(auto& [attribute,_] : search_index){
                createIndex(attribute);
            }
        }
        else if(q.type == "update"){
            updateRecord(q);
            for(auto& [attribute,_] : search_index){
                createIndex(attribute);
            }
        }
        else if(q.type == "index"){
            if(search_index.count(q.index_column)){
                std::cout<<"Index already exists."<<std::endl;
                continue;
            }
           createIndex(q.index_column,true);
        }
        else{
            std::cout<<"Invalid Syntax.\\n";
        }
    }
}