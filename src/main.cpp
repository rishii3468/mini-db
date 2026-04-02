#include "structs.h"
#include "engine.h"
#include "parser.h"
#include "storage.h"

#include<iostream>
#include<cctype>
#include<algorithm>
#include<fstream>
#include<filesystem>

using namespace std;
namespace fs = std::filesystem;
string index_file = "./data/search_index.bin";

int main(){
    string input;
    search_index.clear();

    if(fs::exists(index_file)){
        loadIndexes(search_index,index_file);
    }else{
        createIndex("id");
        cout<<"created index id"<<endl;
    }
    
    while(true){
        cout << "db> ";
        getline(cin,input);
        
        if(input == "exit"){
            saveIndexes(search_index,index_file);
            break;
        }

        transform(input.begin(),input.end(),input.begin(),::tolower);
        Query q = parse(input);

        transform(q.type.begin(),q.type.end(),q.type.begin(), ::tolower);
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
           createIndex(q.index_column,true);
        }
        else{
            cout<<"Invalid Syntax.\n";
        }
    }
}