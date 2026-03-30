#include "structs.h"
#include "engine.h"
#include "parser.h"
#include "storage.h"

#include<iostream>
#include<cctype>
#include<algorithm>

using namespace std;

int main(){
    string input;

    while(true){
        cout << "db> ";
        getline(cin,input);
    
        if(input == "exit") break;

        transform(input.begin(),input.end(),input.begin(),::tolower);
        Query q = parse(input);

        transform(q.type.begin(),q.type.end(),q.type.begin(), ::tolower);
        if(q.type == "insert"){
            insertRecord(q);
        }
        else if(q.type == "select"){
            selectRecord(q);
        }
        else if(q.type == "delete"){
            deleteRecord(q);
        }
        else if(q.type == "update"){
            updateRecord(q);
        }
        else{
            cout<<"Invalid Syntax.\n";
        }
    }
}