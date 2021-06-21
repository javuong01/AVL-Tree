#include <string>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "json.hpp"    //remember to use nlohmann::
#include <bits/stdc++.h>

#include "AVL.h"

int main(int argc, char** argv) {
    

    
    std::ifstream file;     //a file stream is open under the name of file
    file.open(argv[1]);     //The inputted json is opened in the file stream
    nlohmann::json jsonObject;    //A jsonobject called jsonObject is made
    if (file.is_open()) {         //This if statement checks if the file opened actually opened
        file >> jsonObject;       //the file opened in file is put into the json object
    }    

    std::ifstream file2;                //a file stream is open under the name of file2
    file2.open("result.json");     //A new json is made and opened in the file2 stream
    nlohmann::json jsonResult;        //A jsonobject called jsonResult is made
    if (file2.is_open()) {             //This if statement checks if the file opened actually opened
        file2 >> jsonResult;         //the file opened in file2 is put into the json object
    }

    AVL* tree = new AVL();
    int key;

    for (auto itr = jsonObject.begin(); itr != jsonObject.end(); ++itr) {
        if (itr.key() == "metadata"){                                              //If the key that the for loop is iterating through is metadata
            continue;
        }
        if (jsonObject[itr.key()]["operation"] == "Insert") {
            key = jsonObject[itr.key()]["key"];
            tree->Insert(key);
        }
        // jsonResult = tree->JSON();
        // std::cout << jsonResult.dump(2) << "\n End of step \n";
    }   
            
    
    jsonResult = tree->JSON();

    std::cout << jsonResult.dump(2) << "\n";
    


    file.close();
    file2.close();
    delete tree;
}