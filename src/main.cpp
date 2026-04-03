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
            if(!validate(q)){
                continue;
            }
            insertRecord(q);
            for(auto& [attribute,_] : search_index){
                createIndex(attribute);
            }
        }
        else if(q.type == "select"){
            if(!validate(q)){
                continue;
            }
            selectRecord(q);
        }
        else if(q.type == "delete"){
            if(!validate(q)){
                continue;
            }
            deleteRecord(q);
            for(auto& [attribute,_] : search_index){
                createIndex(attribute);
            }
        }
        else if(q.type == "update"){
            if(!validate(q)){
                continue;
            }
            updateRecord(q);
            for(auto& [attribute,_] : search_index){
                createIndex(attribute);
            }
        }
        else if(q.type == "index"){
            if(!validate(q)){
                continue;
            }
            if(search_index.count(q.index_column)){
                std::cout<<"Index already exists."<<std::endl;
                continue;
            }
           createIndex(q.index_column,true);
        }
        if(q.type == "help"){
            std::cout<<"COMMANDS:\n1.INSERT(insert id=1 name=john age=34)\n2.SELECT(select id=4)\n3.DELETE(delete id=4)\n4.UPDATE(update id=4 set name=johndoe)\nINDEX(index age)\n";
        }
        else{
            std::cout<<"Invalid Syntax.\n";
        }
    }
}