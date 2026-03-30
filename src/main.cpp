#include "structs.h"
#include "engine.h"
#include "parser.h"
#include "storage.h"

#include<iostream>
#include<cctype>
#include<algorithm>
#include<fstream>

using namespace std;

int main(){
    string input,attribute="id";
    search_index.clear();
    createIndex(attribute);
    while(true){
        cout << "db> ";
        getline(cin,input);
        
        if(input == "exit") break;

        transform(input.begin(),input.end(),input.begin(),::tolower);
        Query q = parse(input);

        transform(q.type.begin(),q.type.end(),q.type.begin(), ::tolower);
        if(q.type == "insert"){
            insertRecord(q);
            search_index.clear();
            createIndex(attribute);
        }
        else if(q.type == "select"){
            selectRecord(q);
        }
        else if(q.type == "delete"){
            deleteRecord(q);
            search_index.clear();
            createIndex(attribute);
        }
        else if(q.type == "update"){
            updateRecord(q);
            search_index.clear();
            createIndex(attribute);
        }
        else{
            cout<<"Invalid Syntax.\n";
        }
    }
}