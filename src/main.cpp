#include "structs.h"
#include "engine.h"
#include "parser.h"
#include "storage.h"

#include<iostream>

using namespace std;

int main(){
    string input;

    while(true){
        cout << "db> ";
        getline(cin,input);
    
        if(input == "exit") break;

        Query q = parse(input);

        if(q.type == "INSERT"){
            insertRecord(q);
        }
        else if(q.type == "SELECT"){
            selectRecord(q);
        }
        else if(q.type == "DELETE"){
            deleteRecord(q);
        }
        else{
            cout<<"Invalid Syntax.\n";
        }
    }
}